/* -*- coding: raw-text-unix; -*-
 *
 * Find out execution time of realtime threads. Fail if such a exceeds
 * its time slice. Print runtime and idle time information per thread.
 */
#include "common.h"

using namespace std;

/**
 * Thread that sleeps 3ms on each invocation.
 */
class SleepFor3ms : public CriticalTask<10> {
public:
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

  t1.start(2);
  t2.start(1);

  t1.join(); PRINT(t1);
  t2.join(); PRINT(t2);

  return EXIT_SUCCESS;
}
