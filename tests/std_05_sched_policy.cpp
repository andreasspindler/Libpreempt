/*
 * std_05_sched_policy.cpp
 *
 * preempt::thread is like std::thread plus the ability to enable one of the
 * realtime scheduling policies on the running thread, as demonstrated in this
 * test.
 *
 * Use "ulimit -r" to see the maximum realtime priority the current user can
 * use. See also /etc/security/limits.conf
 */
#include <preempt/process.h>
#include <preempt/thread.h>
#include <string>

#include <base/verify.h>

#include <iostream>

#define X "Om Gam Ganapataye Namaha"

void Task1() { }

void Task2() { }

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();

  preempt::thread thr1;                        // exists
  thr1 = preempt::thread(Task1);               // runs under SCHED_OTHER
  preempt::thread thr2 {SCHED_FIFO, 1, Task1}; // runs under SCHED_FIFO
  preempt::thread thr3 {SCHED_RR,   1, Task2}; // runs under SCHED_RR

  // TODO: change policy of thr1
  thr1.join();
  thr2.join();
  thr3.join();

  preempt::this_process::end_realtime();

  return EXIT_SUCCESS;
}
