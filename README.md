---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# Libpreempt

## Synopsis

[*Libpreempt*](https://github.com/andreasspindler/Libpreempt) -- C++ real-time test suite and programming framework

Components:
- the C++ framework around the notion of threads, processes, tasks and
  schedulers
- the corresponding test suite

## Examples

### Run the tests provided

The C ++ framework was not developed on a theoretical basis but on the basis of
concrete, practical problems. These can be found in the form of individual files
in the *tests/* subdirectory.

All tests can be started very easily by simply cloning the repository and
calling `make`.

Example:

``` sh
 > git clone https://github.com/andreasspindler/Libpreempt
 > cd Libpreempt
 > sudo make
[sudo] password for jiva: 
./pudding.sh -D build 100
### multivm Libpreempt(all) *** WARNING: running under VM 
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 'Linux' (some tests will fail!) 
make[1]: Entering directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_02_fifo.c++17.debug ../../tests/preempt_02_fifo.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_03_fifo.c++17.debug ../../tests/preempt_03_fifo.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_02_fifo.c++14.debug ../../tests/preempt_02_fifo.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o task_01_monopoly.c++14.debug ../../tests/task_01_monopoly.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_01_thread.c++14.debug ../../tests/std_01_thread.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_02_nonstatic.c++17.debug ../../tests/std_02_nonstatic.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o task_01_monopoly.c++17.debug ../../tests/task_01_monopoly.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_03_array.c++14.debug ../../tests/std_03_array.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_04_lambda.c++14.debug ../../tests/std_04_lambda.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_04_thread.c++17.debug ../../tests/preempt_04_thread.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_01_thread.c++17.debug ../../tests/std_01_thread.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_01_other.c++14.debug ../../tests/preempt_01_other.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_01_other.c++17.debug ../../tests/preempt_01_other.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_03_array.c++17.debug ../../tests/std_03_array.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_03_fifo.c++14.debug ../../tests/preempt_03_fifo.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++17 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_04_lambda.c++17.debug ../../tests/std_04_lambda.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o preempt_04_thread.c++14.debug ../../tests/preempt_04_thread.cpp ../../src/base.cpp ../../src/process.cpp
g++ -std=c++14 -pthread -D_GNU_SOURCE -fmax-errors=5 -Og -ggdb -pedantic -I../../include -o std_02_nonstatic.c++14.debug ../../tests/std_02_nonstatic.cpp ../../src/base.cpp ../../src/process.cpp
make[1]: Leaving directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
### multivm Libpreempt(all) *** executing command '100' [Thu, 02 Dec 2021 12:29:52 +0100]
### multivm Libpreempt(all) *** c++14 c++17 debug
### multivm Libpreempt(all) *** 9 tests => 18 target(s) => 1800 total run(s)
#   1/  18: preempt_01_other.c++14.debug                    100 run(s)          0 bad        100 good
#   2/  18: preempt_01_other.c++17.debug                    100 run(s)          0 bad        100 good
#   3/  18: preempt_02_fifo.c++14.debug                     100 run(s)          0 bad        100 good
#   4/  18: preempt_02_fifo.c++17.debug                     100 run(s)          1 bad         99 good
#   5/  18: preempt_03_fifo.c++14.debug                     100 run(s)         10 bad         90 good
#   6/  18: preempt_03_fifo.c++17.debug                     100 run(s)         12 bad         88 good
#   7/  18: preempt_04_thread.c++14.debug                   100 run(s)          0 bad        100 good
#   8/  18: preempt_04_thread.c++17.debug                   100 run(s)          0 bad        100 good
#   9/  18: std_01_thread.c++14.debug                       100 run(s)          0 bad        100 good
#  10/  18: std_01_thread.c++17.debug                       100 run(s)          0 bad        100 good
#  11/  18: std_02_nonstatic.c++14.debug                    100 run(s)          0 bad        100 good
#  12/  18: std_02_nonstatic.c++17.debug                    100 run(s)          0 bad        100 good
#  13/  18: std_03_array.c++14.debug                        100 run(s)          0 bad        100 good
#  14/  18: std_03_array.c++17.debug                        100 run(s)          0 bad        100 good
#  15/  18: std_04_lambda.c++14.debug                       100 run(s)          0 bad        100 good
#  16/  18: std_04_lambda.c++17.debug                       100 run(s)          0 bad        100 good
#  17/  18: task_01_monopoly.c++14.debug                    100 run(s)          0 bad        100 good
#  18/  18: task_01_monopoly.c++17.debug                    100 run(s)          0 bad        100 good
### multivm Libpreempt(all) *** 98.700% (1800 runs = 1777 good + 23 bad + 0 missing)
make: *** [Makefile:8: all] Error 1
```

Don't be impressed. Make simply calls the test driver `pudding.sh` here. First
the script compiles a matrix of executables for each single test. Each test is
stretched over different C++ standards, optimization levels and (optionally) CPU
micro-architecures. In this way it compiles many executable files for each
single C++ file. Second, the script runs all executables repeatedly and
summarizes results only based on exit codes.

A run is successful if the process returns code 0 (`EXIT_SUCCESS`). So no test
framework is required on the C++ side. A test is successful when all `assert()`
calls return and neither `std::abort()` nor `std::terminate()` was called.

We have five test categories based on each other:

```
       std ==> task ===|
                       |===> process
   preempt ==> task ===|
```

std
: Tests using the C++ standard library.

preempt
: Test for this C++ framework that extends the standard library with POSIX
  functions.

task
: Define classes that use threads.

process
: Sample applications for tasks.

Make is prefixed with *sudo* because unless the user has **scheduling
privileges** executing programs that use one of the real-time scheduling
policies will fail (permission denied).

## Installing

``` sh
 > git clone https://github.com/andreasspindler/Libpreempt
```

## Details

### Real-time framework

*Libpreempt* is a C++ library build around the concepts of **threads**,
**tasks** and **schedulers**. It is based on standard C++ and POSIX and defined
in namespace `preempt`

**Thread**
: Independent units of execution within a process. Defines class
  `preempt::thread` with the same interface as `std::thread` but with optional
  real-time scheduling.

**Task**
: Any class with at least one thread attribute. Tasks can run standard threads
  and real-time threads. The framework defines the basic classes
  `preempt::mono_task` and `preempt::poly_task` in file
  *include/preempt/task.h*.

**Scheduler**
: An object that schedules tasks using a real-time clock-thread that runs with
  priority 99. Runs tasks based on ticks which are defined in microseconds.

### Real-time scheduler test suite

Every test has the form of a C++ file and has been carefully written in a way
that (1) it runs one specific scenario and (2) is human readable. Each test will
verify several attributes of the **Linux real-time scheduler** with regard to
the POSIX scheduling policies.

``` sh
 > sudo make
```

The default Make target compiles all programs in the *tests/* folder for C++14
and C++17 and executes each compilation 100 times, counting the failed and
successful attempts.

If the *PREEMPT_RT* patches are installed all tests should complete 100%,
otherwise the rate of successful tests should be over 95% under Linux and still
greater than 0% under layers like Cygwin.

### Etymology

The name "preempt" is short for preemptive. The name is a reminder that
real-time threads are only preempted by threads with higher priorities (FIFO) or
the same priority (RR), or they deliberately call `sched_yield()`

The name of the unit test drive "pudding" was inspired by the expression "the
proof is in the pudding." The phrase means the best test of a pudding is to eat
it. The real worth or effectiveness of something can only be determined by
putting it to the test by trying or using it, over and over again, standards,
versions and promises aside.

## FAQ

### Why can real-time tests fail?

Only four reasons are known to the author.

**PREEMPT_RT** patches are not installed.
: The code compiles because POSIX does not require an RTOS; in other words, a
  kernel can implement `SCHED_FIFO` and `SCHED_RR` with best effort. But some
  tests are so tight that they must fail if the kernel is not patched.

The user has no **scheduling privileges**
: Use `ulimit -r` to see the maximum real-time priority the current user can
  use. If this value is 0 then the current user is not allowed to use real-time
  scheduling policies. In this case, the affected test fails 100%. See also
  */etc/security/limits.conf*.

Linux **RT throttling** enabled
: If tests only fail sometimes (not 100%) then Linux RT throttling is likely
  enabled.

**Page-faults** occur
: The only other thing known to mess with the real-time scheduling policies is
  the memory manager. To disable page-faults all pages must be locked before
  calling creating a real-time thread.

``` c++
#include <preempt/process.h>

int main(int argc, char *argv[])
{
  preempt::this_process::begin_real_time();
  /*
   * all memory locked - the VMM won't interrupt the process
   */
  preempt::this_process::end_real_time();
  return 0
}
```

### What does "real-time" mean for a program?

**The program should behave as predictably as possible**. The specific
description of *real-time* is that processes/threads have minimum response time
guarantees. This means that it should always take up the same period of time,
regardless of the data to be processed, and this period must be measurable.

### What is an RTOS?

**The basis of an RTOS is a real-time scheduler for threads and processes**. A
RT scheduler operates in O(1) time or, under Linux, as CFS (*Completely Fair
Scheduler*). While an RTOS requires an RT scheduler it also needs predictable
"real-time behaviors" in the other parts of the OS too.

Real-time processes and threads are in charge of critical tasks whose execution
cannot be interrupted (usually involved in multimedia processing). Because
real-time processes/threads prevent any non-real-time processes/threads from
running, in general their life-time should be short.

### How predictable is an RTOS?

It is usually sufficient if the system is **predictable for a specific,
practical problem** and not for all conceivable questions.

In order to achieve predictability real-time threads must be distributed to the
underlying CPU cores in such a way that no thread is disadvantaged. A thread is
not disadvantaged if its preset priority, affinity and scheduling policy is
always adhered to by the OS. Only real-time threads have a priority and a
scheduling policy.

### Is an RTOS required to use real-time scheduling?

No. Real-time scheduling is a POSIX function available on all Linux kernels or
compatibility layers such as Cygwin. A kernel built with the real-time patches
is needed if:

- You want to run software with very low latency settings that require
  performance that can only be achieved with an RT kernel.

- Your hardware configuration triggers poor latency behaviour which might be
  improved with an RT kernel.
  
### How to check if PREEMPT_RT patches are installed?

``` sh
 $ uname -v
 #60~20.04.1-Ubuntu SMP PREEMPT Thu May 6 09:52:46 UTC 2021
                         ^
                         |
                         |
                         indicates RTOS
```

## Does Yocto-Linux has an RT kernel?

Yes.

## How does preemptive multitasking work for real-time systems?

Preemptive multitasking creates the illusion that multiple processes and threads
run concurrently on a single processor, while they are actually only assigned a
time slice. If the time slice is exceeded, the process or thread is "preempted",
i.e. interrupted and has to wait for its next time slice.

Real-time threads are an exception here. They are only interrupted if a thread
with a higher priority (FIFO) or the same priority (RR) is to run, or if
`sched_yield()` is called explicitly. This means that such a thread can take
over and paralyze the entire system if it does not return quickly enough or - in
the case of longer jobs - voluntarily calls `sched_yield()`.

For this reason, RT throttling is activated under Linux: by default 5% of the
CPU time is also assigned to non-RT threads per second.

## How does cooperative multitasking work for real-time systems?

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

### How does SCHED_FIFO and SCHED_RR work on multi-core systems?

Processes and threads scheduling with `SCHED_FIFO` or `SCHED_RR` policy belong
to the real-time (RT) category.

On **multicore systems** the kernel will not run multiple threads on multiple
cores simultaneously, which would violate the defined behavior. A thread
scheduled `SCHED_FIFO` or `SCHED_RR`, when selected for running, will continue
to use the CPU until either it is blocked by an I/O request, it is preempted by
a higher priority `SCHED_FIFO` or `SCHED_RR`, or it calls `sched_yield()`.

### How do time-slices affect SCHED_FIFO and SCHED_RR?

They don't. `SCHED_FIFO` and `SCHED_RR` are simple scheduling algorithms without
time slicing.

While a thread may or may not have started when `pthread_create()` returns
`SCHED_FIFO` threads are always executed in the order of their creation.

### Which priorities are valid for SCHED_FIFO and SCHED_RR?

In POSIX 1-32, under Linux 1-99. The kernel can't change the priority.

A source of error is to pass 0 as the priority. `SCHED_FIFO` can be used only
with static priorities higher than 0, which means that when a `SCHED_FIFO`
becomes runnable, it will always immediately preempt any currently running
`SCHED_OTHER`, `SCHED_BATCH`, or `SCHED_IDLE` threads.

https://man7.org/linux/man-pages/man7/sched.7.html

## Copyright

Written by Andreas Spindler <info@andreasspindler.de>

For copyright and license information see file *UNLICENSE*.

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
