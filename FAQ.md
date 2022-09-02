---
title: Libpreempt FAQ
author: Andreas Spindler <info@andreasspindler.de>
---

Libpreempt
==========
C++ real-time test suite and framework for writing embedded firmwares.

FAQ
---

### What does "real-time" mean for a program?

It means **the program should behave as predictably as possible**. The specific
description of *real-time* is that processes/threads have minimum response time
guarantees. That means the thread must be finished in a preset period of time,
regardless of the data to be processed, and this period must be measurable.

Real-time processes and threads are in charge of critical tasks whose execution
cannot be interrupted (usually involved in multimedia processing). Since
real-time processes/threads prevent any non-real-time processes/threads from
running, in general their life-time should be short.

This idea is implemented by `preempt::critical_task` which is a wrapper around
`preempt::thread`. The `preempt::thread` class is equal to `std::thread` but
additionally can change the scheduling policy (it is basically a real-time
capable `std::thread`).

### What is an RTOS?

**The basis of an RTOS is a real-time scheduler for threads and processes**. The
RT scheduler implements the scheduling policy and priority associated with a
thread. While an RTOS requires an RT scheduler it also needs predictable
"real-time behaviors" in the other parts of the OS too.

<!-- The Linux scheduler operates in O(1) time or, under Linux, as CFS
(*Completely Fair Scheduler*). -->

### How predictable is an RTOS?

It is usually sufficient if the system is **predictable for a specific,
practical problem** and not for all conceivable questions. In order to achieve
predictability real-time threads must be distributed to the underlying CPU cores
in such a way that no thread is disadvantaged. A thread is not disadvantaged if
its preset priority, affinity and scheduling policy is always adhered to by the
RTOS. Only real-time threads have a priority and a scheduling policy.

### Is an RTOS required to use real-time scheduling?

No. Real-time scheduling is a POSIX function available on all Linux kernels or
compatibility layers such as Cygwin. A kernel with an RT scheduler is needed if:

- You want to run software with very low latency settings that require
  performance that can only be achieved with an RT kernel.

- Your hardware configuration triggers poor latency behaviour which might be
  improved with an RT kernel.

### Why can real-time fail?

Only four reasons are known to the author.

(1) The user has no **scheduling privileges**

Use `ulimit -r` to see the maximum real-time priority the current user can use.
If this value is 0 then the current user is not allowed to use real-time
scheduling policies. In this case, some test will fail 100%. See also
*/etc/security/limits.conf*.

(2) Linux **PREEMPT_RT** patches are not installed

The code compiles because POSIX does not require an RTOS; in other words, a
kernel can implement `SCHED_FIFO` and `SCHED_RR` with best effort. But some
tests proposed by Libpreempt have so tight timing (intentionally) that they must
fail if the kernel is not patched (for example, *preempt_02_fifo.cc* and
*preempt_03_decrement.cc*).

(3) Linux **RT throttling** enabled

If tests only fail sometimes (not 100%) then Linux RT throttling is likely
enabled.

(4) **Page-faults** have occured

The only other thing known to mess with the real-time scheduling policies is the
memory manager. To prevent page-faults all pages must be locked before creating
a real-time thread.
  
To prevent page faults and disable RT throttling the general form a `main`
function that uses Libpreempt is:

```c++
#include <preempt/process.h>

int main(int argc, char *argv[])
{
  preempt::this_process::begin_real_time();
  {
    /*
     * all memory locked - the VMM won't interrupt the process
     *
     * all real-time threads started in this scope will work
     */
  }
  preempt::this_process::end_real_time();
  return 0
}
```

### How to check if PREEMPT_RT patches are installed?

``` sh
 $ uname -v
 #60~20.04.1-Ubuntu SMP PREEMPT Thu May 6 09:52:46 UTC 2021
                         ^
                         |
                         |
```

## Does the Yocto-Linux kernel support RT scheduling?

Yes.

### Can a VM run real-time tests?

Yes. As long as the kernel actively supports real-time scheduling there
shouldn't be any limitations in a VM.

### Why can the real-time test throw an exception?

Trying to join a default-constructed `std::thread` may throw a
`std::system_error`.

### Why can the test print "terminate called without an active exception"?

The `std::terminate()` function of the GNU C++ standard library prints:

    terminate called without an active exception

Since the `std::thread` dtor calls `std::terminate()` for unjoined threads the
cause of this message can be an unjoined thread.

### How does preemptive multitasking work for real-time systems?

Preemptive multitasking creates the illusion that multiple processes and threads
run concurrently on a single processor, while in reality they are actually only
assigned a time slice. If the time slice is exceeded, the process or thread is
"preempted", i.e. interrupted and has to wait for its next time slice.

Real-time threads are an exception here. They are only interrupted if a thread
with a higher priority (**FIFO**) or the same priority (**RR**) is to run, or if
`sched_yield()` is called explicitly. Therefore a real-time thread can take over
and paralyze the entire system if it does not return quickly enough or - in the
case of longer tasks - voluntarily calls `sched_yield()`.

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
to the real-time category. On **multicore systems** the kernel will not run
multiple real-time threads on multiple cores simultaneously, which would violate
the defined behavior.

A thread scheduled `SCHED_FIFO` or `SCHED_RR`, when selected for running, will
continue to use the same CPU core (blocking all other real-time threads) until
either it is blocked by an I/O request it creates, it is preempted by a higher
priority `SCHED_FIFO` or `SCHED_RR`, or it calls `sched_yield()`.

### How do time-slices affect SCHED_FIFO and SCHED_RR?

They don't. `SCHED_FIFO` and `SCHED_RR` are scheduling algorithms that
implemented a form of cooperative multitasking without time slicing.

### What if two or more ctors with SCHED_FIFO or SCHED_RR run parallely in a program?

Threads created by `pthread_create()` or the `std::thread` ctor will never
violate the specified scheduling policy. So no synchronization measures have to
be taken when threads are created. But be aware that the thread might already
have started before `pthread_create()` or the `std::thread` ctor returned.

If two `SCHED_FIFO` threads with the same priority are created one after the
other (e.g. by two consecutive `std::thread` ctor calls), then the thread of the
first call is always executed first. However, if the second thread created has a
higher priority, it immediately interrupts the first.

### Under what circumstances does the kernel change a thread's policy or priority?

Never. The kernel cannot change the policy or priority of a process or thread on
its own.

### Which priorities are valid for SCHED_FIFO and SCHED_RR?

In POSIX 1 to 32. Linux extends this to 1 to 99. Note that `SCHED_FIFO` can be
used only with static priorities higher than 0, which means that when a
`SCHED_FIFO` becomes runnable, it will always immediately preempt any currently
running `SCHED_OTHER`, `SCHED_BATCH`, or `SCHED_IDLE` thread.

Creating a `SCHED_FIFO` or `SCHED_RR` thread with priority 0 is an error.

https://man7.org/linux/man-pages/man7/sched.7.html

### How to grant a user scheduling privileges? 

Set the `CAP_SYS_NICE` capability of the executable.

https://manpages.debian.org/stretch/manpages/capabilities.7.en.html

Copyright
---------

Written by Andreas Spindler <info@andreasspindler.de>

For copyright and license information see file *UNLICENSE*.

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
