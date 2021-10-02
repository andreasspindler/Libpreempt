/* -*- coding: raw-text-unix; -*-
 *
 * POSIX thread on global and class member functions
 */
#include <base/pthread.h>
#include <base/debug.h>

#include <stdexcept>
#include <functional>
#include <string>
//#include <cstdlib>
#include <iostream>

void* called_by_thread(void* arg)
{
  pthread_t id = pthread_self();
  assert(std::string(static_cast<char const*>(arg)) == "test");
  return nullptr;
}

int main(int argc, char *argv[])
{
  char argument[] = "test";
  base::pthread::context result = base::pthread::create(called_by_thread, argument);
  if (result) {
    result = base::pthread::join(result);
    if (result)
      return EXIT_SUCCESS;
  }
  std::cerr << result.error << std::endl;
  return EXIT_FAILURE;
}
