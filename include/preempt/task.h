/* -*-coding:raw-text-unix-*-
 *
 * preempt/task.h -- base classes for classes using threads
 */
#pragma once

#include <vector>

#include <preempt/thread.h>

namespace preempt {
/**
 * Basic task interface.
 */
class basic_task {
public:
  /**
   * Join the member thread(s).
   */
  virtual void join() = 0;
};

/**
 * Contains a single thread to distribute work.
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
   * Join, then start a new member thread.
   */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /**
   * Join the member thread.
   */
  void join() override;

protected:
  Thread& thread();
};

/**
 * Contains a thread vector to distribute work.
 *
 * @param Thread: std::thread or preempt::thread
 */
template <typename Thread = preempt::thread>
class poly_task : public virtual basic_task {
  std::vector<Thread> threads_;
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

protected:
  std::vector<Thread>& threads();
};

/***********************************************************************
 * inlined implementation
 */
template <class Thread>
template <class Function, class... Args>
Thread&
mono_task<Thread>::spawn(Function&& f, Args&&... args) {
  join();
  thread_ = Thread
    {std::forward<Function>(f), std::forward<Args>(args)...};
  return thread_;
}

template <class Thread>
void
mono_task<Thread>::join() {
  if (thread_.joinable()) {
    thread_.join();
  }
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
std::vector<Thread>&
poly_task<Thread>::threads() {
  return threads_;
}
} // preempt
