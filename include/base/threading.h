/* -*-coding:raw-text-unix-*-
 *
 * base/thread.h -- POSIX threads
 */
#pragma once

#include <base/posix.h>
#include <base/string.h>
#include <base/verify.h>

#include <thread>
#include <memory>
#include <cstring>              // std::strerror

namespace base {
/**
 * Lightweight, trivially copyable class that serves as a unique identifier of
 * schedulabe objects. Like std::thread is can be used as key in associative
 * containers, both ordered and unordered.
 *
 * Instances of this class may also hold the special distinct value that does
 * not represent any thread.
 */
struct thread {
  using handle = pthread_t;

  typedef void*(*function)(void*);

  thread();
  thread(function funp, void* argp = nullptr);
  thread(int policy, int priority, function funp, void* argp = nullptr);

  void join();

  explicit operator bool() const noexcept { return joinable; }

  std::shared_ptr<::pthread_attr_t> attrp {};
  std::shared_ptr<::sched_param> schp {};
  bool joinable = false;
  std::string last_error;
  handle id {};
};

/**
 * Try to modify scheduling policy and priority of a running std::thread. Turns
 * the std::thread in to a realtime thread.
 *
 * Return true if this is successful, false otherwise (failure). Probably fails
 * if the thread has already terminated. Under Linux returns if the user is not
 * a member of the realtime group.
 *
 * If this function returns false last_error() will provide a description.
 */
bool try_scheduling(std::thread&, int policy, int priority std::string* = nullptr) noexcept;

/**
 * Really change the scheduling policy and priority of a running std::thread or
 * call @ref base::quick_exit().
 *
 * The main reason for an error is insufficient user authorizations for creating
 * threads.
 *
 * To test if a realtime scheduler is available use @ref
 * base::have_realtime_kernel().
 */
void change_scheduling(std::thread&, int policy, int priority, std::string* = nullptr) noexcept;

/**
 * ETIMEDOUT
 *
 * Thrown in case a @ref mutex::lock() or @ref critical_section::lock() times
 * out.
 */
struct timeout_error : std::runtime_error {
  timeout_error(const std::string& what)
    : std::runtime_error(what) { }
};

/**
 * EDEADLK
 *
 * Thrown in case a pthread_xxx() or sem_xxx() function returns EDEADLK. This
 * usually means that the calling thread already holds the lock.
 */
struct deadlock_error : std::runtime_error {
  deadlock_error(const std::string& what)
    : std::runtime_error(what) { }
};

/**
 * EINTR
 *
 * Thrown in case a pthread_xxx() or sem_xxx() function returns EINTR. This
 * means a signal had interrupted the function, like when CTRL+C was pressed.
 *
 * When this error is catched check anything signal handlers could have been
 * altered and then simply restart the blocking function.
 */
struct signal_interrupt_error : std::runtime_error {
  signal_interrupt_error(const std::string& what)
    : std::runtime_error(what) { }
};

/******************************************************************************
 * inline implementation
 */
inline
thread::thread() { }

inline
thread::thread(function funp, void* argp)
  : thread {SCHED_OTHER, 0, funp, argp} { }

inline
thread::thread(int policy, int priority, function funp, void* argp) {
  if (funp) {
    /* initialize structures */
    attrp.reset(new ::pthread_attr_t);
    schp.reset(new ::sched_param);
    if (int errnum = pthread_attr_init(attrp.get())) {
      last_error = base::sprintf("pthread_attr_init() failed: '%s'", std::strerror(errnum));
      return;
    }
    schp->sched_priority = priority;

    switch (policy) {
    case SCHED_FIFO:
    case SCHED_RR:
      /* can be used only with static priorities higher than 0 */
      VERIFY(priority > 0);
      break;
    }

    if (int errnum = pthread_attr_setdetachstate(attrp.get(), PTHREAD_CREATE_JOINABLE)) {
      last_error = base::sprintf("pthread_attr_setdetachstate(PTHREAD_CREATE_JOINABLE) failed: '%s'", std::strerror(errnum));
      return;
    }
    if (int errnum = pthread_attr_setinheritsched(attrp.get(), PTHREAD_EXPLICIT_SCHED)) {
      last_error = base::sprintf("pthread_attr_setinheritsched(PTHREAD_EXPLICIT_SCHED) failed: '%s'", std::strerror(errnum));
      return;
    }
    if (int errnum = pthread_attr_setschedpolicy(attrp.get(), policy)) {
      last_error = base::sprintf("FAILED: pthread_attr_setschedpolicy(%d): '%s'", policy, std::strerror(errnum));
      return;
    }
    if (int errnum = pthread_attr_setschedparam(attrp.get(), schp.get())) {
      last_error = base::sprintf("FAILED: pthread_attr_setschedparam(%d): '%s'", schp->sched_priority, std::strerror(errnum));
      return;
    }

    /* create thread */
    if (int errnum = pthread_create(&id, attrp.get(), funp, argp)) {
      last_error = base::sprintf("thread() ctor failed: '%s'", std::strerror(errnum));
      return;
    }
    joinable = true;
  }
}

inline
void
thread::join() {
  if (int errnum = pthread_join(id, 0)) {
    last_error = base::sprintf("thread::join() failed: '%s'", std::strerror(errnum));
  } else {
    joinable = false;
    last_error.clear();
  }
}

inline
bool
try_scheduling(std::thread& th, int new_policy, int new_priority, std::string* errorp) noexcept {
  switch (new_policy) {
  case SCHED_FIFO:
  case SCHED_RR:
    VERIFY(new_priority > 0);
    return false;
  }
  if (false == th.joinable())
    return true;
  /* get current policy and priority */
  sched_param sch;
  int policy;
  pthread_getschedparam(th.native_handle(), &policy, &sch);
  /* set new policy and priority */
  sch.sched_priority = new_priority;
  if (int errnum = pthread_setschedparam(th.native_handle(), new_policy, &sch)) {
    switch (errnum) {
    case ESRCH:
      /* The thread is not available ("No such process"). This means it has
         already exited. Note that std::thread::joinable() returns true for
         exited threads. */
      return true;
    case EINVAL:
    case EPERM:
    default:
      if (errorp)
        *errorp = base::sprintf("FAILED: pthread_setschedparam(): '%s'", std::strerror(errnum));
      return false;
    }
  }
  return true;
}

inline
void
change_scheduling(std::thread& th, int new_policy, int new_priority, std::string* errorp) noexcept {
  std::string error;
  if (errorp == nullptr)
    errorp = &error;
  if (false == try_scheduling(th, new_policy, new_priority, errorp)) {
    base::quick_exit(error.c_str());
  }
}
} // preempt
