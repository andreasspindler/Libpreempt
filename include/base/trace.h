/* -*-coding:raw-text-unix-*-
 *
 * base/logging.h -- breakpoints, debugging and tracing
 */
#pragma once

#include <chrono>
#include <ctime>
#include <iostream>

#include <base/utility.h>

namespace base {
/**
 * Print to stderr with timestamp prefix.
 */
inline
void
trace(char const* fmt, ...) {
  static std::mutex lock;
  BASE_STD_GUARD(lock);
  std::va_list val;
  va_start(val, fmt);
  std::time_t now;
  std::time(&now);
  char* const s = std::ctime(&now);
  *std::strchr(s, '\n') = '\0';
  std::fprintf(stderr, "%s  ", s);
  std::vfprintf(stderr, fmt, val);
  std::fflush(stderr);
  va_end(val);
}

/**
 * @example:
 *    do {
 *        .
 *        .
 *    } while (TRACE_IF_TRUE(condition));
 */
#define TRACE_IF_TRUE(expr) \
(void) ((!!(expr)) && (base::trace("Expression is true: ", #expr, __FILE__, __LINE__), 1))
#define TRACE_IF_FALSE(expr) \
(void) ((!!(expr)) || (base::trace("Expression is false: ", #expr, __FILE__, __LINE__), 0))

/**
 * Stream to stderr with timestamp and mutex-protection.
 */
#define BASE_LOG_COUT(streamargs) BASE_LOG_IMPL(std::cout, (streamargs))

/**
 * Stream to stderr with timestamp and mutex-protection.
 */
#define BASE_LOG_CERR(streamargs) BASE_LOG_IMPL(std::cerr, (streamargs))

#define BASE_LOG_IMPL(stream, streamargs)                               \
do {                                                                    \
  auto const t = std::chrono::system_clock::now();                      \
  auto const ttm = std::chrono::system_clock::to_time_t(t);             \
  extern std::mutex g_logging_mutex;                                    \
  std::lock_guard<std::mutex> lock(g_logging_mutex);                    \
  std::cerr << "[ "                                                     \
            << std::put_time(std::localtime(&ttm), "%y/%m/%d %H:%M:%S") \
            << " ] "                                                    \
            << std::setw(9) << std::this_thread::get_id() << " "        \
            << __FILE__ << ":" << __LINE__ << "   " << streamargs       \
            << std::endl;                                               \
 } while ((0))
} /* base */
