/*
 * preempt::thread is like std::thread plus the ability to enable one of the
 * realtime scheduling policies on the running thread, as demonstrated in this
 * test.
 *
 * Use "ulimit -r" to see the maximum realtime priority the current user can
 * use.
 */
#include <preempt/process.h>
#include <preempt/thread.h>
#include <string>

#include <base/verify.h>

#include <iostream>

#define X "Om Gam Ganapataye Namaha"

void Task1() {}
void Task2() {}

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();

  preempt::thread thr1;                         // exists
  thr1 = preempt::thread(Task1);                // runs under SCHED_OTHER
  preempt::thread thr2 {SCHED_FIFO, 10, Task1}; // runs under SCHED_FIFO
  preempt::thread thr3 {SCHED_RR,   10, Task2}; // runs under SCHED_RR

  VERIFY(thr1.joinable());
#if 0
  if (!thr1.try_scheduling_policy(SCHED_RR, 1)) { // SCHED_OTHER => SCHED_RR
    std::cerr << thr1.last_error() << std::endl;
    return EXIT_FAILURE;
  }
#endif

  thr1.join();
  thr2.join();
  thr3.join();

  preempt::this_process::end_realtime();

  return EXIT_SUCCESS;
}
