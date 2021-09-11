/*
 * process_0.cpp -- Minimal realtime process
 */
#include <base/details/system.h>

#include <sstream>

int run()
{
  /*
   * this code protected from page locks
   *
   * therefore it is safe to start realtime threads
   */
  return 0;
}

int main(int argc, char *argv[])
{
  base::unlimit_lock_pages();
  base::lock_all_pages();

  run();

  base::unlock_all_pages();
  return 0;
}

#if 0
std::string
get_limits_as_string() {
  using namespace std;
  ostringstream R;

#define X(a) #a
  int policy = sched_getscheduler(0);
  int prio_min = sched_get_priority_min(policy),
    prio_max = sched_get_priority_max(policy);
  R << " - process scheduling policy="
    << (policy == SCHED_OTHER    ? X(SCHED_OTHER) :
        policy == SCHED_FIFO     ? X(SCHED_FIFO)  :
        policy == SCHED_RR       ? X(SCHED_RR)    : "?")
    << " min=" << prio_min
    << " max=" << prio_max << endl;
#undef X

#if RUNNING_UNDER_LINUX
  map<int, rlimit> rlimits;
  rlim_t maxval = numeric_limits<rlim_t>::max();
#define X(L)                                                            \
  do {                                                                  \
    if (getrlimit(L, &rlimits[L]))                                      \
      assert(!"getrlimit");                                             \
    R << " - " #L " soft=";                                             \
    if (rlimits[L].rlim_cur == maxval) R << "max"; else                 \
      R << rlimits[L].rlim_cur;                                         \
    R << " hard=";                                                      \
    if (rlimits[L].rlim_max == maxval) R << "max"; else                 \
      R << rlimits[L].rlim_max;                                         \
    R << endl;                                                          \
  } while ((0))

  X(RLIMIT_AS);
  X(RLIMIT_CPU);
  X(RLIMIT_RTPRIO);
  X(RLIMIT_NICE);
  X(RLIMIT_NPROC);
  X(RLIMIT_RTTIME);
  X(RLIMIT_STACK);
#undef X
#endif // RUNNING_UNDER_LINUX
  return R.str();
}
#endif
