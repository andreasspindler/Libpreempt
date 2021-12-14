/* -*-coding:raw-text-unix-*-
 *
 * base/pthread.h -- lightweight POSIX thread class
 */
#pragma once

#include <base/posix.h>
#include <base/string.h>
#include <base/debug.h>

#include <memory>
#include <cstring>              // std::strerror
#include <iostream>

namespace base {
/**
 * Lightweight, trivially copyable class that serves as a unique identifier of
 * schedulabe objects. Like std::thread is can be used as key in associative
 * containers, both ordered and unordered.
 *
 * Instances of this class may also hold the special distinct value that does
 * not represent any thread.
 */
struct pthread {
  using handle = pthread_t;

  typedef void*(*function)(void*);

  pthread();
  pthread(function funp, void* argp = nullptr);
  pthread(int policy, int priority, function funp, void* argp = nullptr);

  void join();

  explicit operator bool() const noexcept { return joinable; }

  std::shared_ptr<::pthread_attr_t> attrp {};
  std::shared_ptr<::sched_param> schp {};
  bool joinable = false;
  std::string last_error;
  handle id {};
};

inline
pthread::pthread() { }

inline
pthread::pthread(function funp, void* argp)
  : pthread {SCHED_OTHER, 0, funp, argp} { }

inline
pthread::pthread(int policy, int priority, function funp, void* argp)
{
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
      last_error = base::sprintf("pthread() ctor failed: '%s'", std::strerror(errnum));
      return;
    }
    joinable = true;
  }
}

inline
void
pthread::join() {
  if (int errnum = pthread_join(id, 0)) {
    last_error = base::sprintf("pthread::join() failed: '%s'", std::strerror(errnum));
  } else {
    joinable = false;
    last_error.clear();
  }
}
} // preempt
