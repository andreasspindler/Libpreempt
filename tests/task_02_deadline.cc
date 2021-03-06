/* -*- coding: raw-text-unix; -*-
 *
 * Find out execution time of realtime threads.
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

using namespace std;

/**
 * Task that sleeps 3ms on each invocation.
 *
 * Fails if run() runs longer than 20ms. Note that between the sleep() calls
 * possible context switches can occur.
 */
struct SleepFor3ms : preempt::critical_task<20000> {
  void run() override {
    this_thread::sleep_for(chrono::milliseconds      {1});
    this_thread::sleep_for(chrono::microseconds   {1000});
    this_thread::sleep_for(chrono::nanoseconds {1000000});
  };
};

int
main(int argc, char *argv[])
{
  SleepFor3ms t1;
  SleepFor3ms t2;

  t1.start(32);
  t2.start(31);

  t1.join(); PRINT(t1);
  t2.join(); PRINT(t2);

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
