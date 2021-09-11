#include <map>
#include <cassert>

#include <base/details/system.h>

namespace base {
  std::pair<int, int>
  get_thread_policy_and_prioritiy(pthread_t h) {
    sched_param param;
    int policy;
    if (pthread_getschedparam(h, &policy, &param))
      assert(!"pthread_getschedparam");
    return std::make_pair(policy, param.sched_priority);
  }

  bool
  is_main_thread() {
    /* gettid() returns the caller's thread ID (TID). In a single-threaded
       process, the thread ID is equal to the process ID (PID, as returned by
       getpid(2)). In a multithreaded process, all threads have the same PID,
       but each one has a unique TID. */
    return getpid() == get_current_thread_id();
  }

  int
  get_number_of_processors() {
#if RUNNING_UNDER_LINUX
    static const int num_cores = ::sysconf(_SC_NPROCESSORS_ONLN);
    return num_cores;
#else
    return 4;                   // assume quadcore
#endif
  }

  pid_t
  get_current_thread_id() {
#if RUNNING_UNDER_LINUX
    return ::syscall(__NR_gettid);
#else
    return 0;
#endif
  }

  pid_t
  get_current_process_id() {
    return ::getpid();
  }

  pid_t
  get_parent_process_id() {
    return ::getppid();
  }

  bool
  running_under_VM() {
#if RUNNING_UNDER_LINUX
    return false;
#else
    return false;
#endif /* RUNNING_UNDER_LINUX */
  }

  bool
  have_realtime_throttling() {
#if RUNNING_UNDER_LINUX
    return true;                // TBD: test
#else
    return false;
#endif /* RUNNING_UNDER_LINUX */
  }

  bool
  have_PREEMPT_RT() {
    static int result = -1;
    if (result == -1) {
#if RUNNING_UNDER_LINUX
#else
      result = false;
#endif /* RUNNING_UNDER_LINUX */
    }
    return result;
  }

  bool
  have_realtime_kernel() {
#if RUNNING_UNDER_LINUX
    return not have_realtime_throttling() && have_PREEMPT_RT();
#else
    return not have_realtime_throttling();
#endif /* RUNNING_UNDER_LINUX */
  }

  bool
  disable_core_file() {
    struct ::rlimit rl;
    rl.rlim_cur = 0;
    rl.rlim_max = 0;
    return ::setrlimit(RLIMIT_CORE, &rl) == 0;
  }

  bool
  unlimit_lock_pages() {
#ifdef RUNNING_UNDER_LINUX
    struct ::rlimit rl;
    rl.rlim_cur = RLIM_INFINITY;
    rl.rlim_max = RLIM_INFINITY;
    return ::setrlimit(RLIMIT_MEMLOCK, &rl) == 0;
#else
    return true;
#endif // RUNNING_UNDER_LINUX
  }

  bool
  lock_all_pages() {
#ifdef RUNNING_UNDER_LINUX
    return ::mlockall(MCL_CURRENT | MCL_FUTURE) == 0;
#else
    return true;
#endif // RUNNING_UNDER_LINUX
  }

  bool
  unlock_all_pages() {
#ifdef RUNNING_UNDER_LINUX
    return ::munlockall() == 0;
#else
    return true;
#endif // RUNNING_UNDER_LINUX
  }
} /* realtime::details */
