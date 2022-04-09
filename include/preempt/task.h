/* -*-coding:raw-text-unix-*-
 *
 * preempt/task.h -- base classes for classes using threads
 */
#pragma once

#include <vector>
#include <mutex>

#include <preempt/thread.h>

namespace preempt {
/**
 * Basic task interface.
 */
class basic_task {
public:
  virtual ~basic_task() { }

  /**
   * Join the member thread(s).
   */
  virtual void join() = 0;

  /**
   * Test if at least one thread is joinable.
   */
  virtual bool joinable() const = 0;
};

/**
 * Task that uses a single thread to distribute work.
 *
 * Technically a wrapper around std::thread or preempt::thread with a @ref
 * basic_thread interface.
 *
 * Example:
 *
 *     struct task : base::mono_task {
 *       task() {
 *         spawn(&run, this);
 *       }
 *       void run() {
 *         // actual thread function
 *       }
 *     };
 *
 * @param Thread: std::thread or preempt::thread
 */
template <typename Thread = preempt::thread>
class mono_task : public virtual basic_task {
  Thread thread_;
public:
  using thread_type = Thread;

  /**
   * Join a possible running member thread, then start a new one.
   */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  bool joinable() const override;
  void join() override;

protected:
  Thread& thread();
};

/**
 * Task that uses a vector of threads to distribute work.
 *
 * Technically a wrapper around a std::thread or preempt::thread vector with
 * a @ref basic_thread interface.
 *
 * @param Thread: std::thread or preempt::thread
 */
template <typename Thread = preempt::thread>
class poly_task : public virtual basic_task {
  std::vector<Thread> threads_;
  std::mutex lock_;
public:
  using thread_type = Thread;

  /**
   * Push a new member thread.
   */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /**
   * Join all spawned threads.
   */
  void join() override;

  bool joinable() const override;

protected:
  std::vector<Thread>& threads();
};

/**
 * Real-time task.
 *
 * Basically a preempt::mono_task<preempt::thread> with a virtual run() method.
 */
class task : public preempt::mono_task<> {
};

/**
 * Real-time task that measures the time it consumes.
 *
 * Basically a preempt::mono_task<preempt::thread> with a virtual run() method.
 *
 * If a time record was torn then call @ref base::quick_exit (wrapper around
 * std::quick_exit).
 *
 * See @ref base::quick_exit for the reason why it doesn't make sense to throw
 * an exception.
 *
 * @param Us: Logical time slice in microseconds.
 */
template <long Us>
class critical_task : public preempt::mono_task<> {
public:
  using clock = std::chrono::high_resolution_clock;

  virtual ~critical_task() { }

  /**
   * Create a SCHED_FIFO. Time measurement works only for real-time threads that
   * are not interrupted by such with a higher priority.
   */
  void start(int priority = 1);

  /**
   * Actual thread function. May not consume more than Ms milliseconds or the
   * process terminates.
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
template <class Thread>
template <class Function, class... Args>
Thread&
mono_task<Thread>::spawn(Function&& f, Args&&... args) {
  join();                       // finished current thread first
  thread_ = Thread {std::forward<Function>(f), std::forward<Args>(args)...};
  return thread_;
}

template <class Thread>
bool
mono_task<Thread>::joinable() const {
  return thread_.joinable();
}

template <class Thread>
void
mono_task<Thread>::join() {
  /* if default constructed join throws system_error */
  if (joinable())
    thread_.join();
}

template <class Thread>
Thread&
mono_task<Thread>::thread() {
  return thread_;
}

template <class Thread>
template <class Function, class... Args>
Thread&
poly_task<Thread>::spawn(Function&& f, Args&&... args) {
  std::lock_guard<decltype(lock_)> {lock_};
  threads_.emplace_back(Thread {std::forward<Function>(f), std::forward<Args>(args)...});
  return threads_.back();
}

template <class Thread>
void
poly_task<Thread>::join() {
  for (auto& t : threads_) {
    t.join();
  }
}

template <class Thread>
bool
poly_task<Thread>::joinable() const {
  for (auto& t : threads_) {
    if (t.joinable()) {
      return true;
    }
  }
  return false;
}

template <class Thread>
std::vector<Thread>&
poly_task<Thread>::threads() {
  return threads_;
}

template <long Us>
void
critical_task<Us>::start(int priority) {
  spawn(&critical_task::hook, this).change_scheduling(SCHED_FIFO, priority);
}

template <long Us>
long
critical_task<Us>::runtime() const {
  return usec_;
}

template <long Us>
void
critical_task<Us>::hook()
{
  // TODO: use base::timeout()?
  using namespace std;
  using namespace std::chrono;
  auto start = clock::now();
  auto deadline = start + microseconds {Us};
  { run(); }
  auto stop = clock::now();
  auto us = duration_cast<microseconds>(stop - start);
  usec_ = us.count();           // just store last duration
  if (stop > deadline) {
    base::quick_exit(base::sprintf("critical_task error: deadline=%luus used=%lums", Us, usec_).c_str());
  }
}
} // preempt
