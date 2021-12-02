/* -*-coding:raw-text-unix-*-
 *
 * base/details/yield.h -- CPU_RELAX(), busyloop(), yield()
 */
#pragma once

#include <base/details/system.h>

namespace base {
namespace details {
/**
 * @brief Improves the performance of busy loops
 *
 * According to the Intel Architectures Optimization Reference Manual, adding a
 * PAUSE instruction to the body of all spin loops improves performance on
 * Pentium 4 CPUs and later. The PAUSE instruction provides a hint to the CPU
 * that the code being executed is a spin-wait loop (busy loop).
 *
 * PAUSE is backwards compatible (turns into a NOP on older CPUs).
 */
#if __GNUC__
inline void CPU_RELAX() { asm("pause"); }
#else
inline void CPU_RELAX() { }
#endif /* __GNUC__ */

/**
 * @param jiffy: Some short amount of time (implement for-loop to jiffy).
 */
inline
void
busyloop(unsigned long jiffy) {
  CPU_RELAX();
  for (volatile unsigned long i = 0; i < jiffy; i++)
    ;
}

/**
 * Trigger a thread context switch (if any thread is waiting).
 */
inline
void
yield() {
#if RUNNING_UNDER_LINUX
  ::sched_yield();
#else
  ::usleep(1);                  // 1us should trigger context switch
#endif // RUNNING_UNDER_LINUX
}
}
}
