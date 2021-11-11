---
title: Libpreempt README
author: Andreas Spindler <info@andreasspindler.de>
---

# Libpreempt FAQ

## What is an RTOS?

**The basis of an RTOS is a real-time scheduler for threads and processes**. A
RT scheduler operates in O(1) time or, under Linux, as CFS (*Completely Fair
Scheduler*). While an RTOS requires an RT scheduler it also needs predictable
"real-time behaviors" in the other parts of the OS too.

The specific description of "real-time" is that processes/threads have minimum
response time guarantees. **The system should behave as predictably as
possible.** This means that it should always take up the same period of time,
regardless of the data to be processed, and this period must be measurable.

In general, real-time processes and threads are in charge of critical tasks
whose execution cannot be interrupted (usually involved in multimedia
processing). Because real-time processes/threads prevent any non-real-time
processes/threads from running, in general their life-time should be short.

**In order to achieve predictability**, the tasks that a computer executes in
parallel, must be distributed to the underlying CPU cores in such a way that no
task is disadvantaged. A task is not disadvantaged if its preset priority,
affinity and scheduling policy is always adhered to by the OS. Only real-time
tasks have a priority and a scheduling policy.

## How predictable is an RTOS?

It is usually sufficient if the system is **predictable for a specific,
practical problem** (application) and not for all conceivable questions.

## Is a RTOS required to use real-time scheduling?

No. Real-time scheduling is a POSIX function available on all Linux kernels. A
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
                         indicates RTOS
```

## Does Yocto-Linux has a real-time-kernel?

Yes.

## How does preemptive multitasking work for real-time systems?

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

## How does SCHED_FIFO and SCHED_RR work on multi-core systems?

`SCHED_FIFO` and `SCHED_RR` processes/threads belong to the category of the
real-time (RT) processes/threads.

On **multicore systems** the kernel will not run multiple threads on multiple
cores simultaneously, which would violate the defined behavior. A thread
scheduled `SCHED_FIFO` or `SCHED_RR`, when selected for running, will continue
to use the CPU until either it is blocked by an I/O request, it is preempted by
a higher priority `SCHED_FIFO` or `SCHED_RR`, or it calls `sched_yield()`.

## How do time-slices affect SCHED_FIFO and SCHED_RR?

They don't. `SCHED_FIFO` is a simple scheduling algorithm without time slicing.
`SCHED_RR` is a simple scheduling algorithm with time slice for
processes/threads with the same priority.

- The thread **may or may not have started when `pthread_create()` returns** but
  `SCHED_FIFO` threads are always executed in the order of their creation.

## Which priorities are valid for SCHED_FIFO and SCHED_RR?

In POSIX 1-32, under Linux 1-99.  The priority is statically and can't be
changed by the kernel.

A source of error is to pass 0 as the priority. `SCHED_FIFO` can be used only
with static priorities higher than 0, which means that when a `SCHED_FIFO`
becomes runnable, it will always immediately preempt any currently running
`SCHED_OTHER`, `SCHED_BATCH`, or `SCHED_IDLE`.

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
