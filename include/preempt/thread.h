/* -*-coding:raw-text-unix-*-
 *
 * preempt/thread.h -- like std::thread but with optional realtime priority and
 *                     POSIX scheduling policies
 */
#pragma once

#include <base/posix.h>
#include <base/string.h>
#include <base/verify.h>
#include <base/threading.h>

#include <cstring>              // std::strerror
#include <cassert>

namespace preempt {
/**
 * @brief Like std::thread but with optional realtime priority and POSIX
 * scheduling policies
 *
 * Example:
 *
 *     preempt::thread thr;
 *     thr = preempt::thread(function)                 // no priority
 *     thr = preempt::thread(SCHED_FIFO, 10, function) // priority 10
 *
 * @see https://en.cppreference.com/w/cpp/thread/thread
 */
class thread
{
public:
  using native_handle_type = std::thread::native_handle_type;
  using id = std::thread::id;

  /** The default constructor creates a new thread object which does not
      represent a thread. A thread is default-constructed if this->get_id() ==
      thread::id(). */
  thread() noexcept;

  /** Move constructor. Constructs the thread object to represent the thread of
      execution that was represented by other. After this call other no longer
      represents a thread of execution.
  */
  thread(thread&& other) noexcept;
  thread(std::thread&& other) noexcept;

  /** Construct new, normal thread object. Under POSIX this means a thread
      running under the SCHED_OTHER scheduling policy and priority 0. To set a
      new scheduling policy and priority @ref try_scheduling().
  */
  template<class Function, class... Args>
  explicit thread(Function&& f, Args&&... args);

  /** Start a realtime thread with a certain scheduling policy and priority.
      Before the ctor returns the thread may not only begin execution, but also
      can preempt any other threads with a lower priority (@ref
      change_scheduling).

      If setting the policy/priority fails the process is exited with
      EXIT_FAILURE.
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

  /** Checks if this object identifies an active thread of execution. In general
      a thread is not joinable if:
       - it was default-constructed
       - it has been moved
       - detach() has been called

      A thread that has finished executing code, but has not yet been joined is
      still considered an active thread of execution and is therefore joinable.
  */
  bool joinable() const noexcept;

  /** Returns a value identifying the thread associated with *this. */
  id get_id() const noexcept;

  /** Returns the implementation defined underlying thread handle. */
  native_handle_type native_handle();

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
  void join();

  /** Separates the thread of execution from the thread object, allowing
      execution to continue independently. Any allocated resources will be freed
      once the thread exits. After calling detach *this no longer owns any
      thread. */
  void detach();

  /** Exchanges the underlying handles of two thread objects. */
  void swap(thread& other) noexcept;

  /** Returns the number of concurrent threads supported by the underlying
      implementation. The value should be considered only a hint.
  */
  static unsigned int hardware_concurrency();

  /** Modify scheduling policy and priority of the already running thread.
      Return true if this is successful, false otherwise (failure). Probably
      fails if the thread has already terminated. Under Linux fails if the user
      is not a member of the realtime group.

      If this function returns false last_error() will provide a description.
  */
  bool try_scheduling(int policy, int priority) noexcept;

  /** Like try_scheduling() but call @ref base::quick_exit if it fails. The main
      reason for an error is insufficient user authorizations for creating
      realtime threads.

      To test if a realtime scheduler is available use:

         assert(base::have_realtime_kernel())
  */
  void change_scheduling(int policy, int priority) noexcept;

  std::string last_error() const noexcept { return error_; }

private:
  std::thread impl_;
  std::string error_;
};

#if 0
template <int Policy, int Priority>
class static_thread : public thread ..
#endif // 0

/***********************************************************************
 * inlined implementation
 */
inline
thread::thread() noexcept {}

inline
thread::thread(thread&& other) noexcept
  : impl_ {std::move(other.impl_)} {}

inline
thread::thread(std::thread&& other) noexcept
  : impl_ {std::move(other)} {}

template<class Function, class... Args>
thread::thread(Function&& f, Args&&... args)
  : impl_ {std::forward<Function>(f), std::forward<Args>(args)...} { }

template<class Function, class... Args>
thread::thread(int policy, int priority, Function&& f, Args&&... args)
  : thread{std::forward<Function>(f), std::forward<Args>(args)...} {
  change_scheduling(policy, priority); // may not return
}

inline
thread::~thread() {
#if NDEBUG
#else
  auto const thread_still_active = joinable();
  VERIFY(thread_still_active == false);
#endif
}

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

inline
bool
thread::try_scheduling(int policy, int priority) noexcept {
  return base::try_scheduling(impl_, policy, priority, &error_);
}

inline
void
thread::change_scheduling(int policy, int priority) noexcept {
  base::change_scheduling(impl_, policy, priority, &error_);
}

inline
unsigned int
thread::hardware_concurrency() {
  return std::thread::hardware_concurrency();
}
} // preempt
