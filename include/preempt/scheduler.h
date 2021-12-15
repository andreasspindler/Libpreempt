/* -*-coding:raw-text-unix-*-
 *
 * preempt/scheduler.h -- schedule tasks using a realtime clock thread
 */
#pragma once

#include <preempt/task.h>
#include <vector>

namespace preempt {
/**
 * Scheduler interface.
 */
class basic_scheduler {
public:
  virtual ~basic_scheduler() = 0;

  using mono_task_list = vector<mono_task<>>;
  using poly_task_list = vector<poly_task<>>;

  mono_task_list& mono_tasks();
  poly_task_list& poly_tasks();

  virtual void join_all();

private:
  mono_task_list mtasks_;
  poly_task_list ptasks_;
};

/**
 * SCHED_RR scheduler.
 */
class round_robin_scheduler : public base_scheduler {
public:
  ~round_robin_scheduler() { }
};


/**
 * SCHED_FIFO scheduler.
 */
class normal_scheduler : public base_scheduler {
public:
  ~normal_scheduler() { }
};
} // preempt
