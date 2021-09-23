---
title: Libpreempt FAQ
author: Andreas Spindler <info@andreasspindler.de>
---

# Libpreempt FAQ

## What is an RTOS?

The specific description of real-time is that processes have minimum response
time guarantees. **The system should behave as predictably as possible.** This
means that it should always take up the same period of time, regardless of the
data to be processed, and this period must be measurable.

Furthermore, it is usually sufficient if it is **predictable for a specific,
practical problem** (application) and not for all conceivable questions. This is
why the term "realtime" is confusing on several levels.

**In order to achieve predictability**, the tasks that a computer executes in
parallel, must be distributed to the underlying CPU cores in such a way that no
task is disadvantaged. A task is not disadvantaged if its preset priority,
affinity and scheduling policy is always adhered to by the OS. Only real-time
tasks have a priority and a scheduling policy.

**The basis is a real-time scheduler for threads and processes**. A RT scheduler
operates in O(1) time or, under Linux, as CFS (*Completely Fair Scheduler*).
While an RTOS requires an RT scheduler it also needs predictable behaviors in
the other parts of the OS too.

## What is real-time from the view of a computer program?

> Yeah, fugazi, fogazi. It's a wazi, it's a woozi. It's fairy dust. It doesn't
> exist, it's never landed, it is no matter, it's not on the elemental charge.
> It's not f**king real.
>
> *Mark Hannah*: "The Wolf of Wall Street"

Time doesn't exist for a normal threads - they run just as fast as possible.

Time does exist for a real-time thread - it must take into account that its
functions are preemptively interrupted when its time slice expires.

All a computer (hardware + OS) can do is be as precise a clockwork as possible.
An RTOS is an OS that runs tasks as synchronized as possible with the other
clocks that are buzzing around everywhere. This is where latency comes into play
alongside time slices.

In general, the simpler the hardware is the more predictable and therefore
"real-time" it is. By removing the mechanical components clocks have become more
accurate. By adding bus systems, protocols, buffers and networks, one limits the
predictability with which they can perform an operation again.

## What is (real) time in nature?

[Time](https://en.wikipedia.org/wiki/Time) in physics is operationally defined as "what a clock reads". What else
is time is above all mysterious.

[Predictability](https://en.wikipedia.org/wiki/Predictability) on the other hand is a little fuzzy on the more natural side
of things. Sometimes it is more like a mathematical probability, sometimes a
[simultaneity](https://en.wikipedia.org/wiki/Relativity_of_simultaneity), but it can never be absolutely determined and we will leave
it at that.

And while *time* can only be measured but not understood or programmed, *timing*
does exist for all of us. And this experience, this flow, this perfect sequence
of events that we "time", each one in his own way, could only be tarnished by
something like absolute time.
!
<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
