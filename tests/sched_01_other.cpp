/* -*- coding: raw-text-unix; -*-
 *
 * SCHED_OTHER non-realtime test
 */
#include <preempt/pthread.h>

#include <cassert>
#include <iostream>

void* thread_function(void* arg)
{
  pthread_t id = pthread_self();
  assert(std::string(static_cast<char const*>(arg)) == "test");
  return nullptr;
}

int main(int argc, char *argv[])
{
  using namespace preempt;

  char argument[] = "test";

  auto context = pthread::create(thread_function, argument);
  if (context) {
    context = pthread::join(context);
    if (context)
      return EXIT_SUCCESS;
  }
  std::cerr << context.error << std::endl;
  return EXIT_FAILURE;
}
