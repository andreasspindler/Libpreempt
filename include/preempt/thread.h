/* -*-coding:raw-text-unix-*-
 *
 * preempt/thread.h -- thread with priority and same interface as std::thread
 */
#pragma once

#include <preempt/posix.h>

#include <base/details/system.h>
#include <base/string.h>

#include <thread>
#include <memory>
#include <cstring>              // std::strerror
#include <string>

namespace preempt {
/**
 * @brief Like std::thread but allow POSIX scheduling policies and a priority
 *
 * std::thread and preempt::thread have the same interface. preempt::thread has
 * a constructor that accepts a priority and an additional method @ref
 * change_scheduling_policy.
 *
 * Example:
 *
 *     preempt::thread thr;            // exists
 *     thr = preempt::thread(function) // runs normally
 *     thr = preempt::thread<SCHED_FIFO>(10, function) // runs with priority
 *
 * @see https://en.cppreference.com/w/cpp/thread/thread
 */
class thread
{
public:
  using native_handle_type = std::thread::native_handle_type;
  using id = std::thread::id;

  /** Creates new thread object which does not represent a thread. */
  thread() noexcept;

  /** Move constructor. Constructs the thread object to represent the thread of
      execution that was represented by other. After this call other no longer
      represents a thread of execution. */
  thread(thread&& other) noexcept;

  /** Construct new, normal thread object. Under POSIX this means a thread
      running under the SCHED_OTHER scheduling policy and priority 0.
  */
  template<class Function, class... Args>
  explicit thread(Function&& f, Args&&... args);

  /** Start a realtime thread with a certain scheduling policy and priority.
      Before the ctor returns the thread may not only begin execution, but also
      can preempt any other threads with a lower priority (@ref change_scheduling_policy).
  */
  template<class Function, class... Args>
  explicit thread(int policy, int priority, Function&&, Args&&...);

  /** Free the occupied system resources. */
  ~thread();

  /** Move thread object. Assign the state of other to *this and sets other to a
      default constructed state.

      After this call, this->get_id() is equal to the value of other.get_id()
      prior to the call, and other no longer represents a thread of execution.
  */
  thread& operator = (thread&& other) noexcept;

  /** Checks if this object identifies an active thread of execution.
      A thread is generally not joinable if:
       - it was default-constructed
       - it has been moved
       - detach() has been called

      Specifically, returns false if get_id() == id. So a default constructed
      thread is not joinable. In fact calling join() on a thread that is not
      joinable may throw a runtime error.

      A thread that has finished executing code, but has not yet been joined is
      still considered an active thread of execution and is therefore joinable.
  */
  bool
  joinable() const noexcept;

  /** Returns a value identifying the thread associated with *this. */
  id get_id() const noexcept;

  /** Returns the implementation defined underlying thread handle. */
  native_handle_type
  native_handle();

  /** Blocks the current thread until the thread identified by *this finishes
      its execution. The completion of the thread identified by *this
      synchronizes with the corresponding successful return from join().

      No synchronization is performed on *this itself. Concurrently calling
      join() on the same thread object from multiple threads constitutes a data
      race that results in undefined behavior.

      After this function returns joinable() is false.

      Calling join() in a default-constructed thread (joinable() == false) is
      undefined behavior and may also throw a runtime error.
  */
  void
  join();

  /** Separates the thread of execution from the thread object, allowing
      execution to continue independently. Any allocated resources will be freed
      once the thread exits.

      After calling detach *this no longer owns any thread.
  */
  void
  detach();

  /** Exchanges the underlying handles of two thread objects. */
  void
  swap(thread& other) noexcept;

  /** Returns the number of concurrent threads supported by the implementation.
      The value should be considered only a hint. Same value as
      std::thread::hardware_concurrency().
  */
  static unsigned int
  hardware_concurrency();

  /** Modify scheduling policy and priority of the already running thread.
      Return true if this is successful. Under Linux fails if the user is not a
      member of the realtime group (try `std::strerror(errno)`).

      Note that this implementation depends on @ref native_handle() and POSIX
      threads. The POSIX realtime policies do not require a realtime scheduler.
  */
  bool
  try_scheduling_policy(int policy, int priority) noexcept;

  /** Like try_scheduling_policy() but call std::terminate() if it fails. To test if a
      realtime scheduler is available use:

         VERIFY(base::have_realtime_kernel())
  */
  void
  change_scheduling_policy(int policy, int priority);

  std::string last_error() const noexcept { return error_; }

private:
  std::thread impl_;
  std::string error_;
};

inline
thread::thread() noexcept {}

inline
thread::thread(thread&& other) noexcept
  : impl_ {std::move(other.impl_)} {}

template<class Function, class... Args>
thread::thread(Function&& f, Args&&... args)
  : impl_ {std::forward<Function>(f), std::forward<Args>(args)...} { }

template<class Function, class... Args>
thread::thread(int policy, int priority, Function&& f, Args&&... args)
  : thread{std::forward<Function>(f), std::forward<Args>(args)...} { change_scheduling_policy(policy, priority); }

inline
thread::~thread() {}

inline
thread& thread::operator = (thread&& other) noexcept {
  impl_ = std::move(other.impl_);
  return *this;
}

inline
bool
thread::joinable() const noexcept {
  return impl_.joinable();
}

inline
thread::id
thread::get_id() const noexcept {
  return impl_.get_id();
}

inline
thread::native_handle_type
thread::native_handle() {
  return impl_.native_handle();
}

inline
void
thread::join() {
  return impl_.join();
}

inline
void
thread::detach() {
  return impl_.detach();
}

inline
void
thread::swap(thread& other) noexcept {
  return impl_.swap(other.impl_);
}


bool
thread::try_scheduling_policy(int new_policy, int priority) noexcept {
  auto handle = impl_.native_handle();
  struct ::sched_param param;
  int policy;
  ::pthread_getschedparam(handle, &policy, &param);
  param.sched_priority = priority;
  if (::pthread_setschedparam(handle, new_policy, &param)) {
    if (errno) {
#if 0
      char buffer[512];
      std::sprintf(buffer, "%d=%s", errno, std::strerror(errno));
      error_ = buffer;
#endif
      error_ = std::strerror(errno);
    } else {
      error_ = "pthread_setschedparam() failed and errno=0";
    }
    return false;
  } else {
    return true;
  }
}

void
thread::change_scheduling_policy(int policy, int priority) {
  if (try_scheduling_policy(policy, priority)) {
    return;
  } else {
    /* terminate called without an active exception */
    std::terminate();
  }
}

inline
unsigned int
thread::hardware_concurrency() {
  return std::thread::hardware_concurrency();
}
} // preempt
