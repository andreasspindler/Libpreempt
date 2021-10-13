/* -*- coding: raw-text-unix; -*-
 *
 * SCHED_OTHER test
 *
 * Runs threads under SCHED_OTHER default policy (non-realtime, unprioritized).
 */
#include <preempt/posix.h>
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
  auto context = pthread::create(thread_function1, argument);
  if (context) {
    context = pthread::join(context);
    if (context)
      return EXIT_SUCCESS;
  }

  /* thread creation or join failed */
  std::cerr << context.error << std::endl;
  return EXIT_FAILURE;
}
