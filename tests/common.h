/*
 * tests/common.h -- Common test classes
 */
#include <chrono>
#include <iostream>

#include <base/testing.h>
#include <base/idioms.h>

#include <preempt/task.h>

/**
 * This task runs a thread that measures the time it consumes and
 * exits if a time slice was not adhered to.
 *
 * @param Ms: Logical time slice in milliseconds. Throws
 *            deadline_error if the thread function body() runs
 *            longer.
 */
template <int Ms>
class CriticalTask : public preempt::mono_task<>, public base::runnable {
public:
  using clock = std::chrono::high_resolution_clock;

  virtual ~CriticalTask() { }

  /**
   * Create a SCHED_FIFO. Time measurement works only for realtime
   * threads that are not interrupted by such with a higher priority.
   */
  void start(int priority = 1);

  /**
   * Actual thread function. May not consume more than Ms milliseconds
   * or a base::deadline_error is thrown.
   */
  virtual void run() = 0;

  /**
   * Runtime in microseconds.
   */
  long runtime() const;

private:
  void hook();

  long usec_;
};

/***********************************************************************
 * inlined implementation
 */
template <int Ms>
void
CriticalTask<Ms>::start(int priority) {
  spawn(&CriticalTask::hook, this).change_scheduling(SCHED_FIFO, priority);
}

template <int Ms>
long
CriticalTask<Ms>::runtime() const {
  return usec_;
}

template <int Ms>
void
CriticalTask<Ms>::hook()
{
  using namespace std;
  using namespace std::chrono;
  auto start = clock::now();
  auto deadline = start + milliseconds {Ms};
  { run(); }
  auto stop = clock::now();
  if (stop > deadline) {
    base::terminate("CriticalTask: deadline error");
  } else {
    auto us = duration_cast<microseconds>(stop - start);
    usec_ = us.count();
  }
}

#define PRINT(t) \
  if ((1)) { \
    std::cerr << #t << ": runtime = " << t.runtime() << " usec" << std::endl; \
  } else
