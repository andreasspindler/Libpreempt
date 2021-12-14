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
  assert(base::sec_to_nsec(1) == 1000000000L);
  assert(base::sec_to_usec(1) ==    1000000L);
  assert(base::sec_to_msec(1) ==       1000L);
  assert(base::nsec_to_sec(1000000000L) == 1);
  assert(base::usec_to_sec(   1000000L) == 1);
  assert(base::msec_to_sec(      1000L) == 1);

  /*********************************************
   * Basic POSIX thread. Runs threads under SCHED_OTHER default policy
   * (non-realtime, unprioritized).
   */
  char argument[] = X;
  auto context = base::pthread(thread_function, argument);
  VERIFY(context);
  context.join();
  VERIFY(!context);

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
