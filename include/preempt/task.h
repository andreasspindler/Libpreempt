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
 * @param Thread: std::thread or preempt::thread
 */
template <typename Thread>
class mono_task : public virtual basic_task {
protected:
  Thread thread_;
public:
  /**
   * Join, then start a new member thread.
   */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /**
   * Join the member thread.
   */
  void join() override;
};

/**
 * Contains a thread vector to distribute work.
 *
 * @param Thread: std::thread or preempt::thread
 */
template <typename Thread>
class poly_task : public virtual basic_task {
protected:
  std::vector<Thread> threads_;
public:
  /**
   * Push a new member thread.
   */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /**
   * Join all spawned threads.
   */
  void join() override;
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
template <class Function, class... Args>
Thread&
poly_task<Thread>::spawn(Function&& f, Args&&... args) {
  threads_.emplace_back(Thread {std::forward<Function>(f),
                                std::forward<Args>(args)...});
  return threads_.back();
}

template <class Thread>
void
poly_task<Thread>::join() {
  for (auto& t : threads_) {
    t.join();
  }
}
} // preempt
