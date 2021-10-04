---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# NAME

*Libpreempt* -- C++ test framework for embedded systems and realtime programming

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

## Pudding

Pudding is implemented in a single script. It compiles a matrix of executables
for each single test, runs them and summarizes results.

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

## Tests

Most test shipped in this repository have to do with realtime and concurrency.
Every single test has been carefully written in a way that (1) it runs one
specific scenario and (2) is human readable.

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

# BACKGROUND

## RTOS

The specific description of real-time is that processes have minimum response
time guarantees. **The system should behave as predictably as possible.** This
means that it should always take up the same period of time, regardless of the
data to be processed, and this period must be measurable.

**In order to achieve predictability**, the tasks that a computer executes in
parallel, must be distributed to the underlying CPU cores in such a way that no
task is disadvantaged. A task is not disadvantaged if its preset priority,
affinity and scheduling policy is always adhered to by the OS. Only real-time
tasks have a priority and a scheduling policy.

**The basis is a real-time scheduler for threads and processes**. A RT scheduler
operates in O(1) time or, under Linux, as CFS (*Completely Fair Scheduler*).
While an RTOS requires an RT scheduler it also needs predictable behaviors in
the other parts of the OS too.

It is usually sufficient if the system is **predictable for a specific,
practical problem** (application) and not for all conceivable questions.

## Execution privileges required to start realtime threads

Unless the user has scheduling privileges executing tests that use one of the
realtime scheduling policies will fail (permission denied). Use the `-S` option
to prefix each run of a compiled executable with `sudo`. Alternatively:

``` sh
 $ alias pudding='sudo ./pudding.sh'
```

## Etymology

The name of the script was inspired by the expression "the proof is in the
pudding." Generally, it is used to say the best test of a pudding is to eat it.
The real worth or effectiveness of something can only be determined by putting
it to the test by trying or using it, over and over again, standards, versions
and promises aside.

Since it operates inside a computer `pudding.sh` expects that no process will
fail *ever*. Either they all succeed or there are bugs.

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

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
