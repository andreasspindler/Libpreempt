/*
 * std::async -- asynchronous function with timeout
 *
 * Run a std::function as asynchronous operation with timeout. Since std::async
 * may run the function in another thread we wait until it returns.
 */
// https://stackoverflow.com/questions/14203584/timeout-in-stdasync

#include <preempt/process.h>

#include <stdexcept>
#include <future>
#include <chrono>

using namespace std;

using timeout_error = runtime_error;

void work();

int main()
{
  try {
    async(work).wait();
    // it return without throwing
    return EXIT_FAILURE;
  } catch(timeout_error) {
    return EXIT_SUCCESS;
  }
}

void work()
{
  using namespace chrono;
  system_clock::time_point deadline = system_clock::now() + milliseconds(100);

  for (bool done = false; !done;) {
    // do some work (omitted here) then test timeout
    if (system_clock::now() >= deadline) {
      throw timeout_error {"work"};
    }
    // be nice
    preempt::this_process::yield();
  }
}

////////////////////

#include <preempt/posix.h>

/**
 * @brief Like std::thread but allow POSIX scheduling policies
 *
 * std::thread and preempt::thread have the same interface.
 *
 * preempt::thread has an add additional method change_policy and a constructor
 * that accepts a priority.
 *
 * TODO: write test that gets a template template parameter to run std::thread or std::realtime_thread
  *
 * @see https://en.cppreference.com/w/cpp/thread/thread
 */
//
class thread
{
public:
  using own_type = thread<SCHED_OTHER>;
  using native_handle_type = std::thread::handle;
  using id_type = std::thread::id_type;

  /** Construct new, normal thread object. Under POSIX this means a thread
      running under the OTHER scheduling policy and priority 0. */
  template<class Function, class... Args>
  explicit thread(int priority, Function&&, Args&&...);

  /** Start a thread a priority. Note that the thread may not only begin
      execution, but also will preempt any other threads with a lower priority
      BEFORE the ctor returned. */
  template<int SCHED, class Function, class... Args>
  explicit thread(int priority, Function&&, Args&&...);

  /** Free the occupied system resources. */
  ~thread() {}

  /** Move thread object. Call this->liable() and assign the state of other to
      *this and sets other to a default constructed state.

      After this call, this->get_id() is equal to the value of other.get_id()
      prior to the call, and other no longer represents a thread of
      execution. */
  thread& operator = (thread&& other) noexcept;

  /** Checks if this object identifies an active thread of execution.
      Specifically, returns false if get_id() == id(). So a default constructed
      thread is not joinable.

      A thread that has finished executing code, but has not yet been joined is
      still considered an active thread of execution and is therefore
      joinable. */
  bool joinable() const noexcept;

  /** If *this has an associated thread (joinable() == true) call
      std::terminate() is called. */
  void liable() const;

  /** Checks if this object was properly constructed, i.e. native_handle().goood
      == true. */
  bool good();

  /** Returns a value of thread::id_type identifying the thread
      associated with *this. */
  id_type get_id() const noexcept;

  /** Returns the implementation defined underlying thread handle. */
  // TODO: Uses native_handle to enable realtime scheduling of C++ threads on a
  //       POSIX system. Uses native_handle to enable realtime scheduling of C++
  //       threads on a POSIX system.
  native_handle_type native_handle();

  /** Returns an empty thread id. If x.id() == thread::get_id() then
      x was default-constructued and is not joinable. */
  static id_type const& id() {
    return preempt::pthread::null_context().id;
  }

  /** Blocks the current thread until the thread identified by *this finishes
      its execution. The completion of the thread identified by *this
      synchronizes with the corresponding successful return from join().

      No synchronization is performed on *this itself. Concurrently calling
      join() on the same thread object from multiple threads constitutes a data
      race that results in undefined behavior.

      After this function returns joinable() is false. */
  void join();

  /** Separates the thread of execution from the thread object, allowing
      execution to continue independently. Any allocated resources will be freed
      once the thread exits.

      After calling detach *this no longer owns any thread. */
  void detach();

  /** Exchanges the underlying handles of two thread objects. */
  void swap(thread& other) noexcept;

  /** Returns the number of concurrent threads supported by the implementation.
      The value should be considered only a hint. Same value as
      std::thread::hardware_concurrency(). */
  static unsigned int hardware_concurrency() noexcept;

  /** Modify scheduling policy and priority of the already running thread.
      Return true if this is successful. Under Linux fails if the user is not a
      member of the realtime group. */
  template <int SCHED>
  bool change_policy(int priority);

  /** Like change_policy() but call std::terminate() if it fails OR not an RTOS. */
  template <int SCHED>
  bool change_policy_with_prove(int priority);

private:
    std::thread thr_;
};

template <int SCHED>
bool
thread::change_policy(int priority) {
  ::sched_param sch;
  int policy = SCHED;
  ::pthread_getschedparam(thr_.native_handle(), &policy, &sch);
  sch.sched_priority = priority;
  if (::pthread_setschedparam(t1.native_handle(), SCHED_FIFO, &sch)) {
    std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
  }
}

template <int SCHED>
bool
thread::change_policy_with_prove() {
  // TBD: check if preempt patches
  change_policy<SCHED>(priority) || std::terminate();
}

static
unsigned int
thread::hardware_concurrency() noexcept {
  return std::thread::hardware_concurrcy();
}
