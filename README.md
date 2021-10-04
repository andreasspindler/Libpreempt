---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# NAME

*Libpreempt* -- C++ framework for embedded systems and realtime programming

# DESCRIPTION

The project consists of three components:
- the C++ framework *Libpreempt*
- the unit test tool *Pudding*
- the C++ realtime test suite

Quick start:

``` sh
 $ make
```

## Libpreempt

*Libpreempt* is a C++ library build the notion of **threads**, **tasks** and
**schedulers**.

Thread
: Independent units of execution within a process. Threads are to time what
  containers are to data.

Task
: Any class with at least one thread attribute. Tasks can run standard threads
  and realtime threads.

Scheduler
: A scheduler class allows tasks to run under a clearly defined algorithm.

The library is build around the class
`preempt::realtime_thread<SCHEDULING_POLICY>` which is basically equal to
`std::thread` but for realtime threads. That is, threads with a priority above 0
and a realtime policy such as `SCHED_FIFO` or `SCHED_RR`.

Both classes, `std::thread` and `preempt::realtime_thread` spawn a system thread in the
constructor and run in close collaboration with other threads within a process
(`join()`).

## Unit tests

Practically all tests shipped in this repository have to do with realtime and
concurrency. Every single test has been carefully written in a way that (1) it
runs one specific scenario and (2) is human readable.

On this basis one can pick a test and modify it or derive new tests from
existing ones easily. Another motivation was to have an easy environment to
isolate and reproduce [possible] bugs.

*tests/std_xxx.cpp*
: C++ standard library. Currently tests exist for *std::thread* and other
  classes dealing with concurrency.

*tests/pthread_xxx.cpp*
: POSIX threads and [realtime] scheduling policies.

*tests/task_xxx.cpp*
: Tests for classes contain *std::thread* or *pthread* members with the
  intention of starting these as needed or based on events.

*tests/sched_xxx.cpp*
: Scheduling strategies for tasks. These culminate in easy-to-use classes for
  the actual application.

*tests/process_xxx.cpp*
: Processes with [realtime] task schedulers. Also memory management and IPC.

As one can see categories are based on each another:

```
       std ==> task ===|
                       |===> scheduling ==> process
   pthread ==> task ===|
```

We use *pthread* because the C++ standard [yet] implements no realtime threads.
If it will one day, the `realtime_thread` class in *Libpreempt* will probably
become superfluous. The changes required in the application should be
manageable, especially since we can test them beforehand using `pudding.sh`.

## Pudding

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

## Why can realtime tests fail?

If the code is running under a RTOS and Linux RT-throttling is disabled, the
only other thing known to mess with the realtime scheduling policies is the
memory manager. To disable pagefaults all pages must be locked before letting
`pthread_create()` create FIFO or RR threads:

``` c++
#include <preempt/process.h>

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();
  // can safely start realtime threads
  preempt::this_process::end_realtime();
  return 0
}
```

Basically `begin_realtime()` locks all process memory so that the VMM can't
interrupt the process.

## Is a Linux realtime kernel required to use realtime scheduling?

No. Realtime scheduling is POSIX function available on all Linux kernels. A
kernel built with the realtime patches is needed if:

- You want to run software with very low latency settings that require realtime
  performance that can only be achieved with an RT kernel.

- Your hardware configuration triggers poor latency behaviour which might be
  improved with an RT kernel.

## Does Yocto-Linux has a realtime-kernel?

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

## How does Preemptive Multitasking work for realtime systems?

Preemptive multitasking creates the illusion that multiple processes and threads
run concurrently on a single processor, while these are actually only assigned a
small time slice. If the time slice is exceeded, the process or thread is
"preempted", i.e. interrupted and has to wait for its next time slice.

Realtime threads are an exception here. They are only interrupted if a thread
with a higher priority (FIFO) or the same priority (RR) is to run, or if
`sched_yield()` is called explicitly.

This means that a realtime thread can take over and paralyze the entire system
if it does not return quickly enough or in the case of longer jobs voluntarily
calls `sched_yield()`.

For this reason, RT throttling is activated under Linux: by default, 5% of the
CPU time is also assigned to non-RT threads every second.

## How does Cooperative Multitasking work for realtime systems?

> Cooperative multitasking, also known as non-preemptive multitasking, is a
> style of computer multitasking in which the operating system never initiates a
> context switch from a running process to another process. Instead, processes
> voluntarily yield control periodically or when idle or logically blocked in
> order to enable multiple applications to be run concurrently.
>
> *Cooperative multitasking*
> https://en.wikipedia.org/wiki/Cooperative_multitasking

The principle is also used to schedule jobs attached to threads in realtime
systems. Each job is expected to complete within a give timeslice, for example,
200ms. If the thread can guarantee this it does not matter much if the scheduler
is implemented preemptively or cooperatively.

# APPENDIX

## Some details about POSIX realtime scheduling (SCHED_FIFO, SCHED_RR)

FIFO and RR threads belong to the cathegory of the real-time (RT) processes.
Real-time processes are in charge of critical tasks whose execution cannot be
interrupted. These tasks are usually involved in multimedia processing.

- Because realtime threads prevent any non-realtime threads (including all
  `std::thread`) from running, in general **their life-time should be [very]
  short**.

- The thread **may or may not have started when `pthread_create()` returns** but
  `SCHED_FIFO` threads are always executed in the order of their creation.

- On **multicore systems** the kernel will not run multiple threads on multiple
  cores simultaneously, which would violate the defined behavior. A thread
  scheduled FIFO or RR, when selected for running, will continue to use the CPU
  until either it is blocked by an I/O request, it is preempted by a higher
  priority FIFO or RR, or it calls `sched_yield()`.

https://man7.org/linux/man-pages/man7/sched.7.html

## Execution privileges required to start realtime threads

Unless the user has **scheduling privileges** executing tests that use one of
the realtime scheduling policies will fail (permission denied). Use the `-S`
option to prefix each run of a compiled executable with `sudo`. Alternatively:

``` sh
 $ alias pudding='sudo ./pudding.sh'
```

## Etymology

The name of the script `pudding.sh` was inspired by the expression "the proof is
in the pudding." Generally, it is used to say the best test of a pudding is to
eat it. The real worth or effectiveness of something can only be determined by
putting it to the test by trying or using it, over and over again, standards,
versions and promises aside.

Since it operates inside a computer `pudding.sh` expects that no process will
fail *ever*. Either they all succeed or there are bugs.

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
