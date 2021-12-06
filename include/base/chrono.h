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
 */
class stopwatch {
public:
  /** Set time. */
  stopwatch();

  /** Get elapsed seconds so far. */
  long seconds() const;

  /** Get elapsed milliseconds so far. */
  long milliseconds() const;

  /** Get elapsed time so far in microseconds. */
  long microseconds() const;

  /** Get elapsed time in microseconds. */
  double stop() const;

private:
  template <typename T>
  long cast() const;

  using clock_type = std::chrono::high_resolution_clock;

  std::chrono::time_point<clock_type> const t0_;
};

/**
 * Test if a time condition was met.
 *
 * Example:
 *
 *   void f() {
 *     base::timeout t {300};
 *      .
 *      .
 *     if (t.reached()) {
 *       // timeout reached
 *     }
 *
 */
class timeout
{
public:
  timeout(long ms = 0);

  bool reached() const;
  operator bool() const;

private:
  std::chrono::system_clock::time_point const t1_;
};

/***********************************************************************
 * inlined implementation
 */
inline
stopwatch::stopwatch()
  : t0_ {clock_type::now()} { }

inline
double
stopwatch::stop() const {
  std::chrono::duration<double, std::micro> const us = clock_type::now() - t0_;
  return us.count();
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
timeout::timeout(long ms)
  : t1_ {std::chrono::system_clock::now() +
         std::chrono::milliseconds {ms}} { }

inline
bool
timeout::reached() const {
  return std::chrono::system_clock::now() >= t1_;
}

inline
timeout::operator bool() const {
  return reached();
}
} /* base */
