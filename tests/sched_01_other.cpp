/* -*- coding: raw-text-unix; -*-
 *
 * SCHED_OTHER test
 *
 * Runs threads under SCHED_OTHER default policy (non-realtime, unprioritized).
 */
#include <preempt/posix_thread.h>
#include <base/verify.h>

#include <iostream>

#define X "Om Namaha Shivaya"

void* thread_function1(void* arg)
{
  pthread_t id = pthread_self();
  VERIFY(std::string(static_cast<char const*>(arg)) == X);
  return nullptr;
}

int main(int argc, char *argv[])
{
  using namespace preempt;

  char argument[] = X;

  /* thread_function1 */
  auto context = posix_thread(thread_function1, argument);
  if (context) {
    context.join();
    if (context)
      return EXIT_SUCCESS;
  }

  /* thread creation or join failed */
  std::cerr << context.last_error << std::endl;
  return EXIT_FAILURE;
}
