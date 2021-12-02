/* -*- coding: raw-text-unix; -*-
 *
 * Basic POSIX thread. Runs threads under SCHED_OTHER default policy
 * (non-realtime, unprioritized).
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
  char argument[] = X;

  /* thread_function1 */
  auto context = base::pthread(thread_function, argument);
  if (context) {
    context.join();
    if (context)
      return EXIT_SUCCESS;
  }

  /* thread creation or join failed */
  std::cerr << context.last_error << std::endl;
  return EXIT_FAILURE;
}
