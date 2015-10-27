# Chapter 1: History and Standards
## 1.1 A Brief History of UNIX and C 
## 1.2 A Brief History of Linux 
### 1.2.1 The GNU Project 
### 1.2.2 The Linux Kernel 
## 1.3 Standardization 
### 1.3.1 The C Programming Language
### 1.3.2 The First POSIX Standards
### 1.3.3 X/Open Company and The Open Group 
### 1.3.4 SUSv3 and POSIX.1-2001 
### 1.3.5 SUSv4 and POSIX.1-2008 
### 1.3.6 UNIX Standards Timeline 
### 1.3.7 Implementation Standards
### 1.3.8 Linux, Standards, and the Linux Standard Base
## 1.4 Summary

# Chapter 2: Fundamental Concepts 
## 2.1 The Core Operating System: The Kernel
## 2.2 The Shell
## 2.3 Users and Groups
## 2.4 Single Directory Hierarchy, Directories, Links, and Files 
## 2.5 File I/O Model
## 2.6 Programs
## 2.7 Processes
Each process has an environment list, which is a set of environment variables that are
maintained within the user-space memory of the process.

Environment variables are created with the `export` command in most shells (or
the `setenv` command in the C shell), as in the following example:
```bash
export MYVAR='Hello world'
```

### Resource limits
Using the `setrlimit()` system call

## 2.8 Memory Mappings
The `mmap()` system call creates a new memory mapping in the calling process’s virtual
address space.

The memory in one process’s mapping may be shared with mappings in other pro-
cesses.
* *private mapping*: modifications to the contents of the mapping are not visible to other processes and
are not carried through to the underlying file.
* *shared mapping*: visible and carried through

## 2.9 Static and Shared Libraries

## 2.10 Interprocess Communication and Synchronization 
**IPC** includes:
* *signals*: to indicate that an event has occurred.
* *pipes* and *FIFO*: to transfer data between processes.
* *sockets*: to transfer data from one process to another, either
on the same host computer or on different hosts connected by a network.
* *file locking*: allows a process to lock regions of a file in order to prevent
other processes from reading or updating the file contents
* *message queues*: to exchange messages (packets of data) between
processes.
* *semaphores*: to synchronize the actions of processes.
* *shared memory*: allows two or more processes to share a piece of memory.
When one process changes the contents of the shared memory, all of the other
processes can immediately see the changes.

## 2.11 Signals




## 2.12 Threads
## 2.13 Process Groups and Shell Job Control
## 2.14 Sessions, Controlling Terminals, and Controlling Processes
## 2.15 Pseudoterminals
## 2.16 Date and Time
## 2.17 Client-Server Architecture
## 2.18 Realtime
## 2.19 The /proc File System
## 2.20 Summary

# Chapter 3: System Programming Concepts

# Chapter 4: File I/O: The Universal I/O Model

# Chapter 5: File I/O: Further Details

# Chapter 6: Processes

# Chapter 7: Memory Allocation

# Chapter 8: Users and Groups

# Chapter 9: Process Credentials

# Chapter 10: Time

# Chapter 11: System Limits and Options

# Chapter 12: System and Process Information

# Chapter 13: File I/O Buffering

# Chapter 14: File Systems

# Chapter 15: File Attributes

# Chapter 16: Extended Attributes
# Chapter 17: Access Control Lists
# Chapter 18: Directories and Links
# Chapter 19: Monitoring File Events 
# Chapter 20: Signals: Fundamental Concepts
# Chapter 21: Signals: Signal Handlers
# Chapter 22: Signals: Advanced Features
# Chapter 23: Timers and Sleeping
# Chapter 24: Process Creation
# Chapter 25: Process Termination
# Chapter 26: Monitoring Child Processes
# Chapter 27: Program Execution
# Chapter 28: Process Creation and Program Execution in More Detail
# Chapter 29: Threads: Introduction
# Chapter 30: Threads: Thread Synchronization
# Chapter 31: Threads: Thread Safety and Per-Thread Storage
# Chapter 32: Threads: Thread Cancellation
# Chapter 33: Threads: Further Details
# Chapter 34: Process Groups, Sessions, and Job Control
# Chapter 35: Process Priorities and Scheduling
# Chapter 36: Process Resources
# Chapter 37: Daemons
# Chapter 38: Writing Secure Privileged Programs
# Chapter 39: Capabilities
# Chapter 40: Login Accounting
# Chapter 41: Fundamentals of Shared Libraries
# Chapter 42: Advanced Features of Shared Libraries
# Chapter 43: Interprocess Communication Overview
# Chapter 44: Pipes and FIFOs
# Chapter 45: Introduction to System V IPC
# Chapter 46: System V Message Queues
# Chapter 47: System V Semaphores
# Chapter 48: System V Shared Memory 
# Chapter 49: Memory Mappings 
# Chapter 50: Virtual Memory Operations
# Chapter 51: Introduction to POSIX IPC
# Chapter 52: POSIX Message Queues 
# Chapter 53: POSIX Semaphores
# Chapter 54: POSIX Shared Memory
# Chapter 55: File Locking
# Chapter 56: Sockets: Introduction
# Chapter 57: Sockets: UNIX Domain
# Chapter 58: Sockets: Fundamentals of TCP/IP Networks
# Chapter 59: Sockets: Internet Domains
# Chapter 60: Sockets: Server Design
# Chapter 61: Sockets: Advanced Topics
# Chapter 62: Terminals
# Chapter 63: Alternative I/O Models
# Chapter 64: Pseudoterminals
