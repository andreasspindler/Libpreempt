#include <preempt/process.h>

namespace preempt {
  pid_t
  get_current_process_id() {
    return ::getpid();
  }

  pid_t
  get_parent_process_id() {
    return ::getppid();
  }

  namespace this_process {
    void
    begin_realtime() {
      unlimit_lock_pages();
      lock_all_pages();
    }

    void
    end_realtime() {
      unlock_all_pages();
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

    bool
    disable_core_file() {
      struct ::rlimit rl;
      rl.rlim_cur = 0;
      rl.rlim_max = 0;
      return ::setrlimit(RLIMIT_CORE, &rl) == 0;
    }
  }
}
