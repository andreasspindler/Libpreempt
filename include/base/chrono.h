// -*-coding: raw-text-unix;-*-
#pragma once

#include <chrono>

#include <base/numeric.h>

namespace base {
/**
 * Convert integer to nanoseconds and back
 *
 * double represents passed time in form of fractions of seconds or
 * microseconds.
 *
 * nsec_t is an integral type since there are no fractions of nanoseconds.
 */
typedef int64_t nsec_t;

nsec_t sec_to_nsec( double);
double sec_to_usec( double);
double sec_to_msec( double);

double msec_to_sec( double);
double msec_to_usec(double);
nsec_t msec_to_nsec(double);

double usec_to_sec( double);
double usec_to_msec(double);
nsec_t usec_to_nsec(double);

double nsec_to_sec( nsec_t);
double nsec_to_msec(nsec_t);
double nsec_to_usec(nsec_t);

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

/**
 * Get elapsed time since construction.
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

  /** Get time of construction or last time stop() was called. */
  time_point start() const;

  /** Get elapsed seconds so far. */
  long seconds() const;

  /** Get elapsed milliseconds so far. */
  long milliseconds() const;

  /** Get elapsed time in microseconds so far. */
  long microseconds() const;

  /** Get elapsed time in nanoseconds so far. */
  nsec_t nanoseconds() const;

  /** Get elapsed time in microseconds, then reset to current time. */
  long stop();

private:
  template <typename T>
  long cast() const;

  time_point t0_;
};

/**
 * Nanoseconds benchmark as wrapper arround @ref base::stopwatch.
 *
 * Example:
 *     base::benchmark bm;
 *     {
 *       bm.reset(); // start
 *           .
 *           .
 *       bm.stop();  // accumulate
 *           .
 *           .
 *       bm.stop();  // accumulate
 *           .
 *           .
 *       bm.reset(); // start new (no accumulation)
 *           .
 *           .
 *       bm.stop();  // accumulate
 *           .
 *           .
 *       auto intermediate_sum = bm.count();
 *           .
 *           .
 *       bm.stop();  // end
 *       auto total_sum = bm.count();
 *     }
 *
 */
class benchmark
{
public:
  /** Set new timepoint. */
  void reset();

  /** Accumulate nanoseconds since last call to reset() or stop(). */
  void stop();

  /** Get accumulated nanoseconds to far. */
  nsec_t count() const;

private:
  stopwatch t0_;
  nsec_t sum_;
};

/***********************************************************************
 * inlined implementation
 */
inline nsec_t sec_to_nsec( double n) { return round_to_nearest<nsec_t>(n * 1e9); }
inline double sec_to_msec( double n) { return n * 1e3; }
inline double sec_to_usec( double n) { return n * 1e6; }

inline nsec_t msec_to_nsec(double n) { return round_to_nearest<nsec_t>(n * 1e6); }
inline double msec_to_sec( double n) { return n / 1e3; }
inline double msec_to_usec(double n) { return n * 1e3; }

inline nsec_t usec_to_nsec(double n) { return round_to_nearest<nsec_t>(n * 1e3); }
inline double usec_to_sec( double n) { return n / 1e6; }
inline double usec_to_msec(double n) { return n / 1e3; }

inline double nsec_to_sec( nsec_t n) { return n / 1e9; }
inline double nsec_to_msec(nsec_t n) { return n / 1e6; }
inline double nsec_to_usec(nsec_t n) { return n / 1e3; }

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
nsec_t
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

inline
timeout::operator bool() const {
  return reached();
}

inline
nsec_t
timespec_to_nsec(timespec ts) {
  return round_to_nearest<nsec_t>(ts.tv_sec * 1e9) + ts.tv_nsec;
}

inline
timespec
nsec_to_timespec(nsec_t ns) {
  timespec ts {
    time_t(ns / nsec_t(1e9)),
    time_t(ns % nsec_t(1e9))
  };
  return ts;
}

inline
void
benchmark::reset() {
  t0_ = stopwatch {};
}

inline
void
benchmark::stop() {
  sum_ += t0_.nanoseconds();
  reset();
}

inline
nsec_t
benchmark::count() const {
  return sum_;
}
} /* base */
