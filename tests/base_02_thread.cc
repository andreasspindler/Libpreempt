/* -*- coding: raw-text-unix; -*-
 *
 * Base library self tests.
 */
#include <base/all.h>

#define X "Om Gam Ganapataye Namaha"

void* thread_function(void* arg) {
  pthread_t id = pthread_self();
  VERIFY(std::string(static_cast<char const*>(arg)) == X);
  return nullptr;
}

int main(int argc, char *argv[])
{
  /*********************************************
   * base/chrono.h
   */
  VERIFY(base::sec_to_nsec(1) == 1000000000L);
  VERIFY(base::sec_to_usec(1) ==    1000000L);
  VERIFY(base::sec_to_msec(1) ==       1000L);
  VERIFY(base::nsec_to_sec(1000000000L) == 1);
  VERIFY(base::usec_to_sec(   1000000L) == 1);
  VERIFY(base::msec_to_sec(      1000L) == 1);

  /*********************************************
   * Basic POSIX thread. Runs threads under SCHED_OTHER default policy
   * (non-realtime, unprioritized).
   */
  char argument[] = X;
  auto context = base::thread(thread_function, argument);
  VERIFY(context);
  context.join();
  VERIFY(!context);

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
