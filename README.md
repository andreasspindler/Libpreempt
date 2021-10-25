---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# NAME

*Libpreempt* -- C++ framework for embedded systems and realtime programming

# DESCRIPTION

The project consists of two main components:

- the C++ real-time scheduler test suite

- the C++ framework *Libpreempt*

The name "preempt" is short for preemptive. The name is a reminder that
real-time threads are only preempted by threads with higher priorities (FIFO) or
the same priority (RR), or they deliberately call `sched_yield()`

## Real-time scheduler test suite

These tests will verify several attributes of the Linux real-time scheduler with
regard to the POSIX scheduling policies *SCHED_FIFO* and *SCHED_RR*.

To start the tests:

``` sh
 > sudo make
```

Make compiles all programs in the tests / folder for C ++ 14 and C ++ 17 and
executes each compilation 100 times, counting the failed and successful
attempts. Make is prefixed with *sudo* because unless the user has **scheduling
privileges** executing programs that use one of the real-time scheduling
policies will fail (permission denied).

If the PREEMPT_RT patches are installed all tests should complete 100%,
otherwise the rate of successful tests should be over 95% because POSIX does not
require a real-time scheduler. For example, here is the output on an Ubuntu VM
without preempt patches:

Example:

``` sh
 > uname -v
#60~20.04.1-Ubuntu SMP Thu May 6 09:52:46 UTC 2021
 > sudo make
./pudding.sh -D build 100
### multivm Libpreempt(all) *** writing 'out/multivm-all/Makefile'
### multivm Libpreempt(all) *** WARNING: running under VM
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 'Linux'
### multivm Libpreempt(all) *** executing command '100' [Mon, 25 Oct 2021 18:23:39 +0200]
### multivm Libpreempt(all) *** c++14 c++17 debug
### multivm Libpreempt(all) *** 9 tests => 18 target(s) => 1800 total run(s)
#   1/  18: sched_01_other.c++14.debug                      100 run(s)          0 bad        100 good
#   2/  18: sched_01_other.c++17.debug                      100 run(s)          0 bad        100 good
#   3/  18: sched_02_fifo.c++14.debug                       100 run(s)          0 bad        100 good
#   4/  18: sched_02_fifo.c++17.debug                       100 run(s)          1 bad         99 good
#   5/  18: sched_03_fifo.c++14.debug                       100 run(s)         31 bad         69 good
#   6/  18: sched_03_fifo.c++17.debug                       100 run(s)         29 bad         71 good
#   7/  18: std_01_thread.c++14.debug                       100 run(s)          0 bad        100 good
#   8/  18: std_01_thread.c++17.debug                       100 run(s)          0 bad        100 good
#   9/  18: std_02_nonstatic.c++14.debug                    100 run(s)          0 bad        100 good
#  10/  18: std_02_nonstatic.c++17.debug                    100 run(s)          0 bad        100 good
#  11/  18: std_03_array.c++14.debug                        100 run(s)          0 bad        100 good
#  12/  18: std_03_array.c++17.debug                        100 run(s)          0 bad        100 good
#  13/  18: std_04_lambda.c++14.debug                       100 run(s)          0 bad        100 good
#  14/  18: std_04_lambda.c++17.debug                       100 run(s)          0 bad        100 good
#  15/  18: std_05_scheduling.c++14.debug                   100 run(s)          0 bad        100 good
#  16/  18: std_05_scheduling.c++17.debug                   100 run(s)          0 bad        100 good
#  17/  18: task_01.c++14.debug                             100 run(s)          0 bad        100 good
#  18/  18: task_01.c++17.debug                             100 run(s)          0 bad        100 good
### multivm Libpreempt(all) *** 96.600% (1800 runs = 1739 good + 61 bad + 0 missing)
```

Every test has the form of a C++ file and has been carefully written in a way
that (1) it runs one specific scenario and (2) is human readable.

*tests/std_xxx.cpp*
: C++ standard library. Currently tests exist for *std::thread* and other
  classes dealing with concurrency.

*tests/task_xxx.cpp*
: Tests for classes contain *std::thread* or *pthread* members with the
  intention of starting these as needed or based on events.

*tests/sched_xxx.cpp*
: Scheduling strategies for tasks. These culminate in easy-to-use classes for
  the actual application.

*tests/process_xxx.cpp*
: Processes with [real-time] task schedulers. Also memory management and IPC.

As one can see categories are based on each another:

```
       std ==> task ===|
                       |===> scheduling ==> process
   pthread ==> task ===|
```

## Real-time framework

*Libpreempt* is a C++ library build the notion of **threads**, **tasks** and
**schedulers**.

Thread
: Independent units of execution within a process. Threads are to time what
  containers are to data.

Task
: Any class with at least one thread attribute. Tasks can run standard threads
  and real-time threads.

Scheduler
: A scheduler class allows tasks to run under a clearly defined algorithm.

Unlike the C++ standard library this library is designed for POSIX systems.
`preempt::thread` has the same interface as `std::thread` but with an optional
priority and a scheduling policy. If 0 and SCHED_OTHER is used (the defaults)
then it works like `std::thread`. 

Both classes, `std::thread` and `preempt::thread` spawn a system thread in the
constructor and run in cooperation with other threads within a process.

## Unit test driver (Pudding)

Pudding is the driver for the unit tests and implemented in a single script. It
compiles a matrix of executables for each single test, runs them and summarizes
results.

More specifically, this is how the `pudding.sh` script works. It assumes that
each *.cpp*-file in *tests/* is a separate test. It then spans these tests over
several language standards, optimization levels and CPU microarchitectures and
in this way generates many executable files.

The script can execute these files repeatedly while counting successful runs.
Essentially this means all `assert()`s hold true, neither `std::abort()` nor
`std::terminate()` was called and the process returns 0 (`EXIT_SUCCESS`).

As soon as something changes with the hardware, OS, compiler, libraries etc. the
hard-won tests can be rebuild and trust can be regained again.

Pudding works nicely on top of **Docker images** and **Yocto Linux Bitbake
recipes**, with Docker or Yocto providing the system basis and Pudding the basis
for the programming techniques used in the actual application.

Etymology:

The name was inspired by the expression "the proof is in the pudding."
Generally, it is used to say the best test of a pudding is to eat it. The real
worth or effectiveness of something can only be determined by putting it to the
test by trying or using it, over and over again, standards, versions and
promises aside.

Since it operates inside a computer `pudding.sh` expects that no process will
fail *ever*. Either they all succeed or there are bugs.

# EXAMPLES

## Executing tests

Let's assume we have 5 C++ files, each with a `main` function, 3 optimization
levels (*debug Os O3*) and 2 language standards (*c++14 c++17*):

``` sh
 $ alias pudding=./pudding.sh
 $ pudding build 100        # compile 30 executables and run each 100 times
### W530 Libpreempt(all) *** c++14 c++17 debug O3 Os
### W530 Libpreempt(all) *** 5 tests => 25 targets => 2500 runs
### W530 Libpreempt(all) *** BEGIN 100 [Mon, 20 Sep 2021 13:59:22 +0200]
process_00.c++14.O3                     100 run(s)          0 bad        100 good
process_00.c++14.Os                     100 run(s)          0 bad        100 good
process_00.c++14.debug                  100 run(s)          0 bad        100 good
process_00.c++17.O3                     100 run(s)          0 bad        100 good
   .
   .
   .
### W530 Libpreempt(all) *** END 100 [Mon, 20 Sep 2021 21:05:30 +0200]
### Libpreempt(all) *** 100.000% (15000 runs = 15000 good + 0 bad + 0 missing)
### Libpreempt(all) *** OK
```

## Running the test script

``` sh
 $ alias pudding=./pudding.sh
 $ pudding -h                 # command-line options help
 $ pudding build              # compile all files under tests/
 $ pudding build 10           # dto. plus run each executable 10 times
 $ pudding -f std 10          # limit to category 'std'
 $ pudding -f pthread 10
 $ pudding -f task 10
 $ pudding -f sched 10
 $ pudding -f process 10
 $ pudding -q 10              # quiet: no informational output
 $ pudding -q -q 10           # dto. and no statistical output about runs
 $ pudding 10 2>/dev/null     # get rid of warning and error messages
```

## Add a new test

First create a new C++ file in the *tests* directory, for example,
*tests/std_hello.cpp*:

``` c++
#include <iostream>

void main(int argc, char *argv[]) {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
```

Then run either

``` sh
 $ pudding build 10
```

## Add a new test categoy

Let's say we have a new topic "shrama" we work on. Again, create a new C++ file
in the *tests* directory, for example, *tests/shrama_01.cpp*. This will be
already seen by the default flavor "all":

``` sh
 $ pudding build 10
```

To be able to work only on that topic edit the resouce file *.puddingrc*:

``` sh
case ${TestFlavor}; in
  shrama)
    CxxTests=($(find_tests $TestDir 'shrama_*.cpp'))
    ;;
esac
```

Now you can use this flavor:

``` sh
 $ ./pudding.sh -f shrama build 10
```

## Cross-compiling

To cross-compile or optimize set the CPU micro-architecture you define a new
flavor like "cross-compile*". The *Makefile* and the output files are created in
a directory *out/HOSTNAME-FLAVOR/* where *HOSTNAME* is the name of the build
machine and *FLAVOR* the argument passed to *-f*.

For example:

``` sh
    out/
        W530-native/
            process_0.c++14.debug
            process_0.c++14.O0
            process_0.c++14.O1
            process_0.c++14.O2
            process_0.c++14.O3
            process_0.c++14.Ofast
        W530-iA32/
            process_0.c++14.debug
            process_0.c++14.O0
            process_0.c++14.O1
            process_0.c++14.O2
            process_0.c++14.O3
            process_0.c++14.Ofast
```

In *.puddingrc* you modify the global variables *CCStandards* and *CCBaseFlags*
according to the flavor. See `pudding.sh -h` for details.

## Running tests inattentively

A special use case is running all tests very often, like over-night or even when
we going to vacation:

``` sh
 $ pudding afk
```

This command runs tests on a sequence of Fibonacci numbers up to 46368. A
quicker version is `test` which runs the sequence only up to 21 (enough to get a
cup of tea).

``` sh
 $ pudding test
```

# FAQ

## Why can real-time tests fail?

If the code is running under a RTOS and Linux RT-throttling is disabled, the
only other thing known to mess with the real-time scheduling policies is the
memory manager. To disable pagefaults all pages must be locked before letting
`pthread_create()` create FIFO or RR threads:

``` c++
#include <preempt/process.h>

int main(int argc, char *argv[])
{
  preempt::this_process::begin_real-time();
  // can safely start real-time threads
  preempt::this_process::end_real-time();
  return 0
}
```

Basically `begin_real-time()` locks all process memory so that the VMM can't
interrupt the process.

## Is a Linux real-time kernel required to use real-time scheduling?

No. Real-Time scheduling is POSIX function available on all Linux kernels. A
kernel built with the real-time patches is needed if:

- You want to run software with very low latency settings that require real-time
  performance that can only be achieved with an RT kernel.

- Your hardware configuration triggers poor latency behaviour which might be
  improved with an RT kernel.
  
## How to check if PREEMPT_RT patches are installed?

``` sh
 $ uname -v
 #60~20.04.1-Ubuntu SMP PREEMPT Thu May 6 09:52:46 UTC 2021
                         ^
                         |
                         |
                         this word must be printed
```

## Does Yocto-Linux has a real-time-kernel?

Yes.

## What is an RTOS?

**The basis of an RTOS is a real-time scheduler for threads and processes**. A
RT scheduler operates in O(1) time or, under Linux, as CFS (*Completely Fair
Scheduler*). While an RTOS requires an RT scheduler it also needs predictable
"real-time" behaviors in the other parts of the OS too.

The specific description of "real-time" is that processes have minimum response
time guarantees. **The system should behave as predictably as possible.** This
means that it should always take up the same period of time, regardless of the
data to be processed, and this period must be measurable.

**In order to achieve predictability**, the tasks that a computer executes in
parallel, must be distributed to the underlying CPU cores in such a way that no
task is disadvantaged. A task is not disadvantaged if its preset priority,
affinity and scheduling policy is always adhered to by the OS. Only real-time
tasks have a priority and a scheduling policy.

## How predictable is an RTOS?

It is usually sufficient if the system is **predictable for a specific,
practical problem** (application) and not for all conceivable questions.

## How does Preemptive Multitasking work for real-time systems?

Preemptive multitasking creates the illusion that multiple processes and threads
run concurrently on a single processor, while they are actually only assigned a
small time slice. If the time slice is exceeded, the process or thread is
"preempted", i.e. interrupted and has to wait for its next time slice.

Real-Time threads are an exception here. They are only interrupted if a thread
with a higher priority (FIFO) or the same priority (RR) is to run, or if
`sched_yield()` is called explicitly.

This means that a real-time thread can take over and paralyze the entire system
if it does not return quickly enough or in the case of longer jobs voluntarily
calls `sched_yield()`.

For this reason, RT throttling is activated under Linux: by default 5% of the
CPU time is also assigned to non-RT threads per second.

## How does Cooperative Multitasking work for real-time systems?

> Cooperative multitasking, also known as non-preemptive multitasking, is a
> style of computer multitasking in which the operating system never initiates a
> context switch from a running process to another process. Instead, processes
> voluntarily yield control periodically or when idle or logically blocked in
> order to enable multiple applications to be run concurrently.
>
> *Cooperative multitasking*
> https://en.wikipedia.org/wiki/Cooperative_multitasking

The principle is also used to schedule jobs attached to threads in real-time
systems. Each job is expected to complete within a give time slice, for example,
200ms. If the thread can guarantee this it does not matter much if the scheduler
is implemented preemptively or cooperatively.

# APPENDIX

## Some details about POSIX real-time scheduling


`SCHED_FIFO` is a simple scheduling algorithm without time slicing. `SCHED_RR`
is a simple scheduling algorithm with time slice for processes/threads with the
same priority. `SCHED_FIFO` and `SCHED_RR` processes/threads belong to the
category of the real-time (RT) processes/threads.

Real-Time processes/threads are in charge of critical tasks whose execution
cannot be interrupted (usually involved in multimedia processing). Because
real-time threads prevent any non-real-time threads from running, in general their
life-time should be [very] short.

## Some details about Pthreads

- The thread **may or may not have started when `pthread_create()` returns** but
  `SCHED_FIFO` threads are always executed in the order of their creation.

- On **multicore systems** the kernel will not run multiple threads on multiple
  cores simultaneously, which would violate the defined behavior. A thread
  scheduled `SCHED_FIFO` or `SCHED_RR`, when selected for running, will continue to use the CPU
  until either it is blocked by an I/O request, it is preempted by a higher
  priority `SCHED_FIFO` or `SCHED_RR`, or it calls `sched_yield()`.

- `SCHED_FIFO` can be used only with static priorities higher than 0, which
  means that when a `SCHED_FIFO` becomes runnable, it will always immediately
  preempt any currently running `SCHED_OTHER`, `SCHED_BATCH`, or `SCHED_IDLE`.

https://man7.org/linux/man-pages/man7/sched.7.html

# AUTHOR/COPYRIGHT

Andreas Spindler <info@andreasspindler.de>

For copyright and license information see file UNLICENSE.

https://github.com/andreasspindler/Libpreempt

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
