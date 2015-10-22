# Memory

## Memory diagram
![virtual addr space](https://cloud.githubusercontent.com/assets/14265605/9994693/1619d9b6-6044-11e5-8c44-7209aedaaca2.png)

## process and the heap
A *process* is a running instance of your program. Each process has its own address space. For example on a 32 bit machine your process gets about 4 billion addresses to play with, however not all of these are valid or even mapped to actual physical memory (RAM). Inside the process's memory you will find the executable code, space for the stack, environment variables, global (`static`) variables and the heap.

If we write a multi-threaded program (more about that later) we will need multiple stacks (one per thread) but there's only ever one heap.

On typical architectures, the heap is part of the *Data segment* and starts just above the code and global variables.

```C
// Ensure our memory is initialized to zero
link_t *link  = malloc(256);
memset(link, 0, 256); // Assumes malloc returned a valid address!

link_t *link = calloc(1, 256); // safer: calloc(1, sizeof(link_t));
```

Or equivalently use: 
```C
void* calloc(size_t num, size_t size);
```

## Zeroing memory
The memory that is *first* returned by ```C sbrk``` is zeroed. But, memory malloc'ed and then *free'd* by ```C free()``` contains data. ```C malloc``` does not always initialize memory to zero for performance reasons. Allocation and deallocatioin of heap memory are common operations so we have to make them fast.

For example:
```C
char* ptr = malloc(300);
// contents is probably zero because we get brand new memory
// so beginner programs appear to work!
// strcpy(ptr, "Some data"); // work with the data
free(ptr);
// later
char *ptr2 = malloc(308); // Contents might now contain existing data and is probably not zero
```

## What is realloc and when would you use it?
`realloc` allows you to resize an existing memory allocation that was previously allocated on the heap (via malloc,calloc or realloc). The most common use of realloc is to resize memory used to hold an array of values.A naive but readable version of realloc is suggested below
```C
void * realloc(void * ptr, size_t newsize) {
  // Simple implementation always reserves more memory
  // and has no error checking
  void *result = malloc(newsize); 
  size_t oldsize =  ... //(depends on allocator's internal data structure)
  if (ptr) memcpy(result, ptr, oldsize);
  free(ptr);
  return result;
}
```
An INCORRECT use of realloc is shown below:
```C
int *array = malloc(sizeof(int) * 2);
array[0] = 10; array[1]; = 20;
// Ooops need a bigger array - so use realloc..
realloc (array, 3); // ERRORS!
array[2] = 30; 
```

The above code contains two mistakes. Firstly we needed 3*sizeof(int) bytes not 3 bytes.
Secondly realloc may need to move the existing contents of the memory to a new location. For example, there may not be sufficient space because the neighboring bytes are already allocated. A correct use of realloc is shown below.
```C
array = realloc(array, 3 * sizeof(int));
// If array is copied to a new location then old allocation will be freed.
```
A robust version would also check for a `NULL` return value. Note `realloc` can be used to grow and shrink allocations. 

## Where can I read more?
See [the man page](http://man7.org/linux/man-pages/man3/malloc.3.html)!

## What is the silliest malloc and free implementation and what is wrong with it?

```C
void* malloc(size_t size)
// Ask the system for more bytes by extending the heap space. 
// sbrk Returns -1 on failure
   void *p = sbrk(size); 
   if(p == (void *) -1) return NULL; // No space left
   return p;
}
void free() {/* Do nothing */}
```
The above implementation suffers from two major drawbacks:
* System calls are slow (compared to library calls). We should reserve a large amount of memory and only occasionally ask for more from the system.
* No reuse of freed memory. Our program never re-uses heap memory - it just keeps asking for a bigger heap.

If this allocator was used in a typical program, the process would quickly exhaust all available memory.
Instead we need an allocator that can efficiently use heap space and only ask for more memory when necessary.

## Placement Strategies

1. A *perfect-fit* strategy finds the smallest hole that is of sufficient size.
* if we can not find a perfectly-sized hole then this placement creates many tiny unusable holes, leading to high fragmentation. It also requires a scan of all possible holes.

2. A *worst-fit* strategy finds the largest hole that is of sufficient size.
* Since Worst-fit targets the largest unallocated space, it is a poor choice if large allocations are required.

3. A *first-fit* strategy finds the first available hole that is of sufficient size.
*  it will not evaluate all possible placements and therefore be faster.

In practice first-fit and next-fit (which is not discussed here) are often common placement strategy. Hybrid approaches and many other alternatives exist.

## Memory allocator
A memory allocator needs to keep track of which bytes are currently allocated and which are available for use.
* `malloc` and `free` are examples of allocators.

## [Buddy Allocator](https://en.wikipedia.org/wiki/Buddy_memory_allocation)
* blocks must be powers of two in size
 * Every memory block in this system has an order. The size of a block of order n is proportional to 2^n, so that the blocks are exactly twice the size of blocks that are one order lower.
 * The smallest block size (predetermined) is then taken as the size of an order-0 block, so that all higher orders are expressed as power-of-two multiples of this size.
  * Typically the lower limit would be small enough to minimize the average wasted space per allocation, but large enough to avoid excessive overhead. 
* all buddies are aligned on memory address boundaries that are powers of two.
* When a larger block is split, it is divided into two smaller blocks, and each smaller block becomes a unique buddy to the other. A split block can only be merged with its unique buddy block, which then reforms the larger block they were split from.
* UNIX uses it for small block sizes
* the system provides two operations:
 1. Allocate(2^k): Finds a free block of size 2^k, marks it as occupied and returns a pointer to it.
 2. Deallocate(B): Marks the previously allocated block B as free and may merge it with others to form a larger free block.

* the largest block size may not span the entire memory of the system:
 * For instance, if the system had 2000K of physical memory and the order-0 block size was 4K, the upper limit on the order would be 8, since an order-8 block (256 order-0 blocks, 1024K) is the biggest block that will fit in memory. (976K wasted)

## Alignment
Many architectures expect multi-byte primitives to be aligned to some multiple of 2^{n}.it's common to require 4-byte types to be aligned to 4-byte boundaries (and 8-byte types on 8-byte boundaries). If multi-byte primitives are not stored on a reasonable boundary (for example starting at an odd address) then the performance can be significantly impacted because it may require two memory read requests instead of one. On some architectures the penalty is even greater - the program will crash with a bus error.

the `glibc` `malloc` uses the following heuristic: " The block that `malloc` gives you is guaranteed to be aligned so that it can hold any type of data. On GNU systems, the address is always a multiple of eight on most systems, and a multiple of 16 on 64-bit systems."

For example, if you need to calculate how many 16 byte units are required, don't forget to round up:

```c
int s = (requested_bytes + tag_overhead_bytes + 15) / 16
```

The additional constant (15) ensures incomplete units are rounded up. Note, real code is more likely to symbol sizes e.g. `sizeof(x) - 1`, rather than coding numerical constant 15.

## Implementing `free`
We have two things to do:
1. clear/mark the block to be ready for next allocation:

If we encode the `use tag` in the lowest bit of `size` metadata, we do:
```c
*p = (*p) & ~1; // clear lowest bit
```

2. coalesce:
duplicate block metadata at the tail of block, so that we can look back in the linked-list.


# Text I/O

## Besides `printf()`, how else can I print strings and single characters?
Use `puts( name );` and `putchar( c )`  where name is a pointer to a C string and c is just a `char`

```C
int puts ( const char * str );
```
Writes the C string pointed by str to the standard output (stdout) and appends a newline character ('\n').

The function begins copying from the address specified (str) until it reaches the terminating null character ('\0'). This terminating null-character is not copied to the stream.

```C
int putchar ( int character );
```
Write character to stdout
Writes a character to the standard stdout.

It is equivalent to calling `putc` with stdout as second argument.

## How do i print to other file systems?
use `fprintf()`
```C
int fprintf ( FILE * stream, const char * format, ... );
```
e.g: 
```C
fprintf( _file_ , "Hello %s, score: %d", name, score);
```

### How do i print data into a C string?
use `sprintf` or `snprintf`

```C
int sprintf ( char * str, const char * format, ... );
```
Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, the content is stored as a C string in the buffer pointed by str.

The size of the buffer should be large enough to contain the entire resulting string (see `snprintf` for a safer version).

A terminating null character is automatically appended after the content.

After the format parameter, the function expects at least as many additional arguments as needed for format.

```c
int snprintf ( char * s, size_t n, const char * format, ... );
```
Write formatted output to sized buffer
Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, the content is stored as a C string in the buffer pointed by s (taking n as the maximum buffer capacity to fill).

If the resulting string would be longer than n-1 characters, the remaining characters are discarded and not stored, but counted for the value returned by the function.

A terminating null character is automatically appended after the content written.

## How do i parse input?
use `scanf`, ``scanf` or `fscanf`.

### from *stdin*:
```c
int scanf ( const char * format, ... );
```
On success, the function returns the number of items of the argument list successfully filled. This count can match the expected number of items or be less (even zero) due to a matching failure, a reading error, or the reach of the __EOF__.

* __EOF__:
End-of-File, a macro definition of type int that expands into a negative integral constant expression (generally, -1). Used as the value returned by several functions in header `<cstdio>` to indicate that the End-of-File has been reached or to signal some other failure conditions. Also used as the value to represent an invalid character. C++ counterpart: `char_traits<char>::eof()`.

e.g:
```c
/* scanf example */
#include <stdio.h>

int main ()
{
  char str [80];
  int i;

  printf ("Enter your family name: ");
  scanf ("%79s",str); // preventing buffer overflow
  printf ("Enter your age: ");
  scanf ("%d",&i);
  printf ("Mr. %s , %d years old.\n",str,i);
  printf ("Enter a hexadecimal number: ");
  scanf ("%x",&i);
  printf ("You have entered %#x (%d).\n",i,i);
  
  return 0;
}
```

### from *string*

```c
int sscanf ( const char * s, const char * format, ...);
```
As if scanf was used, but reading from s instead of the *stdin*.

```c
/* sscanf example */
#include <stdio.h>

int main ()
{
  char sentence []="Rudolph is 12 years old";
  char str [20];
  int i;

  sscanf (sentence,"%s %*s %d",str,&i);
  printf ("%s -> %d\n",str,i);
  
  return 0;
}
```

### from *FILE*
```c
int fscanf ( FILE * stream, const char * format, ... );
```

```c
/* fscanf example */
#include <stdio.h>

int main ()
{
  char str [80];
  float f;
  FILE * pFile;

  pFile = fopen ("myfile.txt","w+");
  fprintf (pFile, "%f %s", 3.1416, "PI");
  rewind (pFile);
  fscanf (pFile, "%f", &f);
  fscanf (pFile, "%s", str);
  fclose (pFile);
  printf ("I have read: %f and %s \n",f,str);
  return 0;
}
```

## Why is `gets` dangerous? Alternatives:
`gets` does not have buffer overflow protection. Use `fget` or `getline` instead.

```c
char * gets ( char * str );
```
The newline character, if found, is not copied into `str`.
A terminating `null` character is automatically appended after the characters copied to `str`.

C standard (2011) has definitively removed this function.

```c
char * fgets ( char * str, int num, FILE * stream );
```
A newline character makes `fgets` stop reading, but it is considered a valid character by the function and included in the string copied to `str`.
A terminating `NULL` character is automatically appended after the characters copied to `str`.
* fgets accepts a stream argument.
* allows to specify the maximum size of `str`.
* includes in the string any ending newline character.

```c
/* fgets example */
#include <stdio.h>

int main()
{
   FILE * pFile;
   char mystring [100];

   pFile = fopen ("myfile.txt" , "r");
   if (pFile == NULL) perror ("Error opening file");
   else {
     if ( fgets (mystring , 100 , pFile) != NULL )
       puts (mystring);
     fclose (pFile);
   }
   return 0;
}
```

```c
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
```
`getline()` reads an entire line from stream, storing the address of the buffer containing the text into `*lineptr`. The buffer is null-terminated and includes the newline character, if one was found.

### How do I use `getline`?
One of the advantages of `getline` is that it will automatically (re-) allocate a buffer on the heap of sufficient size.

```C
// ssize_t getline(char **lineptr, size_t *n, FILE *stream);

 /* set buffer and size to 0; they will be changed by getline*/
char *buffer = NULL;
size_t size = 0;

ssize_t chars = getline(&buffer, &size, stdin);

// Discard newline character if it is present,
if (chars > 0 && buffer[chars -1] =='\n') buffer[chars-1] = '\0';

// Read another line.
// The existing buffer will be re-used, or, if necessary,
// It will be `free`'d and a new larger buffer will `malloc`'d
chars = getline(&buffer, &size, stdin);

// Later... don't forget to free the buffer!
free(buffer);
```

# I/O
Linux redirections can be combined:
`% a.out < inputfile > outputfile`

Open file -> do something with it -> close file.

One example:
We have the input file: `in.list` which contains a name and his/her score, like this:

`
foo 70

bar 90
...
`

we want to add 10 points to each:
```c
FILE *ifp, *ofp;
char *mode = "r";
char outputFilename[] = "out.list";

ifp = fopen("in.list", mode);

if (ifp == NULL) {
  fprintf(stderr, "Can't open input file in.list!\n");
  exit(1);
}

ofp = fopen(outputFilename, "w");

if (ofp == NULL) {
  fprintf(stderr, "Can't open output file %s!\n",
          outputFilename);
  exit(1);
}

char username[9];  /* One extra for nul char. */
int score;

/**
 * feof is a libary function. 
 * It just takes a file pointer and returns a true/false value 
 * based on whether we are at the end of the file.
 * 
 */
while (!feof(ifp)) { // feof is a lib function
  /**
   * to detect invalid input, e.g. biz A+, '
   * in which if we go ahead do fscanf, since there is an invalid token, 
   * the loop does not know what to do so it will cause an infinite loop.
   */
  if (fscanf(ifp, "%s %d", username, &score) != 2)
    break;
  fprintf(ofp, "%s %d", username, score+10);
}

fclose(ifp);
fclose(ofp);
```

Closing a file is very important, esp. with output files since output is often *buffered*.

## Special file pointers
There are 3 special `FILE *`'s that are always defined for a program. They are `stdin` (standard input), `stdout` (standard output) and `stderr` (standard error).

```c
scanf("%d", &val);
```
is equivalent to:
```c
fscanf(stdin, "%d", &val);
```

```c
printf("Value = %d\n", val);
```
is equivalent to:
```c
fprintf(stdout, "Value = %d\n", val);
```

`stderr` is normally associated with the same place as `stdout`; however, redirecting `stdout` does not redirect `stderr`.

For example: `% a.out > outfile`
only redirects stuff going to `stdout` to the file `outfile` ... anything written to `stderr` goes to the screen.

### An example using special file pointers:
```c
void WriteData(FILE *fp)
{
  fprintf(fp, "data1\n");
  fprintf(fp, "data2\n");
  ...
}
```
Now you can do:
```c
WriteData(ofp);
```
and also:
```c
WriteData(stdout);
```
You don't need to write a second version of `WriteData` for `stdout`.

# Pthreads
To remember how to return from function calls, and to store the values of automatic variables and parameters a thread uses a stack.

Your main function (and other functions you might call) have automatic variables. We will store them in memory using a stack and keep track of how large the stack is by using a simple pointer (the "stack pointer").

![stack pointer](http://i.imgur.com/RPblpE1.png)

## How many threads can my process have?
You can have more than one thread running inside a process. You get the first thread for free! It runs the code you write inside 'main'. If you need more threads you can call `pthread_create` to create a new thread using the pthread library. You'll need to pass a pointer to a function so that the thread knows where to start.

The threads you create all live inside the same virtual memory because they are part of the same process. Thus they can all see the heap, the global variables and the program code etc. Thus you can have two (or more) CPUs working on your program at the same time and inside the same process. It's up to the operating system to assign the threads to CPUs. If you have more active threads than CPUs then the kernel will assign the thread to a CPU for a short duration (or until it runs out of things to do) and then will automatically switch the CPU to work on another thread. 
For example, one CPU might be processing the game AI while another thread is computing the graphics output.

## Hello world pthread example
To use pthreads you will need to include `pthread.h` AND you need to compile with `-pthread` (or `-lpthread`) compiler option. This option tells the compiler that your program requires threading support

To create a thread use the function `pthread_create`. This function takes four arguments:
```C
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);
```
* The first is a pointer to a variable that will hold the id of the newly created thread.
* The second is a pointer to attributes that we can use to tweak and tune some of the advanced features of pthreads.
* The third is a pointer to a function that we want to run
* Fourth is a pointer that will be given to our function

The argument `void *(*start_routine) (void *)` is difficult to read! It means a pointer that takes a `void *` pointer and returns a `void *` pointer. It looks like a function declaration except that the name of the function is wrapped with `(* .... )`

Here's the simplest example:
```C
#include <stdio.h>
#include <pthread.h>
// remember to set compilation option -pthread

void *busy(void *ptr) {
// ptr will point to "Hi"
    puts("Hello World");
    return NULL;
}
int main() {
    pthread_t id;
    pthread_create(&id, NULL, busy, "Hi");
    while (1) {} // Loop forever
}
```
If we want to wait for our thread to finish use `pthread_join`
```C
void *result;
pthread_join(id, &result);
```
In the above example, `result` will be `null` because the busy function returned `null`.
We need to pass the address-of result because `pthread_join` will be writing into the contents of our pointer.

