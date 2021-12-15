/*
 * Basic tests for class preempt::thread
 *
 * preempt::thread is like std::thread plus the ability to enable one of the
 * realtime scheduling policies on the running thread, as demonstrated in this
 * test.
 */
#include <preempt/thread.h>
#include <preempt/process.h>

#include <base/verify.h>

#include <string>

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

  // TBD: pre::thread::try_schedulung
  thr1.join();
  thr2.join();
  thr3.join();

  preempt::this_process::end_realtime();

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
