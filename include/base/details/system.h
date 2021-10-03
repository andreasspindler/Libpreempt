/* -*-coding:raw-text-unix-*-
 *
 * base/details/system.h -- include POSIX and system headers
 */
#pragma once

#include <base/details/cc.h>

#if defined(__linux__)
#define RUNNING_UNDER_LINUX 1
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>          // POSIX semaphores
#include <sched.h>              // Linux scheduling
#include <sys/mman.h>           // mlockall()
#include <sys/types.h>          // gettid()
#include <sys/resource.h>       // rlimit
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif defined(__CYGWIN__)
#define RUNNING_UNDER_CYGWIN 1
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sched.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif defined(_WIN32)
#define RUNNING_UNDER_WINDOWS 1
#pragma warning(push, 0)
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#pragma warning(pop, 0)
#else
#warning unknown target OS
#endif /* __linux__, __unix__, __VXWORKS__ */

//#include <map>
#include <utility>
//#include <limits>

namespace base {
  template <int Policy>
  int
  get_min_priority() {
#if RUNNING_UNDER_LINUX
    return ::sched_get_priority_min(Policy);
#else
    switch (Policy) {
    case SCHED_OTHER:
      return 0;
    case SCHED_FIFO:
    case SCHED_RR:
      return 1;
    }
#endif
  }

  template <int Policy>
  int
  get_max_priority() {
#if RUNNING_UNDER_LINUX
    return ::sched_get_priority_max(Policy);
#else  // POSIX defaults
    switch (Policy) {
    case SCHED_OTHER:
      return 0;
    case SCHED_FIFO:
    case SCHED_RR:
      return 32;
    }
#endif
  }

  /**
   * @return First is policy, second is priority.
   */
  std::pair<int, int>
  get_thread_policy_and_prioritiy(pthread_t);

  /**
   * @return True if this is the main thread, false otherwise.
   */
  bool
  is_main_thread();

  /**
   * @param T: Usually pthread_t, but not so under Windows or VxWorks.
   */
  template <typename T>
  bool
  is_equal_thread(T a, T b) {
    return ::pthread_equal(a, b);
  }

  int
  get_number_of_processors();

  pid_t
  get_current_thread_id();

  /**
   * @return True if (possibly) running under VM, false otherwise.
   */
  bool
  running_under_VM();

  /**
   * @brief Test if realtime throttling is enabled
   *
   * Kernels shipped since 2.6.25 have set the rt_bandwidth value for the
   * default group to be 0.95 out of every 1.0 seconds. In other words, the
   * group scheduler is configured, by default, to reserve 5% of the CPU for
   * tasks not scheduled by the realtime policies SCHED_FIFO and SCHED_RR.
   *
   *       /proc/sys/kernel/sched_rt_period_us
   *       /proc/sys/kernel/sched_rt_runtime_us
   *
   * Under Linux the default settings rt_period=1000000 (1 s) and
   * rt_runtime=950000 (0,95 s) give a limit of 95% CPU utilization.
   *
   * Note also RLIMIT_RTTIME.
   *
   * @return True if the above files read the default values mentioned
   *         above, false otherwise.
   */
  bool
  have_realtime_throttling();

  /**
   * @brief Test if running under Linux kernel with PREEMPT_RT patches
   *
   * @return True if kernel has PREEMPT_RT patches, false otherwise.
   */
  bool
  have_PREEMPT_RT();

  bool
  have_realtime_kernel();

  bool
  kernel_has_PREEMPT_RT();

  bool
  kernel_is_realtime_kernel();
} /* base */
