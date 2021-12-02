/* -*-coding:raw-text-unix-*-
 *
 * base/numeric.h -- round, clamp, epsilon
 */
#pragma once

#include <cmath>                // std::fabs
#include <limits>
#include <functional>           // std::less
#include <cmath>                // std::floor
#include <cstdint>              // int64_t (or inttypes.h)
#include <cassert>

namespace base {
/**
 * @brief Round to to nearest whole integer number.
 *
 * @code
 *     round_to_nearest<long>(   1.5); //  2
 *     round_to_nearest<long>(   1.2); //  1
 *     round_to_nearest<double>( 1.2); //  1.0
 *     round_to_nearest<double>(-2.7); // -3.0
 *     round_to_nearest<double>(-2.5); // -2.0
 *     round_to_nearest<long>(  -2.7); // -3
 *     round_to_nearest<long>( -0.51); // -1
 *     round_to_nearest<long>(  -0.5); //  0
 *     round_to_nearest<long>(   0.0); //  0
 *     round_to_nearest<long>(   0.5); //  1
 * @endcode
 *
 * @param T: Floating-point or integer type
 */
template <typename ReturnT>
inline
ReturnT
round_to_nearest(double const n) {
  return static_cast<ReturnT>(std::floor(n + .5));
}

inline
int
round_to_int(double const n) {
  return round_to_nearest<int>(n);
}

inline
unsigned int
round_to_uint(double const n) {
  return round_to_nearest<unsigned int>(n);
}

inline
long
round_to_long(double const n) {
  return round_to_nearest<long>(n);
}

inline
unsigned long
round_to_ulong(double const n) {
  return round_to_nearest<unsigned long>(n);
}

inline
int64_t
round_to_int64(double const n) {
  return round_to_nearest<int64_t>(n);
}

inline
uint64_t
round_to_uint64(double const n) {
  return round_to_nearest<uint64_t>(n);
}

/**
 * Test for equality taking into account the machine epsilon to
 * compensate rounding errors.
 *
 * @result True if equal.
 */
inline
bool
almost_equal(double a, double b) {
  auto const e = std::numeric_limits<double>::epsilon();
  auto const x = std::fabs(a - b);
  if (x <= e)
    return true;
  /* adapt to the bigger number (adaptive epsilon) */
  return x <= e * std::fmax(std::fabs(a), std::fabs(b));
}

/**
 * Test for equality to zero taking into account the machine epsilon
 * to compensate rounding errors.
 *
 * @result True if equal to zero.
 */
inline
bool
almost_zero(double a) {
  return almost_equal(a, 0.0);
}

/**
 * Less than.
 */
inline
bool
almost_lt(double a, double b) {
  return a < b &&!almost_equal(a, b);
}

/**
 * Less than or equal
 */
inline
bool
almost_le(double a, double b) {
  return a < b || almost_equal(a, b);
}

/**
 * Greater than.
 */
inline
bool
almost_gt(double a, double b) {
  return a > b &&!almost_equal(a, b);
}

/**
 * Greater than or equal.
 */
inline
bool
almost_ge(double a, double b) {
  return a > b || almost_equal(a, b);
}

/**
 * Clamp value into boundaries.
 *
 * @param v: the value to clamp
 *
 * @param hi, lo: the boundaries to clamp v to
 *
 * @param less: Returns true if the first argument is less than the
 *              second, false otherwise.
 *
 * @result Reference to lo if v is less than lo, reference to hi if hi
 *         is less than v, otherwise reference to v.
 *
 * @see std::clamp (C++17).
 */
template <class T, class Less>
constexpr T const&
clamp(T const& v, T const& lo, T const& hi, Less less = std::less<T>{}) {
  assert(!less(hi, lo));
  return less(v, lo) ? lo : less(hi, v) ? hi : v;
}

/**
 * Numeric interval.
 *
 * For integers and floating point.
 */
// TBD: explicit for float and double taking into account
//      std::numeric_limits::epsilon
template <typename T>
class interval {
  T const lower_, upper_;
public:
  interval(T low = {}, T up = {})
    : lower_ {low}, upper_ {up} { }

  /**
   * Test if value is in lower, upper bounds.
   */
  bool contains(T value) const {
    //assert(lower_ <= upper_);
    return value >= lower_ && value <= upper_;
  }

  /**
   * Clamp value into interval.
   *
   * @see base::clamp
   */
  constexpr T const&
  clamp(T const& v) const {
    //assert(lower_ <= upper_);
    return ::base::clamp(v, lower_, upper_);
  }
};
} /* base */
