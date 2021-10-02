---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# NAME

*Libpreempt* -- C++ test framework for embedded systems and realtime programming

# DESCRIPTION

The project consists of two components:
- the C++ framework *Libpreempt*
- the unit test tool *Pudding*

Quick start:

``` sh
 $ make
```

## Libpreempt

*Libpreempt* is a C++ library build around the class
`realtime_thread<SCHEDULING_POLICY>` which is basically equal to `std::thread`
but for realtime threads. That is, threads with a priority above 0 and a
realtime policy such as `SCHED_FIFO` and `SCHED_RR`.

Both classes, `std::thread` and `realtime_thread` spawn a system thread in the
constructor and run in close collaboration with other threads within a process
(aka `join()`).

The next element of the framework are *tasks*. By definition, a task is a class
that contains threads as members.

Based on tasks *scheduling strategies* are defined (also in form of classes)
that implement a way of distributing tasks over time.

## Pudding

Pudding is implemented in a single script. It compiles a matrix of executables
for each single test, runs them and summarizes results.

More specifically, this is how the `pudding.sh` script works. It assumes that
each *.cpp*-file in *tests/* is a separate test. It then spans these tests over
several language standards, optimization levels and CPU microarchitectures and
in this way generates many executable files.

The script can execute these files repeatedly while counting successful runs.
Essentially this means all `assert()`-ions hold true, neither `std::abort()` nor
`std::terminate()` was called and the process returns 0 (`EXIT_SUCCESS`).

### In media res

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

As soon as something changes with the hardware, OS, compiler, libraries etc. the
hard-won tests can be rebuild and trust can be regained again.

Pudding works nicely on top of **Docker images** and **Yocto Linux Bitbake
recipes**, with Docker or Yocto providing the system basis and `pudding.sh` the
basis for the programming techniques used in the actual application.

### Execution privileges required to start realtime threads

Unless the user has scheduling privileges executing tests that use one of the
realtime scheduling policies will fail (permission denied). Use the `-S` option
to prefix each run of a compiled executable with `sudo`. Alternatively:

``` sh
 $ alias pudding='sudo ./pudding.sh'
```

### Etymology

The name of the script was inspired by the expression "the proof is in the
pudding." Generally, it is used to say the best test of a pudding is to eat it.
The real worth or effectiveness of something can only be determined by putting
it to the test by trying or using it, over and over again, standards, versions,
promises and hopes aside.

Since it operates inside a computer `pudding.sh` expects that no process will
fail *ever*. Either they all succeed or something smells.

# CATEGORIES

Most test shipped in this repository has to do with realtime and concurrency. We
have five categories: *std*, *pthread*, *task*, *scheduling* and *process*.

Every single test has been carefully written in a way that (1) it runs one
specific scenario and (2) is human readable. The vision was that the source code
could be projected on the wall in front of people. But not just to teach them -
to better find mistakes in thinking. On this basis one can pick a test and
modify it or derive new tests from existing ones easily. Another motivation was
to have an easy environment to isolate and reproduce [possible] bugs.

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

A special use case is running all tests very often, like when going to lunch or
over-night:

``` sh
 $ pudding afk
```

What this command does is a bit funny. It runs tests on a sequence of Fibonacci
numbers (0 1 1 2 3 5 8 12 21 ... 46368). The same is done by the command
`stretch` but only up to 21.  Example:

``` sh
 $ pudding -S stretch
### multivm Libpreempt(all) *** WARNING: running under VM
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed
### multivm Libpreempt(all) *** 100.000% (0 runs = 0 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 100.000% (160 runs = 160 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 99.300% (160 runs = 159 good + 1 bad + 0 missing)
### multivm Libpreempt(all) *** 100.000% (320 runs = 320 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 100.000% (480 runs = 480 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 100.000% (800 runs = 800 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 100.000% (1280 runs = 1280 good + 0 bad + 0 missing)
### multivm Libpreempt(all) *** 99.700% (1920 runs = 1916 good + 4 bad + 0 missing)
### multivm Libpreempt(all) *** 99.900% (3360 runs = 3357 good + 3 bad + 0 missing)
```

# USE CASES

## Add a new test

Simply create a new C++ file in the *tests* directory, for example,
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
 $ pudding -f std build 10
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

# AUTHOR

Andreas Spindler <info@andreasspindler.de>

# COPYRIGHT

For copyright and license information see file UNLICENSE.

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
