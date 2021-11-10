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
: Independent units of execution within a process. Threads are to time what
  containers are to data. The library defines `preempt::thread` with the same
  interface as `std::thread` but with an optional priority and a scheduling
  policy.

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
### multivm Libpreempt(all) *** writing 'out/multivm-all/Makefile'
### multivm Libpreempt(all) *** WARNING: running under VM
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 'Linux'
### multivm Libpreempt(all) *** WARNING: user has no rights to start realtime threads
make[1]: Entering directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
make[1]: Nothing to be done for 'build'.
make[1]: Leaving directory '/media/sf_H_DRIVE/shastras/Libpreempt/out/multivm-all'
### multivm Libpreempt(all) *** executing command '100' [Wed, 10 Nov 2021 19:22:44 +0100]
### multivm Libpreempt(all) *** c++14 c++17 debug
### multivm Libpreempt(all) *** 9 tests => 18 target(s) => 1800 total run(s)
#   1/  18: sched_01_other.c++14.debug                      100 run(s)          0 bad        100 good
#   2/  18: sched_01_other.c++17.debug                      100 run(s)          0 bad        100 good
#   3/  18: sched_02_fifo.c++14.debug                       100 run(s)          1 bad         99 good
#   4/  18: sched_02_fifo.c++17.debug                       100 run(s)          2 bad         98 good
#   5/  18: sched_03_fifo.c++14.debug                       100 run(s)         25 bad         75 good
#   6/  18: sched_03_fifo.c++17.debug                       100 run(s)         21 bad         79 good
#   7/  18: std_01_thread.c++14.debug                       100 run(s)          0 bad        100 good
#   8/  18: std_01_thread.c++17.debug                       100 run(s)          0 bad        100 good
#   9/  18: std_02_nonstatic.c++14.debug                    100 run(s)          0 bad        100 good
#  10/  18: std_02_nonstatic.c++17.debug                    100 run(s)          0 bad        100 good
#  11/  18: std_03_array.c++14.debug                        100 run(s)          0 bad        100 good
#  12/  18: std_03_array.c++17.debug                        100 run(s)          0 bad        100 good
#  13/  18: std_04_lambda.c++14.debug                       100 run(s)          0 bad        100 good
#  14/  18: std_04_lambda.c++17.debug                       100 run(s)          0 bad        100 good
#  15/  18: std_05_sched_policy.c++14.debug                 100 run(s)          0 bad        100 good
#  16/  18: std_05_sched_policy.c++17.debug                 100 run(s)          0 bad        100 good
#  17/  18: task_01.c++14.debug                             100 run(s)          0 bad        100 good
#  18/  18: task_01.c++17.debug                             100 run(s)          0 bad        100 good
### multivm Libpreempt(all) *** 97.200% (1800 runs = 1751 good + 49 bad + 0 missing)
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

Pudding is the driver for the unit tests and implemented in a single script. It
compiles a matrix of executables for each single test, runs them and summarizes
results. More specifically, this is how the `pudding.sh` script works:

- each *.cpp*-file in *tests/* is a separate test

- span these tests over several language standards, optimization levels and CPU
  micro-architectures and in this way generates many executable files

- execute each repeatedly while counting successful runs.

A run is successful if the process returns 0 (`EXIT_SUCCESS`). This means all
`assert()`s hold true, neither `std::abort()` nor `std::terminate()` was called.

Etymology:

The name was inspired by the expression "the proof is in the pudding."
Generally, it is used to say the best test of a pudding is to eat it. The real
worth or effectiveness of something can only be determined by putting it to the
test by trying or using it, over and over again, standards, versions and
promises aside.

## Why can real-time tests fail?

Case 1: **PREEMPT_RT** patches are not installed. The code compiles because
POSIX does not require an RTOS; in other words, a kernel can implement
`SCHED_FIFO` and `SCHED_RR` with best effort.

Case 2: The user has no **scheduling privileges**. Use `ulimit -r` to see the
maximum realtime priority the current user can use. If this value is 0 then the
current user is not allowed to use realtime scheduling policies. In this case,
the affected test fails 100%. See also */etc/security/limits.conf*.

Case 3: If tests only fail sometimes (not 100%) then Linux RT throttling is
likely enabled.

Case 4: The only other thing known to mess with the real-time scheduling
policies is the memory manager. To disable page-faults all pages must be locked
before calling `pthread_create()`:

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

# EXAMPLES

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
