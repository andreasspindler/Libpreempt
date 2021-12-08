/* -*-coding:raw-text-unix-*-
 *
 * base/error.h -- maps errno values to exceptions
 */
#pragma once

#include <stdexcept>

#include <cerrno>

namespace preempt {
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
} /* preempt */
