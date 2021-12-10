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

The C++ framework was not developed on a theoretical basis but on the basis of
concrete, practical "real-time" and "embedded" questions and scenarios. These
can be found in the form of individual files in the *tests/* sub-directory.

To run these tests clone the repository and run `make`.

Example:

``` sh
 > git clone https://github.com/andreasspindler/Libpreempt
 > cd Libpreempt
 > sudo make
./pudding.sh -DR build 10
### multivm Libpreempt(all) *** c++14 c++17 debug release
### multivm Libpreempt(all) *** 12 tests => 48 targets
### multivm Libpreempt(all) *** WARNING: running under VM 
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 
### multivm Libpreempt(all) *** make -S -j4 -f multivm-all.mak build
make[1]: Entering directory '/media/sf_H_DRIVE/shastras/Libpreempt'
   .
   .
make[1]: Leaving directory '/media/sf_H_DRIVE/shastras/Libpreempt'
### multivm Libpreempt(all) *** 12 tests => 48 targets => 480 runs
#   1/  48: base_01_thread.c++14.debug                       10 run(s)          0 bad         10 good
#   2/  48: base_01_thread.c++14.release                     10 run(s)          0 bad         10 good
#   3/  48: base_01_thread.c++17.debug                       10 run(s)          0 bad         10 good
#   4/  48: base_01_thread.c++17.release                     10 run(s)          0 bad         10 good
#   5/  48: preempt_01_self.c++14.debug                      10 run(s)          0 bad         10 good
#   6/  48: preempt_01_self.c++14.release                    10 run(s)          0 bad         10 good
#   7/  48: preempt_01_self.c++17.debug                      10 run(s)          0 bad         10 good
#   8/  48: preempt_01_self.c++17.release                    10 run(s)          0 bad         10 good
#   9/  48: preempt_02_fifo.c++14.debug                      10 run(s)          0 bad         10 good
#  10/  48: preempt_02_fifo.c++14.release                    10 run(s)          0 bad         10 good
#  11/  48: preempt_02_fifo.c++17.debug                      10 run(s)          0 bad         10 good
#  12/  48: preempt_02_fifo.c++17.release                    10 run(s)          0 bad         10 good
#  13/  48: preempt_03_decrement.c++14.debug                 10 run(s)          0 bad         10 good
#  14/  48: preempt_03_decrement.c++14.release               10 run(s)          0 bad         10 good
#  15/  48: preempt_03_decrement.c++17.debug                 10 run(s)          0 bad         10 good
#  16/  48: preempt_03_decrement.c++17.release               10 run(s)          0 bad         10 good
#  17/  48: preempt_04_thread.c++14.debug                    10 run(s)          0 bad         10 good
#  18/  48: preempt_04_thread.c++14.release                  10 run(s)          0 bad         10 good
#  19/  48: preempt_04_thread.c++17.debug                    10 run(s)          0 bad         10 good
#  20/  48: preempt_04_thread.c++17.release                  10 run(s)          0 bad         10 good
#  21/  48: std_01_thread.c++14.debug                        10 run(s)          0 bad         10 good
#  22/  48: std_01_thread.c++14.release                      10 run(s)          0 bad         10 good
#  23/  48: std_01_thread.c++17.debug                        10 run(s)          0 bad         10 good
#  24/  48: std_01_thread.c++17.release                      10 run(s)          0 bad         10 good
#  25/  48: std_02_nonstatic.c++14.debug                     10 run(s)          0 bad         10 good
#  26/  48: std_02_nonstatic.c++14.release                   10 run(s)          0 bad         10 good
#  27/  48: std_02_nonstatic.c++17.debug                     10 run(s)          0 bad         10 good
#  28/  48: std_02_nonstatic.c++17.release                   10 run(s)          0 bad         10 good
#  29/  48: std_03_array.c++14.debug                         10 run(s)          0 bad         10 good
#  30/  48: std_03_array.c++14.release                       10 run(s)          0 bad         10 good
#  31/  48: std_03_array.c++17.debug                         10 run(s)          0 bad         10 good
#  32/  48: std_03_array.c++17.release                       10 run(s)          0 bad         10 good
#  33/  48: std_04_lambda.c++14.debug                        10 run(s)          0 bad         10 good
#  34/  48: std_04_lambda.c++14.release                      10 run(s)          0 bad         10 good
#  35/  48: std_04_lambda.c++17.debug                        10 run(s)          0 bad         10 good
#  36/  48: std_04_lambda.c++17.release                      10 run(s)          0 bad         10 good
#  37/  48: task_01_monopoly.c++14.debug                     10 run(s)          0 bad         10 good
#  38/  48: task_01_monopoly.c++14.release                   10 run(s)          0 bad         10 good
#  39/  48: task_01_monopoly.c++17.debug                     10 run(s)          0 bad         10 good
#  40/  48: task_01_monopoly.c++17.release                   10 run(s)          0 bad         10 good
#  41/  48: task_02_deadline.c++14.debug                     10 run(s)          0 bad         10 good
#  42/  48: task_02_deadline.c++14.release                   10 run(s)          0 bad         10 good
#  43/  48: task_02_deadline.c++17.debug                     10 run(s)          1 bad          9 good
#  44/  48: task_02_deadline.c++17.release                   10 run(s)          1 bad          9 good
#  45/  48: task_03_sort.c++14.debug                         10 run(s)          0 bad         10 good
#  46/  48: task_03_sort.c++14.release                       10 run(s)          0 bad         10 good
#  47/  48: task_03_sort.c++17.debug                         10 run(s)          0 bad         10 good
#  48/  48: task_03_sort.c++17.release                       10 run(s)          0 bad         10 good
### multivm Libpreempt(all) *** 99.500% (480 runs = 478 good + 2 bad + 0 missing)
### multivm Libpreempt(all) *** exit code 1
```

First the script compiles a matrix of executables for each single test where
each test is stretched over different C++ standards, optimization levels and
(optionally) CPU micro-architecures. In this way it compiles many executable
files for each single C++ file. Second, the script runs all tests repeatedly and
summarizes results based on exit codes.

Make is prefixed `sudo` because unless the user has **scheduling privileges**
executing programs that use one of the real-time scheduling policies will fail
(permission denied).

A run is considered good if the process succeeds. So no test framework is
required on the C++ side. If the process returns exit code 0 it passed,
otherwise not.

If the *PREEMPT_RT* patches are installed all tests should complete 100%,
otherwise the rate of successful tests should be will over 95% under Linux and
50% under layers like Cygwin.

NOTE: Since the tests are all standard C++ they should compile on non-UNIX
      systems such as Windows too. However, currently the test driver generates
      a Makefile for GNU/Linux only. The plan is to have this part, which is
      implemented a little "brute force" at the moment, done with Cmake, but the
      author has not had the time for that yet. Pull requests welcome.

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
: Independent units of execution within a process (functions, class methods).
  Defines class `preempt::thread` in file *include/preempt/thread.h* with the
  same interface as `std::thread` but with optional real-time scheduling.

**Task**
: Any class with at least one thread attribute. Tasks can run standard threads
  and real-time threads. Defines the base classes `preempt::mono_task` and
  `preempt::poly_task` in file *include/preempt/task.h*.

**Scheduler**
: An object that schedules tasks using a real-time clock-thread that runs with
  priority 99. Runs tasks based on ticks which are defined in microseconds.

### Real-time scheduler test suite

Every test has the form of a C++ file and has been carefully written in a way
that (1) it runs one specific scenario and (2) is human readable. Each test will
verify several attributes of the **Linux real-time scheduler** with regard to
the POSIX scheduling policies.

#### Test categories

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

The user has no **scheduling privileges**
: Use `ulimit -r` to see the maximum real-time priority the current user can
  use. If this value is 0 then the current user is not allowed to use real-time
  scheduling policies. In this case, the affected test fails 100%. See also
  */etc/security/limits.conf*.

Linux **PREEMPT_RT** patches are not installed
: The code compiles because POSIX does not require an RTOS; in other words, a
  kernel can implement `SCHED_FIFO` and `SCHED_RR` with best effort. But some
  tests are so tight that they must fail if the kernel is not patched.

Linux **RT throttling** enabled
: If tests only fail sometimes (not 100%) then Linux RT throttling is likely
  enabled.

**Page-faults** have occured
: The only other thing known to mess with the real-time scheduling policies is
  the memory manager. To prevent page-faults all pages must be locked before
  creating a real-time thread.

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

They don't. `SCHED_FIFO` and `SCHED_RR` are scheduling algorithms without time
slicing.

For example, while a thread may or may not have started when `pthread_create()`
returns `SCHED_FIFO` threads are always executed in the order of their creation.

### Which priorities are valid for SCHED_FIFO and SCHED_RR?

In POSIX 1-32, under Linux 1-99. The kernel can't change the priority.
`SCHED_FIFO` can be used only with static priorities higher than 0, which means
that when a `SCHED_FIFO` becomes runnable, it will always immediately preempt
any currently running `SCHED_OTHER`, `SCHED_BATCH`, or `SCHED_IDLE` threads.

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
