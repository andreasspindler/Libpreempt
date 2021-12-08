// -*-coding: raw-text-unix;-*-
#pragma once

#include <chrono>

namespace base {
/**
 * Get elapsed time.
 *
 * Example:
 *
 *   void f() {
 *     base::stopwatch sw;
 *      .
 *      .
 *     auto usec1 = sw.elapsed();
 *      .
 *      .
 *     auto usec2 = sw.elapsed();
 *      .
 *      .
 *     auto usec3 = sw.stop(); // get elapsed + reset
 *      .
 *      .
 */
class stopwatch {
public:
  using clock_type = std::chrono::high_resolution_clock;
  using time_point = std::chrono::time_point<clock_type>;

  /** Set time. */
  stopwatch();

  /** Get time of construction. */
  time_point start() const;

  /** Get elapsed seconds so far. */
  long seconds() const;

  /** Get elapsed milliseconds so far. */
  long milliseconds() const;

  /** Get elapsed time in microseconds so far. */
  long microseconds() const;

  /** Get elapsed time in nanoseconds so far. */
  long nanoseconds() const;

  /** Get elapsed time in microseconds, then reset to current time. */
  long stop();

private:
  template <typename T>
  long cast() const;

  time_point t0_;
};

/**
 * Test if a time condition was met.
 *
 * Example:
 *
 *   void f() {
 *     base::timeout timeout {300};
 *      .
 *      .
 *     if (timeout) {
 *       // timeout reached
 *     }
 *
 */
class timeout {
public:
  /** Define timeout from now plus us microseconds. */
  timeout(long us = 0);

  bool reached() const;

  operator bool() const;

private:
  std::chrono::system_clock::time_point const t1_;
};

/***********************************************************************
 * inlined implementation
 */
inline
stopwatch::stopwatch() {
  stop();
}

inline
stopwatch::time_point
stopwatch::start() const {
  return t0_;
}

inline
long
stopwatch::stop() {
  long result = microseconds();
  t0_ = clock_type::now();
  return result;
}

template <typename T>
long
stopwatch::cast() const {
  return std::chrono::duration_cast<T>(clock_type::now() - t0_).count();
}

inline
long
stopwatch::seconds() const {
  return cast<std::chrono::seconds>();
}

inline
long
stopwatch::milliseconds() const {
  return cast<std::chrono::milliseconds>();
}

inline
long
stopwatch::microseconds() const {
  return cast<std::chrono::microseconds>();
}

inline
long
stopwatch::nanoseconds() const {
  return cast<std::chrono::nanoseconds>();
}

inline
timeout::timeout(long us)
  : t1_ {std::chrono::system_clock::now() + std::chrono::microseconds {us}} { }

inline
bool
timeout::reached() const {
  return std::chrono::system_clock::now() >= t1_;
}

timeout::operator bool() const {
  return reached();
}
} /* base */
