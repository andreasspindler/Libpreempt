/*
 * tests/common.h -- Common test classes
 */
#include <chrono>
#include <iostream>

#include <base/utility.h>
#include <base/string.h>

#include <preempt/task.h>

#define PRINT(t) \
  if ((1)) { \
    std::cerr << #t << ": runtime = " << t.runtime() << " usec" << std::endl; \
  } else
