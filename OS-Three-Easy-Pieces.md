# Piece 1: [**Virtualization**](http://pages.cs.wisc.edu/~remzi/OSTEP/)
## **Virtualization Part 1: virtualize CPU**
# [Chap 2](http://pages.cs.wisc.edu/~remzi/OSTEP/intro.pdf): Introduction

## OS
A body of software to make it easy to run programs on machines.
A resource manager.
Provides a standard library through system calls.

### Primary methods: 

#### virtualization.
* how to virtualize is a crux.

Provides illusions to programs that they own the entire machine.

#### Concurrency.
* how to build *correct* concurrent programs is a crux.

#### Persistence.
Every process has its own virtual address space. but all processes share one file system.

## Design Goals
* abstraction
* performance: minimize the overheads of the OS
* protection: isolation
* reliability
* energy-efficiency, security, mobility etc.

## History

# [Chap 4](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-intro.pdf): The Abstraction: the process

## Visualization
Low-level machinery: **mechanisms** (implementation)
* low-level methods or protocols that implement a needed piece of functionality.

High-level intelligence: **policies** (interface)
* Policies are algorithms for making some kind of *decision* within the OS.

A general software design: Separation of policy (*which* process to run) and mechanism (*how* to context-switch).

## A process is the abstraction of a running program

*machine state* of a process: 
* address space
* registers
 * program counter(PC) i.e. instruction pointer(IP)
* stack pointer and frame pointer
* I/O info. such as open files

## The API of an OS
* **Create** new processes.
* **Destroy** a process.
* **Wait**
* **Miscellaneous Control**, such as suspend and resume.
* **Status**

### Process creation

### Process states, one example:
* Running
* Ready
* Blocked

![process_state_transition](https://cloud.githubusercontent.com/assets/14265605/10094082/e0ef4f0c-6320-11e5-821d-9e7e266163a5.png)

### the Data Structures
The process list.
**Process Control Block**: a fancy way of talking about a C structure that contains information about each process.

# [Chap 5](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf): Interlude: Process API
 
## Why the odd interface of `fork()` - `exec()` - `wait()`
The separation of `fork()` and `exec()` is essential in building a UNIX shell, because it lets the shell run code *after* the call to `fork()` but *before* the call to `exec()`; this code can *alter the environment* of the about-to-be-run program, and thus *enables* a variety of interesting *features* to be readily built.
* Here, the UNIX designers simply got it right.

The separation of `fork()` and `exec()` allows the shell to do a whole bunch of useful things rather easily. 
For example:
```bash
prompt> wc p3.c > newfile.txt
```
* The way the shell accomplishes this task is quite simple:
when the child is created, *before* calling `exec()`, the shell closes standard output and opens the file `newfile.txt`.

## Other parts of the API
such as `kill()` system call to send **signal** to a process.
* In fact, the entire signals subsystem provides a rich infrastructure to deliver external events to processes, including ways to receive and process those signals.

# [Chap 6](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-mechanisms.pdf): Mechanism: Limited Direct Execution
By *time sharing* the CPU, virtualization is achieved.
* Attaining **performance** while maintaining **control** is thus one of the central challenges in building an operating system.

## Virtualize the CPU: how to efficiently virtualize the CPU with control?

### Problem 1: Restricted Operations
processor modes:
User mode -> **trap** -> Kernel mode -> return-from-trap
* for this purpose, OS provides system calls
* To execute a system call, a program must execute a special **trap** instruction. This instruction *simultaneously jumps* into the kernel and *raises* the privilege level to kernel mode; once in the kernel, the system can now perform whatever privileged operations are needed (**if allowed**), and thus do the required work for the calling process. When finished, the OS calls a special **return-from-trap** instruction, which returns
into the calling user program while *simultaneously jumps-back* to user mode and *reduces* the privilege level.
* hardware uses mechanisms to store and restore register values of user mode program, e.g. x86 uses a per-process **kernel stack**.

#### Trap table
Searching this table to determine What code (*trap handlers*) to run when certain exceptions occurs.
* set up by OS at boot time.

#### Limited Direct Execution Protocol
trap first to raise privilege to run certain system code to handle some situations.

### Problem 2: Switching Between Processes

#### OS regain control of the CPU

* early technology: Cooperative approach makes system calls. The OS *trusts* the processes of the system to behave reasonably.
* Timer Interrupt: during the boot sequence, the OS must start the timer. 
 * the hardware has some responsibility when an interrupt occurs.
  * store register values.

The OS does indeed need to be concerned as to what happens if, during interrupt or trap handling, another interrupt occurs. This, in fact, is the exact topic of the entire second piece of this book, on concurrency.

the OS “baby proofs” the CPU, by first (during boot time) setting up the trap handlers and starting an interrupt timer, and then by only running processes in a restricted mode.

# [Chap 7](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched.pdf): Scheduling: Introduction

## Metrics for the performance of a scheduler
* average turnaround time to processes/jobs
* response time (interactivity) to processes/jobs

**FIFO** **SJF** **STCF** 

*Shortest Time-to-Completion First* (STCF) optimizes turnaround.

## Round-Robin(RR) scheduling algorithm
*time-slicing*: the length of a time slice must be a multiple of the timer-interrupt period.
 * deciding on the length of the time slice presents a trade-off to a system designer, making it long enough to **amortize** the cost of switching without making it so long that the system is no longer responsive.
* optimizes response time but is bad for turnaround.

## Considering I/O
the currently-running job won’t be using the CPU during the I/O; it is *blocked* waiting for I/O completion.

Treat each CPU burst as an independent job, use STCF to better utilize the processor.
* the scheduler makes sure processes that are “interactive” get run frequently. 
* While those interactive jobs are performing I/O, other CPU-bound jobs run.

## Considering the fact that the scheduler does not know the runtime of each process.
by building a scheduler that uses the recent past to predict the future. This scheduler is known as the *multi-level feedback queue*.

# [Chap 8](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-mlfq.pdf): Scheduling: The Multi-level Feedback Queue (MLFQ)

MLFQ is interesting for the following reason: instead of demanding a priori knowledge of the nature of a job, it observes the execution of a job and prioritizes it accordingly. In this way, it manages to achieve the best of both worlds: it can deliver excellent overall performance (similar to SJF/STCF) for short-running interactive jobs, and is fair and makes progress for long-running CPU-intensive workloads.

the MLFQ has a number of distinct queues, each assigned a different priority level. At any given time, a job that is ready to run is on a single queue.

we will just use round-robin scheduling among jobs of the same priority level.

Thus, the key to MLFQ scheduling lies in how the scheduler sets priorities.
* MLFQ varies the priority of a job based on its observed behavior.

If, for example, a job repeatedly relinquishes the CPU while waiting for input from the keyboard, MLFQ will keep its priority high to increase *turnaround*. If, instead, a job uses the CPU intensively for long periods of time, MLFQ will reduce its priority to increase *interactivity*.

## How to change priority?

one of the major goals of the algorithm: because it doesn’t know whether a job will be a short job or a long-running job, it first assumes it might be a short job, thus giving the job high priority. If it actually is a short job, it will run quickly and complete; if it is not a short job, it will slowly move down the queues,
and thus soon prove itself to be a long-running more batch-like process. 
* In this manner, MLFQ approximates SJF(Shortest Job First, or STCF).

## Basic rules for MLFQ (The Algorithm):

1. If Priority(A) > Priority(B), A runs (B doesn’t).

2. If Priority(A) = Priority(B), A & B run in RR.

3. When a job enters the system, it is placed at the highest priority (the topmost queue).

4. Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU. i.e. accumulated allotment to counter gaming the scheduler), its priority is reduced (i.e., it moves down one queue).
 * Without anti-gaming protection, a process can issue an I/O just before a time slice ends and thus dominate CPU time. 
 * With such protections in place, regardless of the I/O behavior of the process, it slowly moves down the queues, and thus cannot gain an unfair share of the CPU.

5. After some time period S, move all the jobs in the system to the topmost queue. (the priority boost to remove starvation of CPU-intensive jobs).
 * what should S be set to? - *voodoo* constant plays magic.

### Tuning MLFQ

![mlfs](https://cloud.githubusercontent.com/assets/14265605/10121126/ddf6a29a-64a2-11e5-8103-a63d4d16530e.png)

E.g. The Solaris MLFQ:
the Time-Sharing scheduling class, or TS, is particularly easy to configure. 
* it provides a set of tables that determine exactly how the priority of a process is altered throughout
its lifetime, how long each time slice is, and how often to boost the
priority of a job

an administrator can muck with this table in order to make the scheduler behave in different ways.

Default values for the table are 
* 60 queues, with slowly increasing time-slice lengths from 20 milliseconds (highest priority) to a few hundred milliseconds (lowest), and priorities boosted around every 1 second or so.

E.g. the FreeBSD scheduler uses a formula to calculate the current priority level of a job, basing it on how much CPU the process has used. 

E.g. some systems allow user advice to help set priorities.
* by using the command-line utility `nice`.

BSD Unix derivatives, Solaris and Windows NT and subsequents use a form of MLFQ.

# [Chap 9](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-lottery.pdf): Scheduling: Proportional Share (lottery scheduling)
Instead of optimizing for turnaround or response time, a scheduler might instead try to guarantee that each job obtain a certain percentage of CPU time.

## Use tickets to represent shares

## Use randomness

1. random avoids strange corner-case behaviors.

2. random requires little state to track; just the minimal of per-process state. i.e. the number of tickets each has.

3. simple and fast.

## Implementation
We need:

1. a good random number generator.

2. a data structure to track the processes of the system (e.g. a list).

3. the total number of tickets

we simply traverse the list, with a simple counter used to help us find the winner:
![lottery_scheduler](https://cloud.githubusercontent.com/assets/14265605/10125344/f0d06796-6535-11e5-80e9-8561940a7745.png)

```c
// counter: used to track if we’ve found the winner yet
int counter = 0;
 
// winner: use some call to a random number generator to
// get a value, between 0 and the total # of tickets
int winner = getrandom(0, totaltickets);
 
// current: use this to walk through the list of jobs
node_t *current = head;
 
// loop until the sum of ticket values is > the winner
while (current) {
    counter = counter + current->tickets;
    if (counter > winner)
    break; // found the winner
    current = current->next;
}
// ’current’ is the winner: schedule it...
```
* The ordering does not affect the (approximate) correctness of the algorithm.

## the “ticket-assignment problem” remains open.

## Stride scheduling: a deterministic fair-share scheduler.

* Use a *Stride*: proportional to the inverse of the number of tickets each job has.
* when you run a process, increment its *pass counter* by its stride.
* at any given time, pick the process to run that has the lowest pass counter so far.
```c
current = remove_min(queue); // pick client with minimum pass
schedule(current); // use resource for quantum
current->pass += current->stride; // compute next pass using stride
insert(queue, current); // put back into the queue
```

Lottery scheduling achieves the proportions probabilistically over time; stride scheduling gets them exactly right at the end of each scheduling cycle.

Stride scheduling is deterministic but Lottery makes it much easier to incorporate new processes, because it requires no global state. 
* Imagine a new job enters in the middle of our stride scheduling example above; what should its pass value be? Should it be set to 0? If so, it will monopolize the CPU. With lottery scheduling, there is no global state per process; we simply add a new process with whatever tickets it has, update the single global variable to track how many total tickets we have, and go from there. In this way, lottery makes it much easier to incorporate new
processes in a sensible manner.

## proportional-share schedulers do not achieve wide-spread adoption as CPU schedulers
1. Such approaches do not particularly mesh well with I/O

2. The hard problem of ticket assignment
 * but can be used in domains where the problem is easy. e.g. a virtualized data center.

# [Chap 10](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-multi.pdf): Multiprocessor Scheduling

## Problems need to be considered:
1. cache coherence.

2. cache affinity.
 * jobs return to the same CPU on which they firstly run.

## Single-queue multiprocessor scheduling (SQMS)
One global queue.
* not scalable in terms of a growing number of CPUs

## Multi-queue Multiprocessor Scheduling (MQMS)
One queue per CPU. Scheduled essentially independently.
* scalable.
* has cache affinity.
 * reuse cache contents.

### A new problem: load imbalance
Imagine a two-CPU machine: If one CPU finishes all its jobs while the other CPU still gets jobs running. The one CPU could be idling, wasting 50% computing resource.

#### How to deal with load imbalance?
**migration** 
##### How??
**work stealing**: With a work-stealing approach, a (source) queue that is low on jobs will occasionally peek at another (target) queue, to see how full it is. If the target queue is (notably) more full than the source queue, the source will “steal” one or more jobs from the target to help balance load.

## Linux multiprocessor schedulers

## **Virtualization Part 2: virtualize Memory**
# [Chap 11](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-dialogue.pdf): Summary Dialogue on CPU virtualization
The OS is a resource manager.
If your scheduler is good at turnaround time, it’s bad at response time, and vice versa.

# [Chap 12](http://pages.cs.wisc.edu/~remzi/OSTEP/dialogue-vm.pdf): A Dialogue on Memory Virtualization
Every address generated
by a user program is a **virtual address**. The OS is just providing an illusion
to each process, specifically that it has its own large and private memory; with
some hardware help, the OS will turn these pretend virtual addresses into real
physical addresses, and thus be able to locate the desired information.
* For the ease of use: virtual address space of the program is large and has lots of room for storage. Programmers don't have to worry about fitting all of the code data into a small, crowded memory.
* For isolation and protection

# [Chap 13](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-intro.pdf): The Abstraction: Address Spaces

The **Address Space**: the running program's view of memory in the system.
* In fact, any address you can see as a programmer of a user-level program is a virtual address.
* only the OS (and the hardware) knows the real truth.

*Transparency* in computer science:  in common usage, a transparent system is one that is hard to notice, not one
that responds to requests as stipulated by the Freedom of Information Act.

## Goals of memory virtualization
* transparency
* efficiency
* protection: isolation among processes

# [Chap 14](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-api.pdf): Interlude: Memory API

## the memory allocation interfaces in Unix

### Types of Memory
**Stack** (automatic) memory: allocation and deallocation are managed *implicitly* by the compiler.
**Heap** memory: *explicitly* handled by you, the programmer.

### The `malloc()` call
```c
#include <stdlib.h>
...
void *malloc(size_t size);
```

You don't have to include `stdlib.h` to use `malloc`: the C library has the code for `malloc()` in it but including `stdlib.h` allows the compiler to check whether you are calling `malloc()` correctly.

`NULL` in C is just a macro for `0`.

### The `free()` call
```c
int *x = malloc(10 * sizeof(int));
...
free(x);
```
* the size of the allocated region is not passed in by the user, 
 * it is tracked by the memory-allocation library itself.

In some cases, it may seem like not calling `free()` is reasonable. For example, your program is short-lived, and will soon exit; in this case, when the process dies, the OS will clean up all of its allocated pages and thus no memory leak will take place per se. While this certainly “works”, it is probably a bad habit to develop.
* Two levels of memory management in the system: 1. by the OS. 2. within each process.

##### use `gdb` and `valgrind`

`malloc()` library calls system calls such as `brk` `sbrk`.
anonymous memory region: `mmap`

`calloc()` `realloc()`

# [Chap 15](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-mechanism.pdf): Mechanism: Address Translation
The generic technique we will use, which you can consider an addition
to our general approach of *limited direct execution*, is something that is
referred to as ***hardware-based address translation*** (***address translation*** for short).
* changing the virtual address provided by the instruction to a physical address
 * performed by the hardware
* the OS manages memory

## Address Translation
### HARDWARE-BASED DYNAMIC RELOCATION
* a *base* register: provides offset for physical address
* a *bound* (*limit*) register: provides bounds for physical address for isolation/protection
* "dynamic" means:
 * the translation happens at run-time
 * we can move the address space even after the process has started running.

In most
cases, the OS just sets up the hardware appropriately and lets the process
run directly on the CPU; Only when the process misbehaves does the OS
have to become involved.

*Base-and-bounds* virtualization suffers from *internal fragmentation* problem. we are going to need more sophisticated machinery, to try to better utilize physical memory and avoid internal fragmentation. Our first attempt will be a slight generalization of base and bounds known as **segmentation**,

# [Chap 16](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-segmentation.pdf): Segmentation

## Crux: How to support a large address space?

### Generalized Base-and-bounds: Segmentation
Instead of having just one base and bounds pair in our **MMU** (Memory Management Unit): have a base and bounds pair per *logical segment* of the address space.
* Place each one of process segments (code, stack, heap etc.) *independently* in different parts of physical memory.

#### Hardware structure of **MMU** requires:
e.g. three process segments: code, stack and heap.
* **MMU** needs three pairs of base-and-bounds registers:

|Segment|Base|Size|Growth Direction|
|:-----:|:--:|:--:|:--------------:|
|Code   |32K |2K  |1               |
|Heap   |34K |2K  |1               |
|Stack  |28K |2K  |0               |

*Segmentation Fault*: arises from a memory access
on a segmented machine to an illegal address. Humorously, the term
persists, even on machines with no support for segmentation at all. 

#### Support for Sharing
We need *protection bits*:

|Segment|Base|Size|Growth Direction|Protection|
|:-----:|:--:|:--:|:--------------:|:--------:|
|Code   |32K |2K  |1               |READ-EXE  |
|Heap   |34K |2K  |1               |READ-WRT  |
|Stack  |28K |2K  |0               |READ-WRT  |

such that the code segment can be mapped into multiple virtual address spaces.

## Problems
* external fragmentation: segments are variable-sized => free memory chopped up into odd-sized pieces.
 * memory allocation algorithms needed: best-fit, worst-fit, first-fit, buddy algorithm etc.
  * a good algorithm simply attempts to minimize external fragmentation but cannot eliminate it.

* segmentation isn’t flexible enough to support our fully generalized, sparse address
space. 
 * e.g. if we have a large but sparsely-used heap all in one
logical segment, the entire heap must still reside in memory in order to be
accessed. 

# [Chap 17](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-freespace.pdf): Free-Space Management

#### Best Fit
Could be called Smallest fit too.
* pro: reduce wasted space
* con: exhaustive search for the correct free block

#### Worst Fit
find the largest chunk, split and return the requested amount;
* pro: leave a lot of big chunks.
* con: exhaustive search
* experiments show it performs badly: leading to excess fragmentation while still having high overheads.

#### First Fit
* pro: speed
 * still walks through but no exhaustive search
* con: polluting the beginning of the free list with small objects.

#### Next Fit
The idea is to spread the searches for
free space throughout the list more uniformly, thus avoiding splintering
of the beginning of the list. The performance of such an approach is quite
similar to first fit, as an exhaustive search is once again avoided.

#### Segregated Lists
##### The basic idea
If a particular application
has one (or a few) popular-sized request that it makes, keep a separate
list just to manage objects of that size; all other requests are forwarded to
a more general memory allocator.

e.g. **SLAB** allocator
* memory chunks suitable to fit *data objects* of certain type or size are *preallocated*.
 * *data objects* are frequent objects for the kernel: semaphores, process descriptors, file objects etc.
* **SLAB** allocator tracks these trunks, known as caches.
 * Destruction does not free memory: only opens a slot which is put in the list of free slots by the slab allocator. 
* Slab represents a *contiguous* piece of memory
 * minimizes segmentation
* The allocation takes place quickly, because the system builds the objects in advance and readily allocates them from a slab.

e.g. Object Caches:
When the kernel boots up, it allocates a number of *object caches* for kernel objects that are likely to be requested frequently.
* the object caches thus are each segregated free lists of a given size and serve memory allocation and free requests quickly.
* When a given cache is running low on free space, it requests some slabs of memory from a more general memory allocator.

#### Buddy Allocation
Making coalescing simple.

In such a system, free memory is first conceptually thought of as one
big space of size 2^N . When a request for memory is made, the search for
free space recursively divides free space by two until a block that is big
enough to accommodate the request is found (and a further split into two
would result in a space that is too small). At this point, the requested
block is returned to the user
* pro: easy to coalesce.
 * the address of each buddy pair only differs by a single bit.
  * which bit is determined by the level in the buddy tree.
 * recursively check its buddy after freed.
* con: internal fragmentation: power-of-two-sized blocks are the only allocation allowed.

#### The real world
Read how [`glibc` allocator](http://www.gnu.org/software/libc/manual/html_node/Memory-Allocation.html) works.

# [Chap 18](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf): Paging: Introduction
Chop up space into variable-size (e.g. code, heap, stack) chunks => segmentation.
Chop up space into fixed-sized (called a **page**) chunks => paging.
* physical memory => an array of fixed-sized slots, called **page frames**.

## Page table
To record where each virtual page of the address space is placed in
physical memory, the operating system usually keeps a *per-process* data
structure known as a **page table**.
* The major role of the page table is to store *address translations*.

An `load` (`movl`) operation:
* **virtual page number(VPN)** => **physical frame number (PFN)** also called **physical page number (PPN)**.  and * **offset** within the page => exact byte on that page.
![address_translation](https://cloud.githubusercontent.com/assets/14265605/10579624/bbe7aebc-763e-11e5-987e-19dc46501951.png)

## Where are page tables stored?
Page tables can be large. (32-bit: 4MB / process)
* because of the size, we don't keep any special on-chip hardware in the **MMU** to store the page table. 
 * stored in memory

## What's actually in the page table?
The page table is just a data structure that is used to map virtual addresses (or really, virtual page numbers) to physical addresses (physical frame numbers). 
* The simplest form is called a *linear page table*, which is just an array.
 * This array is indexed by the virtual page number, looking up the **page-table entry** at that index.

### **page-table entry**
![page-table-entry](https://cloud.githubusercontent.com/assets/14265605/10234447/4082eada-685a-11e5-8504-6b367815642d.png)

* a **Valid bit**: unused space is invalid; cannot be accessed => trap to the OS.
* **Protection bits**: permissions.
* a **Present bit**: on physical memory or swapped to disk.
* a **Dirty bit**: has been modified?
* a **reference bit (accessed bit)**: used to track popularity of pages to optimize *page replacement*.
* etc.
* Finally the **Page Frame Number(PFN)** itself.

Current paging problems:
* too big
* too slow
 * one extra memory reference to first fetch the translation from the page table.
  * must be done for every memory access, including instruction fetch.
  * slows the system down ~two folds.

Solutions: use *caches*:

# [Chap 19](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf): Paging: Faster Translations

## The Crux
How to generally avoid going to memory for translation information?

A hardware: **translation-lookaside buffer** (**TLB**): part of the chip's **MMU** and is simply a hardware **cache** of popular virtual-to-physical address translations.
* a better name: ***address-translation cache***
 * *TLB*s rely upon both spatial and temporal locality for success, which are program properties. If the program of interest exhibits such locality (and many programs do), the **TLB** hit rate will likely be high.
* the **TLB** is found near the processing core and is designed
to be quite fast. When a miss occurs, the high cost of paging is
incurred; the page table must be accessed to find the translation, and an
extra memory reference (or more, with more complex page tables) results.

## Who handles the **TLB** Miss?

Older architecture: e.g. Intel x86 has *hardware-managed **TLB**s*, which uses a fixed *multi-level page table*.

Modern architecture: e.g. **MIPS**. **RISC** (reduced-instruction set computers) has *software-managed **TLB***.
* on a **TLB** miss, the hardware simply raises an exception, which pauses the current
instruction stream, raises the privilege level to kernel mode, and jumps
to a trap handler, which lookups the translation in the page table, use special
“privileged” instructions to update the **TLB**, and return from the trap;
at this point, the hardware retries the instruction (resulting in a **TLB** hit).
 * note the **PC** should point to the instruction that caused the trap; not the one next as in the usual trap cases.
 * the handler code should be stored somewhere not subject to translation, otherwise infinite chain of **TLB** misses could occur.
 
The primary advantage of the software-managed approach is *flexibility*.
* the OS can use any data structure to implement the page table.
Another advantage is *simplicity*.
* the hardware does not have to do much on a miss.

## TLB content
![tlb-content](https://cloud.githubusercontent.com/assets/14265605/10320466/36caf8a6-6c39-11e5-90ec-d704c1f74fe9.png)

## Manage **TLB** contents on a context switch
Solution 1: *flush* **TLB** via software or hardware.
* In either case, the flush operation simply sets all valid bits to 0, essentially clearing the contents of the **TLB**.
 * **TLB** misses cost

Solution 2: **address space identifier (ASID)** as if there is a **PID**

![asid](https://cloud.githubusercontent.com/assets/14265605/10320629/1271b99e-6c3a-11e5-875e-a0294a421e37.png)

* which enables *sharing*:
 * Sharing of code pages (in binaries, or shared libraries) is useful as it reduces the number of physical pages in use, thus reducing memory overheads:

![page-sharing](https://cloud.githubusercontent.com/assets/14265605/10320711/70d8d364-6c3a-11e5-9ec4-6b894efa58b3.png)

## The replacement policy
The goal, of course, being to minimize the miss rate (or increase hit rate).

Typical policies:
* least-recently-used (**LRU**)
* random
 * useful due to its simplicity and ability to avoid corner-case behaviors.

## A real TLB Entry (contains 64 bits)
![mips-tlb](https://cloud.githubusercontent.com/assets/14265605/10320827/1115d3c2-6c3b-11e5-8624-3acf03309e87.png)

we would expect a 20-bit VPN and 12-bit offset (4KB pages, byte-addressable) in our typical virtual address.
However, as you can see in the TLB, there are only 19 bits for the
**VPN**; as it turns out, user addresses will only come from half the address
space (the rest reserved for the kernel) and hence only 19 bits of **VPN**
are needed. The **VPN** translates to *up to* a 24-bit physical frame number
(**PFN**), and hence can support systemswith up to 64GB of (physical)main
memory (2^24 4KB pages).

The **MIPS** provides four such instructions: `TLBP`,
which probes the **TLB** to see if a particular translation is in there; `TLBR`,
which reads the contents of a **TLB** entry into registers; `TLBWI`, which replaces
a specific **TLB** entry; and `TLBWR`, which replaces a random **TLB**
entry. The OS uses these instructions to manage the **TLB**’s contents. It is
of course critical that these instructions are *privileged*;

> RAM isn’t always RAM. 

* accessing a particular page of memory may be costly, particularly if
that page isn’t currently mapped by your **TLB** (called: *exceeding the **TLB** coverage*).

# [Chap 20](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-smalltables.pdf): Advanced Page Tables

Problem: simple linear (array-based) page tables are too big.
* 32-bit address space with 4KB pages: 4MB-page table per process
 * recording both used and unused address spaces

How to get rid of all those invalid regions in the page table instead
of keeping them all in memory?

## Hybrid Approach: Segments and Paging

Thus, a virtual address looks like this:
![segments-and-paging](https://cloud.githubusercontent.com/assets/14265605/10323667/25c0be90-6c4a-11e5-82d3-43ec258e4f45.png)

**Pro**: 
* We now have a *base register* and a *bound(limit) register* to locate the segment (stack, heap and code segments etc.). Since we do not need entries for unallocated pages between the stack and the heap, our page tables could be smaller.

**Con**: 
* requires segmentation, which is not flexible.
* variable sizes of page tables challenges allocators.

## Modern systems: ***Multi-level Page Tables***

A two-level example:
![multi-level-page-table](https://cloud.githubusercontent.com/assets/14265605/10323989/42605c3e-6c4c-11e5-8302-fcd5343efb54.png)

* we added a *level of indirection*: ***page directory***, which points to pieces of the page table.
 * this indirection allows us to place page-table pages wherever we would like in physical memory.

***Pro***:
* space proportional to usage
* flexible

***Con***:
* two loads from memory required to get the right translation information from the page table
 * one for the page directory
 * one for the **PTE** itself
* complex

## More than two levels
We need to make each piece of the page table fit within a single page. When the page directory gets too big, we can add more levels to the directory.

Not that bad: Only upon a **TLB** miss does the hardware need to
perform the full multi-level lookup.

## Inverted Page Tables

The PowerPC: Here, instead of having many page
tables (one per process of the system), we keep a single page table that
has an entry for each physical page of the system. The entry tells us which
process is using this page, and which virtual page of that process maps to
this physical page.

> page tables are just data structures.

## Cache and **TLB**
After getting physical address from the **TLB**, we look for that address (the real physical address) in cache if it is not found there,than we look for main memory.
* Cache stores the actual contents of the memory.
* **TLB** on the other hand, stores only mapping.

# [Chap 21](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf): Swapping: Mechanisms Beyond Physical Memory

*multiprogramming*: running multiple programs “at once”, to better utilize the machine.
The combination of multiprogramming and ease-of-use leads to the need to support using more memory than is physically available.

## Swap Space
Some space reserved on the disk for moving pages back and forth.
* using swap space allows the system to pretend that memory is larger than it actually is.

In modern systems, pages of a program are loaded to memory one page at a time ***when needed***.

> Remember that the hardware first extracts the VPN from the virtual
address, checks the TLB (Translation Lookaside Buffer) for a match (a TLB hit), and if a hit, produces the
resulting physical address and fetches it from memory. This is hopefully
the common case, as it is fast (requiring no additional memory accesses).
If the VPN is not found in the TLB (i.e., a TLB miss), the hardware
locates the page table in memory (using the page table base register)
and looks up the page table entry (PTE) for this page using the VPN
as an index. If the page is valid and present in physical memory, the
hardware extracts the PFN from the PTE, installs it in the TLB, and retries
the instruction, this time generating a TLB hit.

After adding **Swap**, we need a new piece of information in each page-table entry: the **present bit**.
* if the present bit is one: proceeds as above.
* if the present bit is zero: the page is not in memory but rather on disk
 * accessing a page that is not in physical memory: *page miss*, but also called *page fault* (also means illegal accessing).
 * *page-fault handler* will run to service the page fault.

## The page fault

Virtually all systems handle page faults in
software; even with a hardware-managed **TLB**, the hardware trusts the
OS to manage this important duty.

For reasons of performance and simplicity, the OS handles page faults and hardware is happy.
* disk is slow, the overhead of running handler code of the OS is OK.
* the OS already knows the swap space, I/O and other details but it is not trivial for the hardware to understand those.

Note that while the I/O is in flight, the process will be in the **blocked**
state. Thus, the OS will be free to run other ready processes while the
page fault is being serviced. Because I/O is expensive, this overlap of
the I/O (page fault) of one process and the execution of another is yet
another way a multiprogrammed system can make the most effective use
of its hardware. 

### Page-replacement policy

#### Page-fault control flow
Or, an interview question: what happens when a program fetches some data from memory?
The hardware does this:
```
VPN = (VirtualAddress & VPN_MASK) >> SHIFT
(Success, TlbEntry) = TLB_Lookup(VPN)
if (Success == True) // TLB Hit
  if (CanAccess(TlbEntry.ProtectBits) == True)
      Offset = VirtualAddress & OFFSET_MASK
       PhysAddr = (TlbEntry.PFN << SHIFT) | Offset
       Register = AccessMemory(PhysAddr)
   else
       RaiseException(PROTECTION_FAULT)
else // TLB Miss
    PTEAddr = PTBR + (VPN * sizeof(PTE))
    PTE = AccessMemory(PTEAddr)
    if (PTE.Valid == False)
        RaiseException(SEGMENTATION_FAULT)
    else
        if (CanAccess(PTE.ProtectBits) == False)
            RaiseException(PROTECTION_FAULT)
    else if (PTE.Present == True)
        // assuming hardware-managed TLB
        TLB_Insert(VPN, PTE.PFN, PTE.ProtectBits)
        RetryInstruction()
    else if (PTE.Present == False)
        RaiseException(PAGE_FAULT)
```
and the OS does this:
```
PFN = FindFreePhysicalPage()
if (PFN == -1) // no free page found
PFN = EvictPage() // run replacement algorithm
DiskRead(PTE.DiskAddr, pfn) // sleep (waiting for I/O)
PTE.present = True // update page table with present
PTE.PFN = PFN // bit and translation (PFN)
RetryInstruction() // retry instruction
```

*swap daemon* or *page daemon*: the background thread responsible for evicting pages from memory when fewer than *low watermark* pages are available.
* the evicting stops when free pages reaches the *high watermark*.

Those actions all take place
*transparently* to the process. As far as the process is concerned, it is just
accessing its own private, contiguous virtual memory. Behind the scenes,
pages are placed in arbitrary (non-contiguous) locations in physical memory,
and sometimes they are not even present in memory, requiring a fetch
from disk.

# [Chap 22](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys-policy.pdf) Swapping: Policies

## The ***optimal*** replacement policy
> replacing the page that will be accessed furthest 
> in the future is the optimal policy,
> resulting in the fewest-possible cache misses.
> > “A Study of Replacement Algorithms for Virtual-Storage Computer” Laszlo A. Belady IBM Systems Journal 5(2): 78-101, 1966. The paper that introduces the simple way to compute the optimal behavior of a policy (the MIN algorithm).

But the future is hard to know for a general-purpose OS. This policy serves as a comparison point as the perfect policy.

### Types of cache misses

1. Compulsory miss (cold-start miss)

2. Capacity miss

3. Conflict miss (due to set-associativity)


## A simple policy: FIFO

* FIFO simply can’t determine the importance of blocks.
* larger cache might lead to worse FIFO performance.

## Another simple policy: Random

The performance depends on your luck.
One nice property: not having weird corner-case behaviors.

## Using history: **LRU** (Least Recently Used)

***Principle of Locality***: an observation about programs and their behavior and a heuristic that often proves useful in the design of computer systems.
* spatial locality
* temporal locality

### Implementing historical algorithms

Hardware help: an additional time field (stamp) on every memory reference.

but the number pages are huge (> million): accounting for every page is prohibitively expensive.
* Can we instead survive with an approximation?


#### Approximating **LRU**: not accounting time stamps

Hardware support: **use bit** or **reference bit**.
* one use bit per page of the system.
* lives in memory somewhere
* whenever a page is referenced (i.e. read or written), the use bit is set by hardware to 1. The hardware never clears the bit.
 * OS is responsible for clearing the bit.

##### clock algorithm

When a replacement must occur, the OS
checks if the currently-pointed to page P has a use bit of 1 or 0. If 1, the use bit for P set to 0 (cleared), and the clock hand is incremented to the next page (P + 1). The algorithm continues until it finds a use bit that is set to 0.
* nice property: not repeatedly scanning through all of memory looking for an unused page.
 * the worst case is that walked through all pages and cleared them all to 0.
 
To approximate **LRU**. Indeed, any approach which periodically clears the
use bits and then differentiates between which pages have use bits of 1
versus 0 to decide which to replace would be fine.

Some VM systems prefer to evict clean pages over dirty pages.
* because if a page has been modified and is thus dirty, it must be written back to disk when eviction is called on this page; clean page does not need a writing back to disk.
* to support this, the hardware provides a modified bit (a.k.a. ***dirty bit***).
 * can be incorporated into the page-replacement algorithm.

The clock algorithm,
for example, could be changed to scan for pages that are both unused
and clean to evict first; failing to find those, then for unused pages that
are dirty, and so forth.

## Other VM policies

### modern page-replacement policy
*scan resistance*: tries to avoid the worst-case behavior of **LRU**, such as in the looping-sequential workload.
**ARC**

### *page selection* policy
* *demand paging*: loads the page on demand (when it is accessed).
* *prefetching*: 

### OS writes pages out to disk
*clustering* / *grouping* of writes.

## Thrashing
Happens when a process does not have enough pages, the CPU is busy in swapping pages, so much that it can not respond to users' programs. 
Solutions: 
* *admission control* 
* *out-of-memory killer*

Paging in modern systems is so expensive: buy more memory.

# [Chap 23](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-vax.pdf) Case Study: [VAX/VMS](https://cseweb.ucsd.edu/~voelker/cse221/papers/vms.pdf)

***copy-on-write*** (**COW** for short) only do the needed copying before writing; no writing => no need to copying; just reference
* when the OS needs to copy a page from one address space to another, instead of
copying it, it can map it into the target address space and mark it read-only
in both address spaces. If both address spaces only read the page, no
further action is taken, and thus the OS has realized a fast copy without
actually moving any data.
If, however, one of the address spaces does indeed try to write to the
page, it will *trap* into the OS. The OS will then notice that the page is a
**COW** page, and thus (lazily) allocate a new page, fill it with the data, and
map this new page into the address space of the faulting process. The
process then continues and now has its own private copy of the page.
* any sort of shared
library can be mapped copy-on-write into the address spaces of many
processes, saving valuable memory space.

* UNIX `fork()` `exec()` benefit from **COW**

# [Chap 24](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-dialogue.pdf) Summary

> I now really know that all addresses that we can observe within a program
are virtual addresses.

> the TLB truly makes virtualizing memory possible.

> the page table is one of those data structures
you need to know about; it’s just a data structure, though, and that means almost
any structure could be used.

> ...swapping to disk...good to know how page replacement works.

# [Piece 2](http://pages.cs.wisc.edu/~remzi/OSTEP/): **Concurrency**

# [Chap 25](http://pages.cs.wisc.edu/~remzi/OSTEP/dialogue-concurrency.pdf): Dialogue

# [Chap 26](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf): Concurrency and Threads

## Process vs. Thread
In a context switch:
* processes: we saved state to a *process control block* (**PCB**).
* threads: we’ll need one or more *thread control block*s (**TCB**s) to store the state of each thread of a process.

Major difference: 
* in the context switch:
 * For threads, we don't need to switch which page table we are using.
 * For processes, we need to switch the address spaces. 
* stack:
 * multiple threads have multiple stacks in the same address space.

![thread-stack](https://cloud.githubusercontent.com/assets/14265605/10328586/5e0f4c12-6c7c-11e5-8bf0-5d22322071f7.png)

## Key Concurrency Terms

A ***critical section*** is a piece of code that accesses a *shared* resource,
usually a variable or data structure.

A ***race condition*** arises if multiple threads of execution enter the
critical section at roughly the same time; both attempt to update
the shared data structure, leading to a surprising (and perhaps undesirable)
outcome.

An ***indeterminate*** program consists of one or more race conditions;
the output of the program varies from run to run, depending on
which threads ran when. The outcome is thus not deterministic.

To avoid these problems, threads should use some kind of ***mutual
exclusion*** *primitives*; doing so guarantees that only a single thread
ever enters a critical section, thus avoiding races, and resulting in
deterministic program outputs.

The idea behind making a series of actions ***atomic*** is simply expressed
with the phrase “all or nothing”.

# [Chap 27](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf): Thread API in a brief

## Thread Creation
POSIX:
```c
#include <pthread.h>
int
pthread_create(          pthread_t *     thread,
                const pthread_attr_t *   attr,
                          void *         (*start_routine)(void*),
                          void *         arg                      );
```
* why use `void *`: having
a `void` pointer as an argument to the function `start_routine` allows us
to pass in *any* type of argument; having it as a return value allows the
thread to return *any* type of result.
 * `void*` means *anything*.

## Thread Completion
POSIX:
```c
int pthread_join(pthread_t thread, void **value_ptr);
```
* why `void **`: because the `pthread_join()` routine changes the value
of the passed in argument.

## Locks
POSIX:
```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

Bare-bone E.g.
```c
pthread_mutex_t lock;
pthread_mutex_lock(&lock);
x = x + 1; // or whatever your critical section is
pthread_mutex_unlock(&lock);
```
* many threads may be stuck waiting
inside the lock acquisition function at a given time; only the thread with
the lock acquired, however, should call `unlock`.
* this code is **broken**:
 * the lock is not initialized.
   * all locks must be properly initialized to guarantee they have the correct values to begin with.
 * this code fails to check error codes when calling `lock` and `unlock`.

Example wrapper:
```c
// Use this to keep your code clean but check for failures
// Only use if exiting program is OK upon failure
void Pthread_mutex_lock(pthread_mutex_t *mutex) {
    int rc = pthread_mutex_lock(mutex);
    assert(rc == 0);
}
```


### ways to initialize lock
```c
// method 1:
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// method 2: (at run time)
int rc = pthread_mutex_init(&lock, NULL); // arg2: an optional set of attributes; NULL means default
assert(rc == 0); // always check success!
```
As a final cleanup step, call `pthread mutex destroy()`.
* On architectures and implementations where the API has no effect, this will be optimised away, however if the API changes in future to require cleaning up of internal state and if your code does not call it, your code will now have a memory and/or resource leak.
* A `pthread_mutex` is a `struct`, not a C++ `class`. It has no C++ destructor. That's why `pthread_mutex_destroy` exists. The question of what happens when you call `pthread_mutex_init` and don't call `pthread_mutex_destroy` is implementation defined. On some platforms nothing bad will happen. On other systems, it might cause your long running server process to crash. That's why you don't program to the implementation. You program to the API. And the API says you call `pthread_mutex_destroy` when you're done. If you program to the API, you're more likely that your code will actually work in the wild

> [Good Practice](http://stackoverflow.com/questions/14721229/is-it-necessary-to-call-pthread-mutex-destroy-on-a-mutex): Create/Destroy Pattern, Program to the API not to the implementation.

Other useful routines:
```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            struct timespec *abs_timeout);
```
* Both of these versions should generally be avoided;
however, there are a few cases where avoiding getting stuck (perhaps indefinitely)
in a lock acquisition routine can be useful.

## Condition Variables
Condition
variables are useful when some kind of signaling must take place
between threads, if one thread is waiting for another to do something before
it can continue.
* both waiting and signaling threads use a same lock to prevent race condition

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex); // puts the calling thread to sleep, and thus waits for some other thread to signal it.
int pthread_cond_signal(pthread_cond_t *cond);
```
* requires a lock to be held when calling the above routines (see: [Condition Variables](https://github.com/bo-rc/cs241/blob/master/OS-Three-Easy-Pieces.md#parent-waiting-for-child-use-a-condition-variable))
* To use a condition variable, one has to in addition have a lock that is
associated with this condition. When calling either of the above routines,
this lock should be held.

e.g.
```c
// global signal
volatile int ready = 0;
// signal lock and variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// the calling thread:
Pthread_mutex_lock(&lock);
while (ready == 0) // use while other than if to recheck: safer
    Pthread_cond_wait(&cond, &lock); // puting this thread to sleep: Not wasting CPU cycles.
        /* The same lock used also by the signaling thread must be held before being called.
         * The way it works:
         * calling Pthread_cond_wait -> system put this thread to sleep and release the lock m so that
         * the signaling thread can aquire the same lock m and 
         * modify the signal flag. 
         * then, after a signal arrives and awakes this thread,
         * Pthread_cond_wait starts to return and re-lock the same lock m so that 
         * Pthread_mutex_unlock(&m) works as a pair with Pthread_mutex_lock(&m) to unlock the same lock m.
         *
         * the whole purpose is to avoid race condition 
         * w/o the lock m, putting this thread to sleep and signaling this thread to awake could happen at the same time: 
         * this would be a race condition because the relative timing would change the result of the code. */
Pthread_mutex_unlock(&lock);

// some other thread: the code which wakes up the above thread:
Pthread_mutex_lock(&lock); // about to signal: use lock to prevent race condition
ready = 1;
Pthread_cond_signal(&cond);
Pthread_mutex_unlock(&lock);
```

Don't think that you can get away without using condition variables. Don't do this:
```c
// calling thread
while (ready == 0)
    ; // spinning, waste CPU cycles

// waiting thread
ready = 1;
```
Experiments show this style is very buggy. Be aware that condition variables exist and use them. More reasons/examples coming later.

## Compiling and Running
```c
#include <pthread.h>
```

```bash
gcc -o main main.c -Wall -pthread
```

## Tips before going on
* Keep it simple. 
* Minimize thread interactions.
* Initialize locks and condition variables. 
* Check your return codes.
* Be careful with how you pass arguments to, and return values
from, threads. 
 * In particular, any time you are passing a reference to
a variable allocated on the stack, you are probably doing something
wrong.
* Each thread has its own stack. 
 * To share data between threads, the values must be
in the heap or otherwise some locale that is globally accessible.
* Always use condition variables to signal between threads. 
 * While it is often tempting to use a simple flag, don’t do it.
* Use the manual pages.

# [Chap 28](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf): Locks
We want ***Atomic Processing***. But due to interrupts, we couldn't.
* Use ***lock***

## Basic Idea

A *lock* is just a (global) variable (or `struct`), holding the state of the lock.
* The name that the POSIX library uses for a lock is a **mutex**.

In general, we view threads as entities created by the programmer
but scheduled by the OS, in any fashion that the OS chooses.
Locks yield some of that control back to the programmer

### basic usage:
```c
lock_t mutex; // some globally-allocated lock ’mutex’
//...
lock(&mutex); // entering critical section
balance = balance + 1;
unlock(&mutex); // leaving critical section
```

## Pthread Locks

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // can have different locks for different critical sections/variables.

Pthread_mutex_lock(&lock); // wrapper for pthread_mutex_lock()
balance = balance + 1;
Pthread_mutex_unlock(&lock);
```

## Building a lock

With help from hardware and OS.

### Evaluation
* Does it work?
* Fairness
* Performance

Earliest solutions:
```c
void lock() {
    DisableInterrupts();
}

void unlock() {
    EnableInterrupts();
}
```
* By disabling interrupts, a thread can be sure that the code it executes will execute
and that no other thread will interfere with it.

* this is for sure a privileged operation: BAD idea!
* does not work on multiple processors.

Hardware support: *test-and-set instruction* a.k.a. *atomic exchange*, defined by the following C code snippet:
```c
// test-and-set atomic instruction defined by C code snippet
int TestAndSet(int *old_ptr, int new) {
    int old = *old_ptr;
    *old_ptr = new; 
    return old; 
}
```
*  It returns the old
value pointed to by the `ptr`, and simultaneously updates said value to
`new`. The key, of course, is that this sequence of operations is performed
***atomically***. The reason it is called “test and set” is that it enables you
to “test” the old value (which is what is returned) while simultaneously
“setting” the memory location to a new value.

### Spin Lock

#### Hardware (atomic) primitive 1: *atomic exchange instruction (xchg)* (x86) 
A simple spin lock using ***test-and-set***:
```c
typedef struct __lock_t {
    int flag;
} lock_t;

void init(lock_t *lock) {
    // 0 indicates that lock is available, 1 that it is held
    lock->flag = 0;
}

void lock(lock_t *lock) {
        while (TestAndSet(&lock->flag, 1) == 1) // atomically
            ; // spin-wait (do nothing). Spins if held by other thread already
    }

void unlock(lock_t *lock) {
    lock->flag = 0;
}
```
* this code actually works if `*old_ptr = new; // atomic operation` is an atomic operation.
 * x86: `xchg`(atomic exchange instruction)
* By making both the *test* (of the old lock value) and *set* (of the new
value) a single atomic operation, we ensure that only one thread acquires
the lock. And that’s how to build a working mutual exclusion primitive!
* requires a preemptive scheduler not to waste CPU cycles
 * Without preemption, spin locks don’t make much sense on
a single CPU, as a thread spinning on a CPU will never relinquish it.

Evaluation:
* correctness: Yes.
* fairness: a thread spinning may spin forever under contention.
* performance: 
 * single CPU: each of those threads will spin for the duration
of a time slice before giving up the CPU, a waste of CPU cycles.
 * multiple CPUs: Spinning to wait for a lock held on another processor
doesn’t waste many cycles in this case, and thus can be effective.

#### Hardware (atomic) primitive 2: *compare-and-swap* (x86)

```c
// compare-and-swap atomic instruction defined by C code snippet
int CompareAndSwap(int *ptr, int expected, int new) {
    int actual = *ptr;
    if (actual == expected)
        *ptr = new; 
    return actual;
}
```
So that we can build a lock:
```c
void lock(lock_t *lock) {
    while(CompareAndSwap(&lock->flag, 0, 1) == 1)
        ; // spin
}
````

### Hardware (atomic) primitive 3: **load-linked** and **store-conditional** (MIPS, PowerPC, ARM)

```c
// load-linked and store-conditional instructions defined by C snippet code
int LoadLinked(int *ptr) {
    return *ptr;
}

int StoreConditional(int *ptr, int value) {
    if (no one has updated *ptr since the LoadLinked to this address) {
        *ptr = value;
        return 1; // success!
    } else {
        return 0; // failed to update
    }
}
```
So we can build a lock:
```c
void lock(lock_t *lock) {
    while (1) {
        while (LoadLinked(&lock->flag) == 1)
        ; // spin until it’s zero
    if (StoreConditional(&lock->flag, 1) == 1)
        return; // if set-it-to-1 was a success: all done
                // otherwise: try it all over again
    }
}

void unlock(lock_t *lock) {
    lock->flag = 0;
}
````

### Hardware (atomic) primitive 4: **fetch-and-add**

```c
// C definition
int FetchAndAdd(int *ptr) {
    int old = *ptr;
    *ptr = old + 1;
    return old;
}
```
so that we can build the *ticket lock*
```c
typedef struct __lock_t {
    int ticket;
    int turn;
} lock_t;

void lock_init(lock_t *lock) {
    lock->ticket = 0;
    lock->turn = 0;
}

void lock(lock_t *lock) {
    int myturn = FetchAndAdd(&lock->ticket);
    while (lock->turn != myturn)
        ; // spin
}

void unlock(lock_t *lock) {
    FetchAndAdd(&lock->turn);
}
```
* has good fairness
 * thread acquire the lock according to their turn ticket value. (queued)

**Summary**: we need some hardware primitives (like above) to realize locks.

## but, how to avoid spinning?
> ... thread 1 spins and spins until a timer interrupt goes off, thread 0 runs again, which releases the lock.

Hardware support alone cannot solve the problem. We’ll need OS support too!

### a simple approach: Just Yield, assuming an OS primitive `yield()`

```c
void init() {
    flag = 0;
}

void lock() {
    while (TestAndSet(&flag, 1) == 1)
    yield(); // give up the CPU
}

void unlock() {
    flag = 0;
}
```
A thread can be in one of three states:
* running
* ready
* blocked

`yield` is simply a system call that moves the caller from the *running* state to *ready*.
* essentially deschedules itself.

This approach works but has problems
* context switch cost
* not fair: some thread could starve (if the scheduler makes bad choices).

### Using Queues: Sleeping instead of Spinning
We must explicitly exert some control over who gets to acquire
the lock next after the current holder releases it. To do this, we will need a
little more OS support, as well as a queue to keep track of which threads
are waiting to enter the lock.

Solaris: 
* `park()` - put a calling thread to sleep
* `unpark(threadID)` - wake a particular thread

```c
typedef struct __lock_t {
    int flag;
    int guard; // guard operations inside lock/unlock
    queue_t *q;
} lock_t;

void lock_init(lock_t *m) {
    m->flag = 0;
    m->guard = 0;
    queue_init(m->q);
}

void lock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ; // only spins when other thread is in lock/unlock
    if (m->flag == 0) {
        m->flag = 1; // lock is acquired
        m->guard = 0;
    } else {
        queue_add(m->q, gettid());
        setpark(); // about to park, park() may return immediately if another thread releases lock before this thread's park()
        m->guard = 0; 
        // setpark() avoids wakeup/waiting race here.
        park(); // yield the CPU
    }
}

void unlock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1)
        ; // acquire guard lock by spinning
    if (queue_empty(m->q))
        m->flag = 0; // let go of lock; no one wants it
    else
        unpark(queue_remove(m->q)); // next thread returns from park(), entering critical section thus actually equivalent to holding the lock.
    m->guard = 0;
}
```

## Different OS, Different Support
Linux: **futex** locks:
```c
void mutex_lock (int *mutex) {
    int v;
    /* Bit 31 was clear, we got the mutex (this is the fastpath) */
    if (atomic_bit_test_set (mutex, 31) == 0)
        return;
    atomic_increment (mutex);
    while (1) {
        if (atomic_bit_test_set (mutex, 31) == 0) {
            atomic_decrement (mutex);
            return;
        }
        /* We have to wait now. First make sure the futex value
           we are monitoring is truly negative (i.e. locked). */
        v = *mutex;
        if (v >= 0)
            continue;
        futex_wait (mutex, v);
    }
}

void mutex_unlock (int *mutex) {
    /* Adding 0x80000000 to the counter results in 0 if and only if
       there are not other interested threads */
    if (atomic_add_zero (mutex, 0x80000000))
        return;

    /* There are other threads waiting for this mutex,
       wake one of them up. */
    futex_wake (mutex);
}
```
* is a form of **two-phase lock**
 * spinning can be useful, particularly
if the lock is about to be released. So in the first phase, the lock spins for
a while, hoping that it can acquire the lock.
 * However, if the lock is not acquired during the first spin phase, a second
phase is entered, where the caller is put to sleep, and only woken up
when the lock becomes free later.

how real locks are built these days:
some hardware support (in the form of a more powerful instruction) plus some
operating system support (e.g., in the form of `park()` and `unpark()`
primitives on Solaris, or `futex` on Linux).

# [Chap 29](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf): Lock-based Concurrent Data Structures
Adding locks to a data structure to make it usable
by threads makes the structure ***thread safe***.

> CRUX: HOW TO ADD LOCKS TO DATA STRUCTURES

## Concurrent Counters

Simple solution: counter with locks.
* not scalable

***Sloppy Counter***: keep local counters for each CPU and update global counter less frenqently to improve scaling.
* One *single* global logical counter
* *each* CPU maintains a *local* physical counters
* local counter updates the global one when the local reaches a *threshold*.
 * sloppiness: the global logical counter *lags* by `~ the number of CPUs * threashold`.

### implementation
```c
typedef struct _counter_t 
{
    int             global; // global counter
    pthread_mutex_t glock; // global lock
    int             local[NUMCPUS]; // local counters (per cpu)
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int             threshold; // update frequency
} counter_t;

void init(counter_t *c, int threshold)
{
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);

    init i;
    for(i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

void update(counter_t *c, int threadID, int amt)
{
    pthread_mutex_lock(&c->llock[threadID]);
    c->local[threadID] += amt; // assumes amt > 0
    if (c->local[threadID] >= c-> threashold)
    {
        pthread_mutex_lock(&c->glock);
        c->global += c->local[threadID];
        pthread_mutex_unlock(&c->glock);
        c->local[threadID] = 0;
    }
    pthread_mutex_unlock(&c->llock[threadID]);
}

// get: return global amount
int get(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val;
}
```
## Concurrent Linked Lists

### A simple implementation
```c
typedef struct _node_t {
    int             key;
    struct _node_t* next;
} node_t;

typedef struct _list_t {
    note_t*          head;
    pthread_mutex_t lock;
} list_t;

void List_Init(list_t * L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
    node_t *new = malloc(sizeof(node_t));
    if(new == NULL) {
        perror("malloc error");
        return;
    }
    new->key = key;

    pthread_mutex_lock(&L->lock);
    new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
}

int List_lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while(curr) {
        if(curr->key == key) {
            rv = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv;
}
```

A (conceptually scalable method): *hand-over-hand locking* (a.k.a. *lock coupling*): 
* per-node lock.
*  When traversing the
list, the code first grabs the next node’s lock and then releases the current
node’s lock: thus hand-over-hand.

Conceptually make-sense, however in practice the overheads of acquiring/releasing locks for each node is prohibitive. 
* maybe a lock every so many nodes would be worth investigating.

More concurrency isn't necessarily faster.

## Concurrent Queues
There is always a standard method to make a concurrent data structure: add a big lock.

Use two locks and dummy node to separate `enqueue` and `dequeue`:
```c
typedef struct _node_t
{
    int             value;
    struct _node_t * next;
} node_t;

typedef struct _queue_t
{
    node_t *          head;
    node_t *          tail;
    pthread_mutex_t   headLock;
    pthread_mutex_t   tailLock;
} queue_t;

void Queue_Init(queue_t * q)
{
    node_t *tmp = malloc(sizeof(node_t)); // assumes malloc is thread-safe
    tmp->next = NULL;
    q->head = q->tail = tmp; // this is the dummy node
    pthread_mutex_init(&q->headLock, NULL);
    pthread_mutex_init(&q->tailLock, NULL);
}

void Queue_Enqueue(queue_t *q, int value)
{
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;
    
    pthread_mutex_lock(&q->tailLock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tailLock);
}

int Queue_Dequeue(queue_t *q, int *value)
{
    pthread_mutex_lock(&q->headLock);
    node_t *tmp = q->head;
    node_t *newHead = tmp->next;
    if(newHead == NULL)
    {
        pthread_mutex_unlock(&q->headLock);
        return -1; // queue was empty
    }
    *value = newHead->value;
    q->head = newHead;
    pthread_mutex_unlock(&q->headLock);
    free(tmp);
    return 0;
}
```

## Concurrent Hash Table

### Simple implementation w/o handling resizing
```c
#define BUCKETS (101)

typedef struct _hash_t {
    list_t lists[BUCKETS];
} hash_t;

void Hash_Init(hash_t * H) {
    int i;
    for(i = 0; i < BUCKETS; ++i) {
        List_Init(&H->lists[i]);
    }
}

int Hash_Insert(hash_t *H, int key) {
    int bucket = key%BUCKETS;
    return List_Insert(&H->lists[bucket], key);
}

int Hash_Lookup(hash_t *H, int key) {
    int bucket = key%BUCKETS;
    return List_Lookup(&H->lists[bucket], key);
}
```
* this code scales well due to per-bucket locks.

####  performance problems should only be remedied once they exist.
> Premature optimization is the root of all evil. - Knuth

# [Chap 30](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf): Condition Variables
Primitives to build concurrent programs: locks and *mechanisms to check whether a condition is true* (e.g. `join`).

## Crux: how to wait for a condition?
A rudimentary solution: spinning the parent until a global variable is set.
* inefficient and wastes CPU cycles

### Parent waiting for child: use a condition variable
POSIX: 
`pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m);`: assumes a lock is held before being called. releases the lock when called. re-acquires the lock just before returning.
* responsibility: release the lock and put the calling thread to sleep atomically.

`pthread_cond_signal(pthread_cond_t *c);`

```c
int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() { // wait-signal pair: the signal side
    Pthread_mutex_lock(&m); // acquire a lock to prevent race condition with the wait side.
    done = 1;                
    Pthread_cond_signal(&c); 
    Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join() { // wait-signal pair: the wait side
    Pthread_mutex_lock(&m); // acquire the SAME lock to prevent race condition with the signal side
    while(done = 0) // it is always a good idea to use while, rather than if
        Pthread_cond_wait(&c, &m); // assumes a lock is held before being called to prevent race condition: the thread tries to be both signal and wait.
        /* the way it works:
         * calling Pthread_cond_wait -> system put this thread to sleep and release the lock m so that
         * the signaling thread can aquire the same lock m and 
         * modify the signal flag. 
         * then, after a signal arrives and awakes this thread,
         * Pthread_cond_wait starts to return and re-lock the same lock m so that 
         * Pthread_mutex_unlock(&m) works as a pair with Pthread_mutex_lock(&m) to unlock the same lock m.
         *
         * the whole purpose is to avoid race condition 
         * w/o the lock m, putting this thread to sleep and signaling this thread to awake could happen at the same time: 
         * this would be a race condition because the relative timing would change the result of the code. */
    Pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t p;
    Pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent: end\n");
    return 0;
}
```
> Always hold the lock while signaling or waiting, and you will always be in good shape.

## The Producer/Consumer (Bounded Buffer) Problem
This problem posed by Dijkstra leads to the invention of the generalized *semaphore*.

e.g. 
```bash
grep foo file.txt | wc -l
# grep is the producer, wc is the consumer. Between them is an in-kernel bounded buffer.
```

*Mesa semantics* v.s. *Hoare semantics*

Thanks to Mesa semantics, a simple rule to remember with condition
variables is to always use while loops. Sometimes you don’t have to recheck
the condition, but it is always safe to do so; just do it and be happy.

Signaling is clearly needed, but must be more directed. A consumer
should not wake other consumers, only producers, and vice-versa.
* producer threads wait on the condition **empty**, and
signals **fill**. Conversely, consumer threads wait on **fill** and signal **empty**.

Using `while` for conditions is always correct.
* also handles the case where spurious wakeups occur.
 * because it will re-check in `while`.

### Solution to producer/consumer problem
```c
int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1)%MAX;
    count++;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    count--;
    return tmp;
}

cond_t empty, fill;
mutex_t mutex;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; ++i) {
        Pthread_mutex_lock(&mutex);
        while(count == MAX)
            Pthread_cond_wait(&empty, &mutex);
        put(i);
        Pthread_cond_signal(&fill);
        Pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; ++i) {
        Pthread_mutex_lock(&mutex);
        while(count == 0)
            Pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        Pthread_cond_signal(&empty);
        Pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
    }
}
```

## Covering condition
When we do not know to wake which thread.
* use `pthread_cond_broadcast()` instead of `pthread_cond_signal()`

# [Chap 31](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf): Semaphores

One can use semaphores as both locks and condition variables.

POSIX: 
```c
#include <semaphore.h>
sem_t s;
sem_init(&s, 0, 1); // 0: this semaphore is shared between threads in the SAME process
                    // 1: initialized value
```
we can call one of two functions to interact with it: `sem_wait()` and `sem_post()`.
```c
int sem_wait(sem_t *s) {
    // decrement the value of semaphore s by one
    // wait if value of semaphore s is negative
}

int sem_post(sem_t *s) {
    // increment the value of semaphore s by one
    // if there are one or more threads waiting,
    // wake one
}
```
* the value of the semaphore, when negative, is equal to the number of waiting threads. (Dijkstra'a implementation, POSIX doesn't require this.)

## A binary semaphore is a lock

The initial semaphore value should be set to `1`.
```c
sem_wait(&s);
// critical section here
sem_post(&s);
```

## Semaphore as condition variables

The initial semaphore value should be set to `0`.


## The producer/consumer (Bounded Bugger) problem
Semaphore + Mutex:
```c
int buffer[MAX];
int fill = 0;
int use = 0;

void put(int value) {
    buffer[fill] = value; // caution: not atomic
    fill = (fill + 1)%MAX; // caution: not atomic
}

int get() {
    int tmp = buffer[use]; // caution: not atomic
    use = (use + 1)%Max; // caution: not atomic
    return tmp;
}

sem_t empty;
sem_t full;
sem_t mutex;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; ++i) {
        sem_wait(&empty); // semaphore first
        sem_wait(&mutex); // mutex second, otherwise could be a deadlock
        put(i); // caustion: not atomic->surround this line with mutex
        sem_post(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; ++i) {
        sem_wait(&full); // semaphore first
        sem_wait(&mutex); // mutex second, otherwise could be a deadlock
        int tmp = get(); // caustion: not atomic->surround this line with mutex
        sem_post(&mutex);
        sem_post(&empty);
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[]) {
    // ...
    sem_init(&empty, 0, MAX); // MAX buffers are empty to begin with ...
    sem_init(&full, 0, 0);    // ... and 0 are full
    sem_init(&mutex, 0, 1);   // mutex=1 because it is a lock
    // ...
}
```

## Reader-Writer Locks
Different data structure accesses might require different kinds of locking.
* some operations do not change the data; they just observe, thus do not need to be blocked.
 * Critical: AS LONG AS the data is not changed, we should allow as many as observing threads.
  * How to make sure the data is not changed while observing?

### Solution
```c
typedef struct _rwlock_t {
    sem_t lock; // binary semaphore (basic lock)
    sem_t writelock; // used to allow ONE writer or MANY readers
    int readers; // count of readers reading in critical section
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock); // surrounds the following critical section
    rw->readers++; // keeps track of the number of current readers
    if (rw->readers == 1)
        sem_wait(&rw->writelock); // first reader acquires writelock to prevent updating the data while reading
    sem_post(&rw->lock); // surrounds the above critical section
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)
        sem_post(&rw->writelock); // last reader releases writelock
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    sem_post(&rw->writelock);
}
```

## The Dining Philosophers

![diniingphilosophers](https://cloud.githubusercontent.com/assets/14265605/10520621/c786afd6-7330-11e5-95f8-a3a7d6c368c2.png)

### Solution: breaking the symmetry:
```c
// the basic loop of each philosopher:
while(1) {
    think();
    getforks();
    eat();
    putforks();
}

int left(int p) { return p; } // fork at p's left
int right(int p) { return (p+1)%5; } // fork at p's right

sem_t forks[5];

/* wont' work
void getforks() { 
    sem_wait(forks[left(p)]); // deadlock here when all p tries to grab forks
    sem_wait(forks[right(p)]);
}
*/

void getforks() {
    if(p == 4) { // breaking the symmetry
        sem_wait(forks[right(p)]);
        sem_wait(forks[left(p)]);
    } else {
        sem_wait(forks[left(p)]);
        sem_wait(forks[right(p)]);
    }
}

void putforks() {
    sem_post(forks[left(p)]);
    sem_post(forks[right(p));
}
```

## Implementing Semaphores

Zemaphores with locks and CVs:
```c
typedef struct __Zem_t {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

// only one thread can call this
void Zem_init(Zem_t *s, int value) {
    s->value = value;
    Cond_init(&s->cond);
    Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
    Mutex_lock(&s->lock);
    while(s->value <= 0)
        Cond_wait(&s->cond, &s->lock);
    s->value--;
    Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
    Mutex_lock(&s->lock);
    s->value++;
    Cond_signal(&s->cond);
    Mutex_unlock(&s->lock);
}
```

# [Chap 32](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-bugs.pdf): Common Concurrency Problems 

## Types of bugs: based on studies (Lu08) of real applications
![concurrency_bugs](https://cloud.githubusercontent.com/assets/14265605/11102371/b57fe06c-8881-11e5-9e04-56a27e3b0f5f.png)

## Non-Deadlock Bugs
Two major types: **atomicity violation** bugs and **order violation** bugs

### Atomicity violation: a code region is intended to be atomic, but the atomicity is not enforced during execution.
In `MySQL`:
```c
// Thread 1::
if (thd->proc_info) {
// ... // what if interrupt happens here and thread 2 runs
fputs(thd->proc_info, ...); // then dereference a NULL would crash the program
// ...
}

// Thread 2::
thd->proc_info = NULL;
```

*Solution*: add a lock
```c
pthread_mutex_t proc_info_lock = PTHREAD_MUTEX_INITIALIZER;

// Thread 1::
pthread_mutex_lock(&proc_info_lock);
if (thd->proc_info) {
// ...
fputs(thd->proc_info, ...);
// ...
}
pthread_mutex_unlock(&proc_info_lock);

// Thread 2::
pthread_mutex_lock(&proc_info_lock);
thd->proc_info = NULL;
pthread_mutex_unlock(&proc_info_lock);
```

### Order-Violation Bugs: `A` should always be executed before `B`, but the order is not enforced during execution.

```c
// Thread 1::
void init() {
 // ...
 mThread = PR_CreateThread(mMain, ...);
 // ...
}

// Thread 2::
void mMain(...) {
 // ...
 mState = mThread->State;
 // ...
}
```

*Solution*: using condition variables is an easy and robust way to add this style of synchronization into modern code bases.
```c
pthread_mutex_t mtLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mtCond = PTHREAD_COND_INITIALIZER; // condition variable
int mtInit = 0; // state variable

// Thread 1::
void init() {
 // ...
 mThread = PR_CreateThread(mMain, ...);

 // signal that the thread has been created...
 pthread_mutex_lock(&mtLock);
 mtInit = 1;
 pthread_cond_signal(&mtCond);
 pthread_mutex_unlock(&mtLock);
 // ...
}

// Thread 2::
void mMain(...) {
 // ...
 // wait for the thread to be initialized...
 pthread_mutex_lock(&mtLock);
 while (mtInit == 0)
  pthread_cond_wait(&mtCond, &mtLock);
 pthread_mutex_unlock(&mtLock);

 mState = mThread->State;
 // ...
}
```
Note that we could likely use `mThread` as the state variable itself, but do not do so
for the sake of simplicity here. When ordering matters between threads,
condition variables (or semaphores) can come to the rescue.

Non-Deadlock Bugs: 97%

## Deadlock Bugs
![deadlock](https://cloud.githubusercontent.com/assets/14265605/11104771/59ca05d6-8890-11e5-80f5-17f5046cc822.png)

Due to:

1. large code bases easily have circular dependency.
2. *encapsulation* hides details so it hides potentials for deadlocks too.

### Conditions for Deadlocks

Four conditions need to hold at once for a deadlock to occur [C+71]:
* **Mutual exclusion**: Threads claimexclusive control of resources that
they require (e.g., a thread grabs a lock).
* **Hold-and-wait**: Threads hold resources allocated to them(e.g., locks
that they have already acquired) while waiting for additional resources
(e.g., locks that they wish to acquire).
* **No preemption**: Resources (e.g., locks) cannot be forcibly removed
from threads that are holding them.
* **Circular wait**: There exists a circular chain of threads such that
each thread holds one more resources (e.g., locks) that are being
requested by the next thread in the chain.
 * if a system has only one single lock, no deadlock can occur.

Solution is to prevent one of the four conditions from arising.

### Handling Deadlocks (Prevention)

***Circular Wait***: provide **total ordering**
* For example, if there are only
two locks in the system (L1 and L2), you can prevent deadlock by always
acquiring L1 before L2. Such strict ordering ensures that no cyclical wait
arises; hence, no deadlock.

When total ordering is hard to achieve: at least enforce a **partial ordering**.

A *Tip* example:
In some cases, a function must grab two (or more) locks; thus, we know
we must be careful or deadlock could arise. Imagine a function that is
called as follows: `do_something(mutex t *m1, mutex t *m2)`. If
the code always grabs `m1` before `m2` (or always `m2` before `m1`), it could
deadlock, because one thread could call `do_something(L1, L2)` while
another thread could call `do_something(L2, L1)`.
To avoid this particular issue, the clever programmer can use the address
of each lock as a way of ordering lock acquisition. By acquiring locks in
either high-to-low or low-to-high address order, `do_something()` can
guarantee that it always acquires locks in the same order, regardless of
which order they are passed in. The code would look something like this:
```c
if (m1 > m2) { // grab locks in high-to-low address order
 pthread_mutex_lock(m1);
 pthread_mutex_lock(m2);
} else {
 pthread_mutex_lock(m2);
 pthread_mutex_lock(m1);
}
// Code assumes that m1 != m2 (it is not the same lock)
```
By using this simple technique, a programmer can ensure a simple and
efficient deadlock-free implementation of multi-lock acquisition.

Solution problems:
* ordering is just a convention.
* requires knowing the implementation detail of the proper locking order.

***Hold-and-wait***: can be avoided by acquiring all locks at once, atomically.
This could be achieved as follows:
```c
lock(prevention); // guard for atomicity
lock(L1);
lock(L2);
// ...
unlock(prevention); // guard for atomicity
```

Solution problems:
* requires to know locks' mechanism implementation details
* decreases concurrency

***No Preemption***: use `trylock()`
```c
top:
lock(L1);
if (trylock(L2) == -1) {
 unlock(L1);
 goto top;
}
```

Note that another thread could follow the same protocol but grab the
locks in the other order (L2 then L1) and the program would still be deadlock
free.
* this could create a new problem: **livelock**: It is possible
(though perhaps unlikely) that two threads could both be repeatedly attempting
this sequence and repeatedly failing to acquire both locks.
 * *A solution*: add a random delay before looping back and trying
the entire thing over again to decrease the odds of repeated interference
among competing threads.

Solution problems:
* *livelock* potential as described above
* before re-trying we need to release resources created along the path if any.

***Mutual Exclusion***: avoid the need for mutual exclusion at all.
e.g. *wait-free* data structures with hardware support:
Suppose we have an atomic instruction:
```c
int CompareAndSwap(int *address, int expected, int new) {
 if (*address == expected) {
  *address = new;
  return 1; // success
 }
 return 0; // failure
}
```
so that we can atomically increment a value by a certain amount:
```c
void AtomicIncrement(int *value, int amount) {
 do {
  int old = *value;
 } while (CompareAndSwap(value, old, old + amount) == 0);
}
```
For list insertions, traditionally we have to do:
```c
void insert(int value) {
 node_t *n = malloc(sizeof(node_t));
 assert(n != NULL);
 n->value = value;
 lock(listlock); // begin critical section
 n->next = head;
 head = n;
 unlock(listlock); // end of critical section
}
```
Now we can do:
```c
void insert(int value) {
 node_t *n = malloc(sizeof(node_t));
 assert(n != NULL);
 n->value = value;
 do {
  n->next = head;
 } while (CompareAndSwap(&head, n->next, n) == 0);
}
```

### Deadlock avoidance via scheduling
***Avoidance***: using scheduling to guarantee no deadlock.

Can be used for example, in an embedded system where one has full knowledge
of the entire set of tasks that must be run and the locks that they
need.

e.g. for threads (T) and locks(L) like this:

<img style="float: left;" src="https://cloud.githubusercontent.com/assets/14265605/11158837/7d4f69ba-8a20-11e5-9152-96eacdce5a22.png">

The scheduler should do:

<img style="float: left;" src="https://cloud.githubusercontent.com/assets/14265605/11158848/979be7da-8a20-11e5-9f43-40886866dea5.png">

e.g. for threads (T) and locks(L) like this:

<img style="float: left;" src="https://cloud.githubusercontent.com/assets/14265605/11158858/b0504e24-8a20-11e5-8af0-c84d2011d1a4.png">

The scheduler should do:

<img style="float: left;" src="https://cloud.githubusercontent.com/assets/14265605/11158874/c9e47c98-8a20-11e5-8f6c-5d19d795a138.png">

Solution problems:
* requries full knowledge of tasks
* limits concurrency

This approach is not widely used due to the above problems.

### Detect and Recover

Many database systems employ deadlock detection and recovery techniques.
A deadlock detector runs periodically, building a resource graph
and checking it for cycles. In the event of a cycle (deadlock), the system
needs to be restarted. 

> Don't always do it perfectly ...
> Not everything worth doing is worth doing well - <<Soul of a New Machine>>

If it happens only once a year and it is OK to reboot the machine, then you can just live with the deadlock.
* This solutions does not apply in mission-critical systems.


## Summary

Non-deadlock bugs are easy to fix.

For deadlock bugs,  The best solution
in practice is to be careful, develop a lock acquisition order, and
thus prevent deadlock from occurring in the first place.

Locks are problematic by their very nature;
perhaps we should seek to avoid using them unless we truly must.

# [Chap 33](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-events.pdf): Event-based concurrency
Besides using *threads*, ***event-based concurrency*** is another common style for concurrent programming.
* used in both GUI-based applications and some types of internet servers.
 * such as in `node.js`

Problems with *multi-threaded* applications:
* managing concurrency correctly in multi-threaded applications can be challenging
 * missing locks, deadlock, and other nasty problems can arise. 
* In amulti-threaded application, the developer has little or no control over what is scheduled at a givenmoment in time.
 * OS schedules them in a reasonable manner across available CPUs.

> The Crux: building concurrent servers w/o threads

## The basic idea: An event loop
The approach is quite simple: you simply wait for something
(i.e., an “event”) to occur; when it does, you check what type of
event it is and do the small amount of work it requires (which may include
issuing I/O requests, or scheduling other events for future handling,
etc.). That’s it!

Pseudocode:
```c
while (1) {
 events = getEvents();
 for (e in events)
  processEvent(e);
}
```
* the code that processes each event is known as an **event handler**.
* deciding which event to handle next is equivalent to scheduling. 
 * This explicit control over scheduling is one of the fundamental advantages of the eventbased
approach.

*how can an event server tell if a message has arrived for it?* 
* how to receive events?

## An Important API: system calls `select()` (or `poll()`)
These basic primitives give us away to build a non-blocking
event loop,which simply checks for incoming packets, reads from sockets
with messages upon them, and replies as needed.

**Blocking** vs **Non-Blocking** interfaces:
* *Blocking* (or *synchronous*) interfaces do all of theirwork before returning
to the caller.
 * e.g. I/O
* *non-blocking* (or *asynchronous*) interfaces begin some work
but return immediately, thus letting whatever work that needs to be done
get done in the background.

### Using `select()`
e.g. Let’s examine how to use select() to see
which network descriptors have incoming messages upon them.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
 // open and set up a bunch of sockets (not shown)
 // main loop
 while (1) {
  // initialize the fd_set to all zero
  fd_set readFDs;
  FD_ZERO(&readFDs); // Macro to clear the set of file descriptors

  // now set the bits for the descriptors
  // this server is interested in
  // (for simplicity, all of them from min to max)
  int fd;
  for (fd = minFD; fd < maxFD; fd++)
   FD_SET(fd, &readFDs); // Macro to include all of the file descriptors from minFD to maxFD
                         // those fd's represent all the network sockets to which the server is paying attention.

  // do the select
  int rc = select(maxFD+1, &readFDs, NULL, NULL, NULL);

  // check which actually have data using FD_ISSET()
  int fd;
  for (fd = minFD; fd < maxFD; fd++)
  { 
   if (FD_ISSET(fd, &readFDs)) // which of the descriptors have data ready and process the incoming data.
    processFD(fd);
  }
 }
}
```
* why simpler?
 * no locks needed

## A problem: Blocking system calls
System calls such as I/O may block, thus wasting computing resources w/o progress
* A rule in event-based systems: no blocking calls are allowed.

### A Solution: Asynchronous I/O provided by many modern OS
These interfaces enable an application
to issue an I/O request and return control immediately to the caller, before
the I/O has completed.
* additional interfaces enable an application to
determine whether various I/Os have completed.

e.g. **AIO control block** on Mac:
```c
struct aiocb {
 int aio_fildes; /* File descriptor */
 off_t aio_offset; /* File offset */
 volatile void *aio_buf; /* Location of buffer */
 size_t aio_nbytes; /* Length of transfer */
};

int aio_read(struct aiocb *aiocbp); // tries to issue the I/O; if successful, it simply returns right away.

int aio_error(const struct aiocb *aiocbp); // checks whether the request referred to by aiocbp has completed.
/* If it has, the routine returns success (indicated by a zero);
 * if not, EINPROGRESS is returned.
 *
 * an application can periodically poll the system via a call
 * to aio_error() to determine whether said I/O has yet completed.
 */
```
* Problem: if a program has tens or hundreds I/Os issued, should every one be checked repeatedly?
 * The remedy: interrupt-based Unix ***signal***s to inform applications when an AIO completes.

In systems without asynchronous I/O, the pure event-based approach
cannot be implemented.
* but some hybrid method exists ([PDZ99])

## Problems
*State Management*: multi-threaded programming has state stored in thread's stack after a contect switch.
* event-based approach needs to do *manual stack management* to store the state in a data structure when event is triggered so that when the AIO of the event is done, handler code can run with the proper state.

With multiple CPUs, some of the simplicity of the event-based approach disappeared.
* Thus, on modern multicore systems, simple event handling without locks is no longer possible.

The event-based approach is that it does not
integrate well with certain kinds of systems activity, such as paging.
* implicit blocking due to page faults is hard to avoid and thus can lead to large
performance problems when prevalent.

Maintaining the code base is more difficult.

AIO APIs are not well integreated as you think they are.

## Summary
Event-based servers give control of scheduling
to the application itself, but do so at some cost in complexity and
difficulty of integration with other aspects of modern systems (e.g., paging).

# [Chap 34](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-dialogue.pdf): Concurrency Summary
Writing correct concurrent code is hard.
* the [Map-Reduce method](https://en.wikipedia.org/wiki/MapReduce) for writing parallel data analysis code w/o multi-threading.

# Piece 3: [**Persistance**](http://pages.cs.wisc.edu/~remzi/OSTEP/)

# [Chap 35](http://pages.cs.wisc.edu/~remzi/OSTEP/dialogue-persistence.pdf): Dialogue
Making information
persist, despite computer crashes, disk failures, or power outages is a tough and
interesting challenge.

# [Chap 36](http://pages.cs.wisc.edu/~remzi/OSTEP/file-devices.pdf): I/O Devices
> Crux: How should I/O be integrated into systems? How to make device interaction efficient?

## A Canonical Device
All devices have 
* **Hardeware** ***Interface***
 * and ***Protocol*** for typical interaction.
* **Internal Structure**
 * implementation specific; is responsible for implementing the abstraction the device presents to the system.
 * e.g. modern **RAID** controllers might consist of hundreds of thousands of
lines of firmware (i.e., software within a hardware device) to implement
its functionality.

e.g.:
![canonicaldevice](https://cloud.githubusercontent.com/assets/14265605/11169978/65e4f7c8-8b8c-11e5-8194-970252362d65.png)
By reading and writing these registers(*Interface*), the operating system
can control device behavior.

## A Polling Protocol
```c
While (STATUS == BUSY)
; // Polling, wait until device is not busy
Write data to DATA register
Write command to COMMAND register
(Doing so starts the device and executes the command)
While (STATUS == BUSY)
; // Polling, wait until device is done with your request
```
* works but inefficient.

## Solving polling's problem: the invention of **Interrupt**

1. The OS issues a request, puts the calling
process to sleep, and context switch to another task. 

2. When the device is finished with the operation, it will raise a *hardware interrupt*,
causing the CPU to jump into the OS at a pre-determined *interrupt service routine* (**ISR**) or more simply an *interrupt handler*. 
 * The handler is just a piece of operating system code that will finish the request and wake the process waiting for the I/O, which can then proceed as desired.
  * for example, by reading data and perhaps an error code from the device 
 
### Problems and Improving Interrupt
Note that using interrupts is not always the best solution. For example,
imagine a device that performs its tasks very quickly: the first poll usually
finds the device to be done with task. Using an interrupt in this case will
actually slow down the system.
* if a device is fast, it may be best to poll.
* if it is slow, interrupts.
* if not-know, or somethimes fast and sometimes slow, use a **hybrid** that polls for a
little while and then, if the device is not yet finished, uses interrupts. 
 * This *two-phased* approach may achieve the best of both worlds.

Another reason not to use interrupts arises in networks [MR96]. When
a huge stream of incoming packets each generate an interrupt, it is possible
for the OS to *livelock*.
* that is, find itself only processing interrupts
and never allowing a user-level process to run and actually service the
requests.
* use polling occasinally to allow the web server to service some requests before going back to the
device to check for more packet arrivals.

*Coalescing*: waiting a bit before delivering the interrupt to allow multiple interrupts be coalesced into a single interrupt delivery to reduce the overhead of interrupt processing.

## More Efficient Data Movement With **DMA**

With Programmed I/O (PIO), the CPU spends too much time moving data to and from
devices by hand.
* Solution: ***Direct Memory Access (DMA)***
 * A **DMA** engine is essentially a very specific device
within a system that can orchestrate transfers between devices and main
memory without much CPU intervention.

![dma](https://cloud.githubusercontent.com/assets/14265605/11170606/152ab832-8b9f-11e5-826c-f614b3a1fc45.png)

* The copying of data is now handled by the DMA controller.

## Methods of Device Interaction
* The first, oldest method (used by IBM mainframes for many years) is to have **explicit I/O instructions**. 
 * e.g. x86: the `in` and `out` instructions can be used to communicate
with devices. For example, to send data to a device, the caller
specifies a register with the data in it, and a specific port which names the
device. Executing the instruction leads to the desired behavior.

* **memory-mapped I/O**
 * The hardware makes device registers available as if they were memory locations.
  * To access a particular register,
the OS issues a load (to read) or store (to write) the address; the hardware
then routes the load/store to the device instead of main memory.

The memory-mapped approach is nice in that no new instructions are needed
to support it, but both approaches are still in use today.

## Fitting Into The OS: The Device Driver

> Crux: How can we keep most of the OS device-neutral, thus hiding the details
of interactions with different devices? (e.g. SCSI disks, USB drives)

Solution: **Abstraction**
* A piece of software in the OS must know in detail
how a device works: a device driver
 * Any specifics of device interaction are encapsulated within the device driver.

### The linux file system software stack
![file-system-stack](https://cloud.githubusercontent.com/assets/14265605/11170683/01d0a1be-8ba1-11e5-9f49-26469c235d18.png)

* The block read and write requests to the generic
block layer are routed to the appropriate device driver, which
handles the details of issuing the specific request.

*Downdside*: being generic means we could lost specific features of a device due to comformatioin to an simpler interface.

Drivers represent > 70% of the kernel code.
* for any give installation, most of that code may not be active.
* divers are a primary contributor to kernel crashes.
 * divers are usually written by amateurs rather than kernel developers.

## Case Study: A Simple IDE Disk Driver

*Interface*: four types of registers:
* control
* command block
* status
* error

```assembly
// The IDE Interface
Control Register:
 Address 0x3F6 = 0x80 (0000 1RE0): R=reset, E=0 means "enable interrupt"

Command Block Registers:
 Address 0x1F0 = Data Port
 Address 0x1F1 = Error
 Address 0x1F2 = Sector Count
 Address 0x1F3 = LBA low byte
 Address 0x1F4 = LBA mid byte
 Address 0x1F5 = LBA hi byte
 Address 0x1F6 = 1B1D TOP4LBA: B=LBA, D=drive
 Address 0x1F7 = Command/status

Status Register (Address 0x1F7):
 7      6     5     4   3    2    1    0
 BUSY READY FAULT SEEK DRQ CORR IDDEX ERROR

Error Register (Address 0x1F1): (check when Status ERROR==1)
 7    6  5   4    3   2    1    0
 BBK UNC MC IDNF MCR ABRT T0NF AMNF

 BBK = Bad Block
 UNC = Uncorrectable data error
 MC = Media Changed
 IDNF = ID mark Not Found
 MCR = Media Change Requested
 ABRT = Command aborted
 T0NF = Track 0 Not Found
 AMNF = Address Mark Not Found
```

A simplified IDE driver:
```c
static int ide_wait_ready() { 
/* ensure the drive is ready before issuing a request to it. */
 while (((int r = inb(0x1f7)) & IDE_BSY) || !(r & IDE_DRDY))
  ; // loop until drive isn’t busy
}

static void ide_start_request(struct buf *b) { 
/* send a request (and perhaps data, in the case of a write) to the disk. */
 ide_wait_ready();
 outb(0x3f6, 0); // generate interrupt
 outb(0x1f2, 1); // how many sectors?
 outb(0x1f3, b->sector & 0xff); // LBA goes here ...
 outb(0x1f4, (b->sector >> 8) & 0xff); // ... and here
 outb(0x1f5, (b->sector >> 16) & 0xff); // ... and here!
 outb(0x1f6, 0xe0 | ((b->dev&1)<<4) | ((b->sector>>24)&0x0f));
 if(b->flags & B_DIRTY){
  outb(0x1f7, IDE_CMD_WRITE); // this is a WRITE
  outsl(0x1f0, b->data, 512/4); // transfer data too!
 } else {
  outb(0x1f7, IDE_CMD_READ); // this is a READ (no data)
 }
}

void ide_rw(struct buf *b) { 
/* queues a request (if there are others pending), 
 * or issues it directly to the disk (via ide start request())
 */
 acquire(&ide_lock);
 for (struct buf **pp = &ide_queue; *pp; pp=&(*pp)->qnext)
  ; // walk queue
 *pp = b; // add request to end
 if (ide_queue == b) // if q is empty
  ide_start_request(b); // send req to disk
 while ((b->flags & (B_VALID|B_DIRTY)) != B_VALID)
  sleep(b, &ide_lock); // wait for completion
 release(&ide_lock);
}

void ide_intr() { 
/* is invoked when an interrupt takes place; it
 * reads data from the device (if the request is a read, not a write), wakes the
 * process waiting for the I/O to complete, and (if there are more requests
 * in the I/O queue), launches the next I/O via ide start request().
 */
 struct buf *b;
 acquire(&ide_lock);
 if (!(b->flags & B_DIRTY) && ide_wait_ready() >= 0)
  insl(0x1f0, b->data, 512/4); // if READ: get data
 b->flags |= B_VALID;
 b->flags &= ˜B_DIRTY;
 wakeup(b); // wake waiting process
 if ((ide_queue = b->qnext) != 0) // start next request
  ide_start_request(ide_queue); // (if one exists)
 release(&ide_lock);
}
```

## Summary 
Interrupts, DMA, and related
ideas are all direct outcomes of the nature of fast CPUs and slow
devices.

OS interacts with a device: two techniques
* Interrupt solves polling's problem
* DMA controller solves moving data effeciency problem.

Two approaches to access device registers
* explicit I/O
* memory-mapped I/O

A device driver does the abstraction.

# [Chap 37](http://pages.cs.wisc.edu/~remzi/OSTEP/file-disks.pdf): Hard Disk Drives

## The interface
The drive consists of a large number of sectors (512-byte blocks), each of which can be read or written.
* Multi-sector operations are possible.
  * indeed, many file systems will read or write 4KB at a time (or more). 
  * a single 512-byte write is *atomic*.

Locality assumption applies to hard disks.

## Basic Geometry
A drive that rotates at 10,000 RPM means
that a single rotation takes about 6 milliseconds (6 ms).

## A simple Hard Disk
On writes, the drive has a choice: should it acknowledge the write has
completed when it has put the data in its memory, or after the write has
actually been written to disk? The former is called ***write back*** caching
(or sometimes immediate reporting), and the latter ***write through***. Write
back caching sometimes makes the drive appear “faster”, but can be dangerous;
if the file system or applications require that data be written to
disk in a certain order for correctness, write-back caching can lead to
problems.

## I/O Time: doing the math
T(I/O) = T(seek) + T(rotation) + T(transfer)

The rate of I/O (R(I/O)):
R(I/O) = Size(transfer) / T(I/O)

## Disk Scheduling
Due to high cost of I/O, given a set of I/O requests, the disk scheduler examines the requests and decides which one to schedule next.

Becuase the length of disk operations can be estimated accurately, the disk scheduler is able to follow the
principle of SJF (shortest job first) in its operation.
* this coulde lead to *starvation*
  * Solution: *elevator* algorithm: sweeping in one direction then sweeping back in one direction.
   * not optimal: not considering rotation

Solution: **SPTF** (Shortest Positioning Time First)
> In engineering, it turns out “it depends” is almost always the answer, reflecting that trade-offs are part of the life of the engineer.
> when you don’t know an answer to your boss’s question, you might want to try this gem.

On modern drives, both seek and rotation are roughly equivalent.
* but OS does not know this thus SPTF is usually performed inside the disk controller.

# [Chap 38](http://pages.cs.wisc.edu/~remzi/OSTEP/file-raid.pdf): Redundant Arrays of Inexpensive Disks (RAIDs)
At a high level, a RAID is very much a specialized
computer system: it has a processor, memory, and disks; however,
instead of running applications, it runs specialized software designed to
operate the RAID.

*The RAID Mapping Problem*: 
```
Disk = A % number_of_disks
Offset = A / number_of_disks
```

## RAID Level 4: Saving Space with Parity
Determine parity using `XOR`:
![raid-4](https://cloud.githubusercontent.com/assets/14265605/11230676/76cc3f84-8d66-11e5-9b86-64269aec3202.png)
![raid-4-xor](https://cloud.githubusercontent.com/assets/14265605/11230705/ac1582c2-8d66-11e5-94df-259ca3430d1a.png)
* bitwise `XOR` across each bit of the data blocks, put the result into the corresponding bit slot in the parity block.
* We can reconstruct the information if only one disk fails.

*small-write* Problem: Even though the data disks could be accessed in
parallel, the parity disk prevents any parallelism from materializing;

## RAID Level 5: Rotating Parity
In order to remove the parity-disk bottleneck for RAID-4:
![raid-5](https://cloud.githubusercontent.com/assets/14265605/11230834/40ba9baa-8d68-11e5-8934-232b5479d2c4.png)

We can generally assume that given a large number of random requests, we
will be able to keep all the disks about evenly busy. If that is the case,
then our total bandwidth for small writes will be `N/4 * R MB/s`. The factor
of four loss is due to the fact that each RAID-5 write still generates 4 total
I/O operations, which is simply the cost of using parity-based RAID.

## Summary
![raid-summary](https://cloud.githubusercontent.com/assets/14265605/11230875/de406cc4-8d68-11e5-9b21-de3f5516458f.png)

To conclude, if you strictly want performance and do not care about
reliability, striping is obviously best. If, however, you want random I/O
performance and reliability, mirroring is the best; the cost you pay is in
lost capacity. If capacity and reliability are your main goals, then RAID-
5 is the winner; the cost you pay is in small-write performance. Finally,
if you are always doing sequential I/O and want to maximize capacity,
RAID-5 also makes the most sense.

# [Chap 39](http://pages.cs.wisc.edu/~remzi/OSTEP/file-intro.pdf): Interlude: File and Directories
Pieces of the virtualization puzzle:
* the **process**: a virtualization of the CPU
* the **address space**: a virtualization of the memory
* **persistent storage**: a virtualizatioin of a persistent file system which makes sure that when you request the data again, you get what you put there in the first place.

First abstraction: ***file***: a linear array of bytes
* low-level name: ***inode number***

Second abstraction: ***directory***

## The file system interface

### *Creating files*: `open()`
```c
int fd = open("foo", O_CREAT | O_WRONLY | O_TRUNC);
```
* the older way of creating a file is to call `creat()`:
* `int fd = creat("foo");
  * equivalent to `open()` with the following flags: `O CREAT | O WRONLY | O TRUNC` 

A ***file descriptor*** is just an integer, private per process, and is used in UNIX systems to access files.

Each running process already has three files open
* *standard input*: which the process can read to receive input (fd = 0)
* *standard output*: which the process can write to in order to dump information to the screen (fd = 1)
* *standard error*: which the process can write error messages to (fd = 2)

### *Reading and Writing files*: `read()` `write()`
```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

Use the `strace` linux command:
```
prompt> strace cat foo
...
open("foo", O_RDONLY|O_LARGEFILE) = 3
read(3, "hello\n", 4096) = 6
write(1, "hello\n", 6) = 6
hello
read(3, "", 4096) = 0
close(3) = 0
...
prompt>
```

#### Reading And Writing, But Not Sequentially: `lseek()`
```c
#include <sys/types.h>
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
```
* `whence` can be: `SEEK_SET`, `SEEK_CUR` and `SEEK_END`

####  Writing Immediately with `fsync()`
```c
int fd = open("foo", O_CREAT | O_WRONLY | O_TRUNC);
assert(fd > -1);
int rc = write(fd, buffer, size);
assert(rc == size);
rc = fsync(fd);
assert(rc == 0);
```
* `fsync()` forces all dirty (i.e., not yet written) data to disk.
* you also need to `fsync()` the directory that contains the file foo. Adding this step ensures not only that the file itself is on disk, but that the file, if newly created, also is durably a part of the directory.

### Renaming files: `rename()`
`prompt> strace mv foo bar`
* using `strace`: mv uses the system call `rename(char *old, char *new)`, which takes precisely two arguments: the original name of the file (`old`) and the new name (`new`).

The `rename()` call is that it is (usually) implemented as an atomic call with respect to system crashes.

e.g. the way a real editor might work when you inserted oneline to a text file:
```c
int fd = open("foo.txt.tmp", O_WRONLY|O_CREAT|O_TRUNC); // temperal work on a tmp file
write(fd, buffer, size); // write out new version of file
fsync(fd); // force write
close(fd); // done with the fd, close it to save resources
rename("foo.txt.tmp", "foo.txt"); // atomically swaps the new file into place, 
                                  // while concurrently deleting the old version of the file, 
                                  // and thus an atomic file update is achieved.
```

### Getting Information About Files: file's metadata: `stat()` `fstat()`

Use the `stat()` or `fstat()` system calls.
* take a pathname (or file descriptor) to a file and fill in a stat structure as seen here:
```c
struct stat {
 dev_t st_dev; /* ID of device containing file */
 ino_t st_ino; /* inode number */
 mode_t st_mode; /* protection */
 nlink_t st_nlink; /* number of hard links */
 uid_t st_uid; /* user ID of owner */
 gid_t st_gid; /* group ID of owner */
 dev_t st_rdev; /* device ID (if special file) */
 off_t st_size; /* total size, in bytes */
 blksize_t st_blksize; /* blocksize for filesystem I/O */
 blkcnt_t st_blocks; /* number of blocks allocated */
 time_t st_atime; /* time of last access */
 time_t st_mtime; /* time of last modification */
 time_t st_ctime; /* time of last status change */
};
```
To see this information, you can use the command line tool `stat`:
```c
prompt> echo hello > file
prompt> stat file
File: ‘file’
Size: 6 Blocks: 8 IO Block: 4096 regular file
Device: 811h/2065d Inode: 67158084 Links: 1
Access: (0640/-rw-r-----) Uid: (30686/ remzi) Gid: (30686/ remzi)
Access: 2011-05-03 15:50:20.157594748 -0500
Modify: 2011-05-03 15:50:20.157594748 -0500
Change: 2011-05-03 15:50:20.157594748 -0500
```
* the info. are in a data structure called **inode**.

### Removing files: `unlink()`
```
prompt> strace rm foo
...
unlink("foo") = 0
...
```
* why a name *unlink*?
When the file system unlinks file, it
checks a *reference count* within the inode number.
Only when the reference count reaches zero
does the file system also *free the inode* and related data blocks, and thus
truly “delete” the file.

### Making Directories: `mkdir()`

The format of the *directory* is considered file system *metadata*.
* You can never write to a directory directly.
* you can only update a directory indirectly by, e.g., creating files, directories, or other object types within it.

```
prompt> strace mkdir foo
...
mkdir("foo", 0777) = 0
...
```
* This creates an empty directory: still has two entries: `.` and `..`.

### Reading Directories: `opendir()`, `readdir()`, and `closedir()`
Let's implement Unix `ls`:
```c
struct dirent {
 char d_name[256]; /* filename */
 ino_t d_ino; /* inode number */
 off_t d_off; /* offset to the next dirent */
 unsigned short d_reclen; /* length of this record */
 unsigned char d_type; /* type of file */
};

int main(int argc, char *argv[]) {
 DIR *dp = opendir(".");
 assert(dp != NULL);
 struct dirent *d;
 while ((d = readdir(dp)) != NULL) {
  printf("%d %s\n", (int) d->d_ino, d->d_name);
 }
 closedir(dp);
 return 0;
}
```

### Deleting Directories: `rmdir()`

### Hard Links: reference counting of inodes: `link()` `unlink()`
Unix command `ln` (`link()`) creates another name and refers it to the same inode number of the orignal file. 
* no file is duplicated.
```
prompt> echo hello > file
prompt> cat file
hello
prompt> ln file file2
prompt> cat file2
hello

prompt> ls -i file file2
67158084 file
67158084 file2 # same inode number, same file
prompt>
```

When you create a file, you are really doing two things:
* First, making an *inode structure*
  * which tracks virtually all relevant information about the file, including its size, where its blocks are on disk, and
so forth. 
* Second, *linking* a human-readable name to that file, and putting that link into a directory.

You can’t create one to a directory.
* for fear that you will create a cycle in the directory tree.
you can’t hard link to files in other disk partitions.
* because inode numbers are only unique within a particular file system, not across file systems.

### Symbolic Links: soft link
Unix command `ln -s`

#### Hard link vs. Soft link
* a soft link is actually another file itself.
* a soft link file has variable file sizes
  * depending on the path of the linked-to file: soft link stores the *pathname* of the linked-to file.
* creating a soft link does not increase the reference count.
 * deleting does not decrease the reference count.

### Making and Mounting a File System: `mkfs` and `mount`
How to create a full directory tree:
* step 1: make a file system: `mkfs`
* step 2: make the file system accessible: `mount`
`prompt> mount -t ext3 /dev/sda1 /home/users`
* mount unifies all file systems into one tree, making naming uniform and convenient.

# [Chap 40](http://pages.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf): File System Implementation

The file system is pure software.

## A simple file system (**vsfs**)

Two aspects:
* data structure of a file system
* access methods of a file system

![vsfs](https://cloud.githubusercontent.com/assets/14265605/11261005/1d594c00-8e36-11e5-8e5d-c481f880c707.png)
* We divide the disk into 4KB blocks.
* Superblock: contains info. about this particular file system, e.g. number of inodes and data blocks, where the inode table begins etc.
* inode/data bitmap: each bit is used to indicate whether the corresponding object/block is free (0) or in-use (1).
  * our **free list** of the file system

### The inode
The name *inode* is short for *index node*, it has a historical reason.
* dnodes, fnodes etc. on other systems

***Inode*** holds the ***metadata*** for a given ***file***.
e.g. The Ext2 Inode:

|Size | Name | What is this inode field for?|
|:---:|:----:|:----------------------------:|
|2 |mode |can this file be read/written/executed?|
|2 |uid |who owns this file?|
|4 |size |how many bytes are in this file?|
|4 |time |what time was this file last accessed?|
|4 |ctime |what time was this file created?|
|4 |mtime |what time was this file last modified?|
|4 |dtime |what time was this inode deleted?|
|2 |gid |which group does this file belong to?|
|2 |links_count |how many hard links are there to this file?|
|4 |blocks |how many blocks have been allocated to this file?|
|4 |flags |how should ext2 use this inode?|
|4 |osd1 |an OS-dependent field|
|60| block |a set of disk pointers (15 total)|
|4 |generation |file version (used by NFS)|
|4 |file_acl |a new permissions model beyond mode bits|
|4 |dir_acl |called access control lists|
|4 |faddr |an unsupported field|
|12| i_osd2 |another OS-dependent field|

Design of the inode is one key part of file system design. 

Each inode is implicitly referred to by a number (called the ***inumber***),
which we’ve earlier called the ***low-level name*** of the file.
* given *inumber*, we can *locate* the inode.

![inode](https://cloud.githubusercontent.com/assets/14265605/11261349/4a5c111c-8e39-11e5-9425-f205f8dd92ce.png)

The sector address `iaddr` of the inode block can be calculated as follows:
```
blk = (inumber * sizeof(inode_t)) / blockSize;
sector = ((blk * blockSize) + inodeStartAddr) / sectorSize;
```














