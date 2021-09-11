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

Usually time doesn't exist for a process. Any process runs just as fast as
possible and they don't know if they will be able to run the next opcode, if the
board will burn out or the universe will cease to exist.

That's because computers are just clocks, too. Incredibly complex, programmable,
clocks with a gigahertz rate and interrupts over which the kernel assigns time
slices to processes, while the clock is networked with other clocks that are
doing the same.

No hardware and no OS, no matter how sophisticated, can conjure up "real time".
All it can do is be as precise a clockwork as possible. From that perspective it
becomes a bit clearer again what an RTOS is: an OS that runs tasks as
synchronized as possible with the other clocks that are buzzing around
everywhere. If it can do this trick we call it predictable.

In general, the simpler the hardware is the more predictable and therefore
"real-time" it is. By removing the mechanical components clocks have become more
accurate. By adding bus systems, protocols, buffers and networks, one limits the
predictability with which they can perform an operation again (latency).

## What is (real) time in nature?

On the more natural side of things it will be a little fuzzy because there is no
"predictability" only probabilities. The closest approximation of what a
computer scientist calls *predictability* is perhaps *simultaneity* for a
physicist.

If we look on Wikipedia under "Time" we will find a cleverly phrased sentence,
so sophisticated that it borders on pointlessness: "time [is] the continued
sequence of existence and events that occurs in an apparently irreversible
succession from the past, through the present, into the future". Nice. But what
were "past", "present" and "future" again? So one tries to explain time with
temporal states.

We have learned something deeper nevertheless: if *time* is anything than that
it corresponds to the energy it costs our brain to digest Wikipedia's definition
of time.

Fortunately, the author(s) of this page knew that too - and now make a giant
leap and speak about measurement and clocks. If we curiously go to the page on
the subject of clocks, it says there that clocks measure time. Deadlock again.

This is not a failure of Wikipedia. Amazingly, there is no mathematical term for
*time* in Einstein's GRT. There is only mass and energy. With the GRT we
demystified the ether - that cosmic stage on which gravity acts. But we have not
completely gotten rid of the ether because we still cling to the idea that there
is something like universal time that we all share. Yet we don't know what or
where it is.

And while *time* can only be measured but not understood or programmed, *timing*
does exist for all of us. And this experience, this flow, this perfect sequence
of events that we "time", each one in his own way, could only be tarnished by
something like universal [real] time.

<!--
Local Variables:
fill-column: 80
indent-tabs-mode: nil
markdown-enable-math: t
End:
-->
