/* -*-coding:raw-text-unix-*-
 *
 * preempt/pthread.h -- POSIX threads
 */
#pragma once

#include <base/details/system.h>
#include <base/string.h>

#include <memory>
#include <cstring>              // std::strerror
#include <iostream>

namespace preempt {
namespace pthread {
typedef void*(*function)(void*);

struct context {
  pthread_t id {};
  std::shared_ptr<::pthread_attr_t> attrp {};
  std::shared_ptr<::sched_param> schp {};
  bool good = true;
  std::string error;
  explicit operator bool() const noexcept { return good; }
};

inline
context
create(function funp, void* argp = nullptr, int policy = SCHED_OTHER, int priority = 0)
{
  context result;
  result.attrp.reset(new pthread_attr_t);
  pthread_attr_init(result.attrp.get());
  pthread_attr_setdetachstate(result.attrp.get(), PTHREAD_CREATE_JOINABLE);
  pthread_attr_setinheritsched(result.attrp.get(), PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(result.attrp.get(), policy);
  result.schp.reset(new sched_param);
  result.schp->sched_priority = priority;
  pthread_attr_setschedparam(result.attrp.get(), result.schp.get());
  if (int error = pthread_create(&result.id, result.attrp.get(), funp, argp)) {
    result.good = false;
    result.error = base::sprintf("pthread_create() failed: '%s'", std::strerror(error));
  }
  return result;
}

inline
context
join(context thread) {
  context result {thread};
  if (int error = pthread_join(thread.id, 0)) {
    result.good = false;
    result.error = base::sprintf("pthread_join() failed: '%s'", std::strerror(error));
  } else {
    result.good = true;
    result.error.clear();
  }
  return result;
}
} // pthread
} // preempt

