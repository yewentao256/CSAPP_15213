# CSAPP-15213

Learning About CSAPP-15213

**Source**: [https://www.cs.cmu.edu/afs/cs/academic/class/15213-f15/www/schedule.html]

**Videos**: [https://scs.hosted.panopto.com/Panopto/Pages/Sessions/List.aspx#folderID=%22b96d90ae-9871-4fae-91e2-b1627b43e25e%22]

- [CSAPP-15213](#csapp-15213)
  - [Labs](#labs)
    - [Datalab](#datalab)
    - [Bomblab](#bomblab)
    - [Attacklab](#attacklab)
    - [Cachelab](#cachelab)
    - [Shell lab](#shell-lab)
    - [malloc lab](#malloc-lab)
    - [The rest of lab](#the-rest-of-lab)
  - [Class Notes](#class-notes)
  - [Learning Process](#learning-process)

## Labs

Note: the questions can be quite hard, be patient and you'll figure it out!

### Datalab

To implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C

Here we can review the basics of integer and float, For example

```c
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0'
 * to '9') Example: isAsciiDigit(0x35) = 1. isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // for 0x30~0x37, (x >> 3) ^ 0x06(0000 0110) = 0
  // for 0x38,0x39, (x >> 1) ^ 0x1C(0001 1100) = 0
  return !((x >> 3) ^ 0x06) | !((x >> 1) ^ 0x1C);
}
```

Done in 2022/12, see [datalab](datalab/README.md) for more details

### Bomblab

A "binary bomb" is a program provided to students as an object code file. When run, it prompts the user to type in 6 different strings. If any of these is incorrect, the bomb "explodes".

Here we can better understand the assembly and instructions, for example

```c
0x000400ee0 <+0>:     sub    $0x8,%rsp
0x000400ee4 <+4>:     mov    $0x402400,%esi         // move 0x402400 to %esi
0x000400ee9 <+9>:     callq  0x401338 <strings_not_equal>  
0x000400eee <+14>:    test   %eax,%eax              // judge if eax == 1
0x000400ef0 <+16>:    je     0x400ef7 <phase_1+23>  // jump if equal/zero
0x000400ef2 <+18>:    callq  0x40143a <explode_bomb>
0x000400ef7 <+23>:    add    $0x8,%rsp
0x000400efb <+27>:    retq
```

Done in 2022/12, see [bomblab-practice](bomblab/README.md) for more details.

Wanna try another bomb? Do it by yourself! Try a new random bomb, see [bomblab-real](bomblab/bomblab-real.md) for more details.

### Attacklab

To modify the behavior of the targets by developing exploits based on either **code injection** or **return-oriented programming**.

Here we can understand instructions better, and the memory of stack. For example:

```c++
004017a8 <getbuf>:
  4017a8:  48 83 ec 28            sub    $0x28,%rsp
  4017ac:  48 89 e7               mov    %rsp,%rdi
  4017af:  e8 8c 02 00 00         callq  401a40 <Gets>
  4017b4:  b8 01 00 00 00         mov    $0x1,%eax
  4017b9:  48 83 c4 28            add    $0x28,%rsp
  4017bd:  c3                     retq   
```

By making input string like

```c++
00 00 00 00 00 00 00 00    // 64 bit, 8 bytes
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00    // 0x28 bytes
c0 17 40 00                // use `touch1` to cover the return address
```

we can make the program `jump` to another place we want.

Done in 2023/1, see [attacklab-practice](attacklab/attacklab-practice.md) for more details.

Wanna try another attack? Do it by yourself! Try a new random attack, see [attacklab-real](attacklab/README.md) for more details.

### Cachelab

In this lab, we will write a small C program that **simulates the behavior of a cache memory** and **optimize a small matrix transpose function**.

There are code like:

```c
void access_cache(unsigned long address) {
  unsigned int tag = address >> (s + b);
  int s_index = address >> b & ((1 << s) - 1);  // (1 << s)-1 for %
  int eviction_index = -1, max_counter = -1;
  int need_evict = 1;

  // first loop to see whether there is a match, or cold start
  for (size_t i = 0; i < E; i++) {
    if (sets[s_index].lines[i].valid) {
      // valid, compare tag
      if (sets[s_index].lines[i].tag == tag) {
        // hit ...
      } else {
        continue;
      }
    } else {
      // cold start ...
    }
  }

  if (need_evict) {
    // ...
  }
}
```

Done in 2023/2, see [cachelab](cachelab/README.md) for more details.

### Shell lab

In this lab, we will write a simple **Unix shell** that supports job control, including subprocess, signals, foreground background job, interrupts etc.

There are code like:

```c
void sigchld_handler(int sig) {
  pid_t pid;
  int status;
  struct job_t *job;

  sigset_t mask_all, prev_all;
  sigfillset(&mask_all);

  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
    if (sigprocmask(SIG_BLOCK, &mask_all, &prev_all) < 0) {
      perror("sigprocmask error");
      exit(1);
    }
    job = getjobpid(jobs, pid);
    if (WIFEXITED(status)) {
      deletejob(jobs, pid);
    } else if (WIFSIGNALED(status)) {
      printf("Job [%d] (%d) terminated by signal %d\n", job->jid, pid,
             WTERMSIG(status));
      deletejob(jobs, pid);
    } else if (WIFSTOPPED(status)) {
      printf("Job [%d] (%d) stopped by signal %d\n", job->jid, pid,
             WSTOPSIG(status));
      job->state = ST;
    }
    if (sigprocmask(SIG_SETMASK, &prev_all, NULL) < 0) {
      perror("sigprocmask error");
      exit(1);
    }
  }
}
```

Done in 2023/4, see [shlab](shelllab/README.md) for more details.

### malloc lab

In this lab, we will implement our own versions of `malloc`, `free`, and `realloc`.

There are code like:

```c
void *mm_malloc(size_t size) {
  size = get_block_size(size);
  Block *block = find_fit(size);
  if (!block) {
    expand_heap(size > EXPAND_HEAP_SIZE ? size : EXPAND_HEAP_SIZE);
    block = find_fit(size);
    if (!block) {
      return NULL;
    }
  }
  SET_ALLOCATED(block);
  return (void *)((char *)block + sizeof(Block));  // return data to user
}
```

Done in 2023/9, see [malloc_lab](malloclab/README.md) for more details.

### The rest of lab

- [ ] proxylab

## Class Notes

[My Class notes](Class-notes.md)

## Learning Process

Lectures:

- [x] Lecture 01: Course Overview
- [x] Lecture 02: Bits, Bytes, and Integers
- [x] Lecture 03: Bits, Bytes, and Integers (cont.)
- [x] Lecture 04: Floating Point
- [x] Lecture 05: Machine-Level Programming I: Basics
- [x] Lecture 06: Machine-Level Programming II: Control
- [x] Lecture 07: Machine-Level Programming III: Procedures
- [x] Lecture 08: Machine-Level Programming IV: Data
- [x] Lecture 09: Machine-Level Programming V: Advanced Topics
- [x] Lecture 10: Program Optimization
- [x] Lecture 11: The Memory Hierarchy
- [x] Lecture 12: Cache Memories
- [x] Lecture 13: Linking
- [x] Lecture 14: Exceptional Control Flow: Exceptions and Processes
- [x] Lecture 15: Exceptional Control Flow: Signals and Nonlocal Jumps
- [x] Lecture 16: System Level I/O
- [x] Lecture 17: Virtual Memory: Concepts
- [x] Lecture 18: Virtual Memory: Systems
- [x] Lecture 19: Dynamic Memory Allocation: Basic Concepts
- [x] Lecture 20: Dynamic Memory Allocation: Advanced Concepts
- [x] Lecture 21: Network Programming: Part 1
- [x] Lecture 22: Network Programming: Part II
- [x] Lecture 23: Concurrent Programming
- [x] Lecture 24: Synchronization: Basics
- [x] Lecture 25: Synchronization: Advanced
- [x] Lecture 26: Thread-Level Parallelism
- [x] Lecture 27: Future of Computing

Recitations(Optional):

- [x] Recitation 3: Datalab and Data Representations
- [x] Recitation 4: Bomb Lab
- [x] Recitation 5: Attack Lab and Stacks
- [x] Recitation 6: C Review
- [x] Recitation 7: Cache Lab and Blocking
- [x] Recitation 8: Exam Review
- [x] Recitation 9: Shell Lab, Processes, and Signals, and I/O
- [x] Recitation 10: Virtual Memory
- [x] Recitation 11: Malloc Lab
- [x] Recitation 12: Debugging Malloc Lab
- [x] Recitation 13: Proxy Lab
- [x] Recitation 14: Synchronization
- [ ] Recitation 15: Exam review
