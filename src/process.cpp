#include <preempt/all.h>

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
    error_ = base::sprintf("FAILED: getrlimit(): '%s'", std::strerror(errno));
    std::quick_exit(EXIT_FAILURE);
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
  base::yield();
}
}

#if RUNNING_UNDER_LINUX
static int g_fd_cpu_dma {-1};

void
request_CPU_latency(unsigned c_state)
{
  struct ::stat st;
  if (::stat("/dev/cpu_dma_latency", &st) == -1) {
    std::perror("stat /dev/cpu_dma_latency)");
    std::exit(EXIT_FAILURE);
  }

  if ((g_fd_cpu_dma = ::open("/dev/cpu_dma_latency", O_RDWR)) == -1) {
    std::perror("open /dev/cpu_dma_latency");
    std::exit(EXIT_FAILURE);
  }

  if (::write(g_fd_cpu_dma, &c_state, sizeof(unsigned)) < 1) {
    std::perror("write /dev/cpu_dma_latency");
    reset_CPU_latency();
    std::exit(EXIT_FAILURE);
  }
}

void
reset_CPU_latency()
{
  if (g_fd_cpu_dma != -1) {
    ::close(g_fd_cpu_dma);
    g_fd_cpu_dma = -1;
  }
}
#else
void
request_CPU_latency(unsigned c_state)
{
  (void) c_state;
}

void
reset_CPU_latency()
{
}
#endif
}
