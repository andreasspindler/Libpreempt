#include <preempt/process.h>

#include <base/details/yield.h>

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
#if 0
    void
    terminate_unless_rtprio(int required_priority) {
      min_fifo = sched_get_priority_min(SCHED_FIFO);
      max_fifo = sched_get_priority_max(SCHED_FIFO);

      if (getrlimit(RLIMIT_RTPRIO, &rl) != 0) {
        error_ = base::sprintf("FAILED: pthread_setschedparam(): '%s'", std::strerror(errnum));
        e = errno;
        printf("Failed to getrlimit(): %s.\n", strerror(e));
        std::terminate();
      }
    }
#endif
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

    void
    yield() {
      base::details::yield();
    }
  }
}
