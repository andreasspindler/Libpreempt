/* -*-coding:raw-text-unix-*-
 *
 * base/logging.h -- breakpoints, debugging and tracing
 */
#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iostream>

namespace base {
/**
 * Print to stderr with timestamp prefix.
 */
inline
void
trace(char const* fmt, ...) {
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
 * Print to stdout if BASE_LOGGING is defined.
 */
#define LOG_COUT(streamargs) LOG_IMPL(std::cout, (streamargs))

/**
 * Print to stderr if BASE_LOGGING is defined.
 */
#define LOG_CERR(streamargs) LOG_IMPL(std::cerr, (streamargs))

#ifdef BASE_LOGGING
#define LOG_IMPL(stream, streamargs)                                    \
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
#else
#define LOG_IMPL(stream, streamargs) (void) 0
#endif // BASE_LOGGING
} /* base */
