---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# NAME

*Libpreempt* -- C++ realt-time test suite and framework

# DESCRIPTION

The project consists of two main components:

- the C++ framework based on C++ and POSIX

- the corresponding real-time scheduler test suite

## Real-time framework

*Libpreempt* contains a C++ library build around the concepts of **threads**,
**tasks** and **processes**.

Thread
: Independent units of execution within a process. Defines class
  `preempt::thread` with the same interface as `std::thread` but with optional
  real-time scheduling.

Task
: Any class with at least one thread attribute. Tasks can run standard threads
  and real-time threads.

Etymology:

The name "preempt" is short for preemptive. The name is a reminder that
real-time threads are only preempted by threads with higher priorities (FIFO) or
the same priority (RR), or they deliberately call `sched_yield()`

## Real-time scheduler test suite

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

If the **PREEMPT_RT** patches are installed all tests should complete 100%,
otherwise the rate of successful tests should be over 95% (POSIX does not
require a real-time scheduler).

Make is prefixed with *sudo* because unless the user has **scheduling
privileges** executing programs that use one of the real-time scheduling
policies will fail (permission denied).

For example, here is the output on an Ubuntu VM without preempt patches:

Example:

``` sh
 > sudo make
./pudding.sh -D build 100
### multivm Libpreempt(all) *** WARNING: running under VM 
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 'Linux' (some tests will fail!) 
make[1]: Entering directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
make[1]: Nothing to be done for 'build'.
make[1]: Leaving directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
### multivm Libpreempt(all) *** executing command '100' [Thu, 11 Nov 2021 17:50:05 +0100]
### multivm Libpreempt(all) *** c++14 c++17 debug
### multivm Libpreempt(all) *** 9 tests => 18 target(s) => 1800 total run(s)
#   1/  18: preempt_01_other.c++14.debug                    100 run(s)          0 bad        100 good
#   2/  18: preempt_01_other.c++17.debug                    100 run(s)          0 bad        100 good
#   3/  18: preempt_02_fifo.c++14.debug                     100 run(s)          1 bad         99 good
#   4/  18: preempt_02_fifo.c++17.debug                     100 run(s)          0 bad        100 good
#   5/  18: preempt_03_fifo.c++14.debug                     100 run(s)         21 bad         79 good
#   6/  18: preempt_03_fifo.c++17.debug                     100 run(s)         30 bad         70 good
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
#  17/  18: task_01.c++14.debug                             100 run(s)          0 bad        100 good
#  18/  18: task_01.c++17.debug                             100 run(s)          0 bad        100 good
### multivm Libpreempt(all) *** 97.100% (1800 runs = 1748 good + 52 bad + 0 missing)
make: *** [Makefile:8: all] Error 1
```

We have five test categories based on each other:

```
       std ==> task ===|
                       |===> process
   preempt ==> task ===|
```

std
: C++ standard library.

preempt
: Test for this C++ framework that extends the standard library with POSIX
  functions.

task
: Define classes that use threads.

process
: Sample applications for tasks.

## Unit test driver (Pudding)

*Pudding* is the driver for the unit tests and implemented in a single script.
It compiles a matrix of executables for each single test, runs them and
summarizes results,

- each *.cpp*-file in *tests/* is a separate test

- span these tests over several language standards, optimization levels and CPU
  micro-architectures and in this way generates many executable files

- execute each repeatedly while counting successful runs.

A run is successful if the process returns code 0 (`EXIT_SUCCESS`). This means
all assertions and neither `std::abort()` nor `std::terminate()` was called.

Etymology:

The name was inspired by the expression "the proof is in the pudding." The
phrase means the best test of a pudding is to eat it. The real worth or
effectiveness of something can only be determined by putting it to the test by
trying or using it, over and over again, standards, versions and promises aside.

## Why can real-time tests fail?

**PREEMPT_RT** patches are not installed.
: The code compiles because POSIX does not require an RTOS; in other words, a
  kernel can implement `SCHED_FIFO` and `SCHED_RR` with best effort.

The user has no **scheduling privileges**
: Use `ulimit -r` to see the maximum real-time priority the current user can
  use. If this value is 0 then the current user is not allowed to use real-time
  scheduling policies. In this case, the affected test fails 100%. See also
  */etc/security/limits.conf*.

Linux **RT throttling** enabled
: If tests only fail sometimes (not 100%) then Linux RT throttling is likely
  enabled.

**Page-faults**
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
