---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

Libpreempt
==========
C++ real-time test suite and programming framework

The target audience for Libpreempt are:

- C++ developers who want to use realtime threads with the STL
- Linux developers familar with POSIX threads and
- firmware developers with real-time requirements

Components:

- the C++ framework around the notion of threads, processes, tasks and schedulers
- the corresponding test suite which was written so

The C++ framework was not developed on a theoretical basis but on the basis of
concrete, practical "real-time" and "embedded" questions and scenarios. These
can be found in the form of individual files in the *tests/* sub-directory.

Each test is a single C++ file and was written to be educational.

The "preempt" in Libpreempt is a reminder that real-time threads are only
preempted by threads with higher priorities (FIFO) or the same priority (RR), or
they deliberately call `sched_yield()`.

https://github.com/andreasspindler/Libpreempt

Install/Run
-----------

Clone the repository, compile, run and modify the real-time tests. It is also
possible to add a new profile to *.buildrc* and cross-compile for other CPU
micro-architectures.

Example:

``` sh
 > git clone https://github.com/andreasspindler/Libpreempt
 > cd Libpreempt
 > sudo make
./testmatrix.sh -DR 100
### multivm Libpreempt(all) *** BUILD: 13 tests => 52 executables
+ make -j4 --no-print-directory -Cbuild -fmultivm-all.mak build
### multivm Libpreempt(all) *** RUN: 13 tests => 52 targets => 5200 runs
### multivm Libpreempt(all) *** WARNING: no PREEMPT_RT patches installed in kernel 
    1/   52:  base_01_main.c++14.debug                        100 runs          0 bad        100 good
    2/   52:  base_01_main.c++14.release                      100 runs          0 bad        100 good
    3/   52:  base_01_main.c++17.debug                        100 runs          0 bad        100 good
    4/   52:  base_01_main.c++17.release                      100 runs          0 bad        100 good
    5/   52:  base_02_thread.c++14.debug                      100 runs          0 bad        100 good
    6/   52:  base_02_thread.c++14.release                    100 runs          0 bad        100 good
    7/   52:  base_02_thread.c++17.debug                      100 runs          0 bad        100 good
    8/   52:  base_02_thread.c++17.release                    100 runs          0 bad        100 good
    9/   52:  preempt_01_main.c++14.debug                     100 runs          0 bad        100 good
   10/   52:  preempt_01_main.c++14.release                   100 runs          0 bad        100 good
   11/   52:  preempt_01_main.c++17.debug                     100 runs          0 bad        100 good
   12/   52:  preempt_01_main.c++17.release                   100 runs          0 bad        100 good
   13/   52:  preempt_02_fifo.c++14.debug                     100 runs          2 bad         98 good
   14/   52:  preempt_02_fifo.c++14.release                   100 runs          0 bad        100 good
   15/   52:  preempt_02_fifo.c++17.debug                     100 runs          1 bad         99 good
   16/   52:  preempt_02_fifo.c++17.release                   100 runs          0 bad        100 good
   17/   52:  preempt_03_decrement.c++14.debug                100 runs          9 bad         91 good
   18/   52:  preempt_03_decrement.c++14.release              100 runs         14 bad         86 good
   19/   52:  preempt_03_decrement.c++17.debug                100 runs         23 bad         77 good
   20/   52:  preempt_03_decrement.c++17.release              100 runs         19 bad         81 good
   21/   52:  preempt_04_thread.c++14.debug                   100 runs          0 bad        100 good
   22/   52:  preempt_04_thread.c++14.release                 100 runs          0 bad        100 good
   23/   52:  preempt_04_thread.c++17.debug                   100 runs          0 bad        100 good
   24/   52:  preempt_04_thread.c++17.release                 100 runs          0 bad        100 good
   25/   52:  std_01_thread.c++14.debug                       100 runs          0 bad        100 good
   26/   52:  std_01_thread.c++14.release                     100 runs          0 bad        100 good
   27/   52:  std_01_thread.c++17.debug                       100 runs          0 bad        100 good
   28/   52:  std_01_thread.c++17.release                     100 runs          0 bad        100 good
   29/   52:  std_02_nonstatic.c++14.debug                    100 runs          0 bad        100 good
   30/   52:  std_02_nonstatic.c++14.release                  100 runs          0 bad        100 good
   31/   52:  std_02_nonstatic.c++17.debug                    100 runs          0 bad        100 good
   32/   52:  std_02_nonstatic.c++17.release                  100 runs          0 bad        100 good
   33/   52:  std_03_array.c++14.debug                        100 runs          0 bad        100 good
   34/   52:  std_03_array.c++14.release                      100 runs          0 bad        100 good
   35/   52:  std_03_array.c++17.debug                        100 runs          0 bad        100 good
   36/   52:  std_03_array.c++17.release                      100 runs          0 bad        100 good
   37/   52:  std_04_lambda.c++14.debug                       100 runs          0 bad        100 good
   38/   52:  std_04_lambda.c++14.release                     100 runs          0 bad        100 good
   39/   52:  std_04_lambda.c++17.debug                       100 runs          0 bad        100 good
   40/   52:  std_04_lambda.c++17.release                     100 runs          0 bad        100 good
   41/   52:  task_01_monopoly.c++14.debug                    100 runs          0 bad        100 good
   42/   52:  task_01_monopoly.c++14.release                  100 runs          0 bad        100 good
   43/   52:  task_01_monopoly.c++17.debug                    100 runs          0 bad        100 good
   44/   52:  task_01_monopoly.c++17.release                  100 runs          0 bad        100 good
   45/   52:  task_02_deadline.c++14.debug                    100 runs          0 bad        100 good
   46/   52:  task_02_deadline.c++14.release                  100 runs          0 bad        100 good
   47/   52:  task_02_deadline.c++17.debug                    100 runs          0 bad        100 good
   48/   52:  task_02_deadline.c++17.release                  100 runs          0 bad        100 good
   49/   52:  task_03_sort.c++14.debug                        100 runs          0 bad        100 good
   50/   52:  task_03_sort.c++14.release                      100 runs          0 bad        100 good
   51/   52:  task_03_sort.c++17.debug                        100 runs          0 bad        100 good
   52/   52:  task_03_sort.c++17.release                      100 runs          0 bad        100 good
### multivm Libpreempt(all) *** SUMMARY: 5200 runs 68 bad 5132 good
### multivm Libpreempt(all) *** SUCCESS: 98.600%
```

First the script compiles a matrix of executables for each single test where
each test is stretched over different C++ standards, optimization levels and
(optionally) CPU micro-architecures. In this way it compiles many executable
files for each single C++ file. Second, the script runs all tests repeatedly and
summarizes results based on exit codes.

A run is considered good if the process succeeds. So no test framework is
required on the C++ side. If the process returns exit code 0 it passed,
otherwise not. In the above example 68 of 5200 runs (or 1.4%) of the tests
failed. The reason for this is that the real-time patches were not active in
this kernel. **Each test is written exactly so that the success rate is always
100% over a kernel with RT scheduling.** Otherwise, there is a bug in the
kernel, runtime libraries, or build system. Basically, the idea of these tests
is to detect such errors when you change the toolchain, upgrade the system or
build a Yocto image, for example.

The use of `sudo` is only necessary if the user has no **scheduling
privileges**, in which case executing programs that use one of the real-time
scheduling policies will fail (permission denied).

Real-time framework
-------------------

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

Real-time scheduler test suite
------------------------------

Every test has the form of a C++ file and has been carefully written in a way
that (1) it runs one specific scenario and (2) is human readable. Each test will
verify several attributes of the **Linux real-time scheduler** with regard to
the POSIX scheduling policies.

### Test categories

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


COPYRIGHT
=========

Written by Andreas Spindler <info@andreasspindler.de>

For copyright and license information see file *UNLICENSE*.

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
