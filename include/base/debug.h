/* -*-coding:raw-text-unix-*-
 *
 * base/debug.h -- debugger, breakpoints and VERIFY()
 */
#pragma once

#include <base/details/cc.h>    // NORETURN

#include <atomic>
#include <optional>
#include <exception>            // std::terminate()

#include <cstdarg>              // va_list
#include <cstdlib>              // getenv
#include <cstdio>               // perror, fprintf
#include <cstring>              // strerror
#include <csignal>              // raise
#include <cassert>
#include <errno.h>

namespace base {
/**
 * @example:
 *     $ RUNNING_UNDER_DEBUGGER=1 gdb <executable>
 *     (gdb) run args1 args2 args3
 *
 * @example:
 *     $ RUNNING_UNDER_DEBUGGER=1 gdb --args <executable> arg1 arg2 arg3
 *
 * @return True if the value of the environment variable RUNNING_UNDER_DEBUGGER
 *         is defined and not empty.
 *
 * @ref breakpoint
 */
bool running_under_debugger();

/**
 * Emit a software breakpoint.
 *
 * @param unconditional: If false emit only if @ref running_under_debugger.
 */
void breakpoint(bool unconditional = true);

/**
 * 1. Flushes streams.
 * 2. Calls @ref base::breakpoint() unless NDEBUG is defined.
 * 3. Calls std::quick_exit(EXIT_FAILURE)
 *
 * std::quick_exit() will not destruct global/static objects including
 * thread objects. This is bad because the thread dtor will call
 * std::terminate() if the thread is unjoined.
 *
 * Solution: in multi-threaded programs use base::terminate() to leave
 * ctors/dtors unconditionally and base::quick_exit() otherwise.
 */
void quick_exit(char const* descr) NORETURN;
void quick_exit(char const* descr, char const* file, unsigned line) NORETURN;

/**
 * 1. Flushes streams.
 * 2. Calls @ref base::breakpoint() unless NDEBUG is defined.
 * 3. Calls std::terminate()
 *
 * std::terminate() is the preferred way to exit ctors and dtors in
 * multi-threaded programs. Rationale: the std::thread dtor calls
 * std::terminate() too. See also @ref base::quick_exit().
 *
 * Solution: in multi-threaded programs use base::terminate() to leave
 * ctors/dtors unconditionally and base::quick_exit() otherwise.
 */
void terminate(char const* descr) NORETURN;
void terminate(char const* descr, char const* file, unsigned line) NORETURN;

/**
 * Like assert() but also if NDEBUG is defined.
 *
 * @example:
 *   VERIFY(expr);
 *
 * @example:
 *   if (VERIFY(expr))
 *     // expr is true
 *   else
 *     // expr is false, a message was printed to stderr
 */
#define VERIFY(expr)                                                    \
  ((!!(expr)) || (base::verify_error(#expr, __FILE__, __LINE__), false))

/**
 * @return True if all @ref VERIFY() macros evaluated true so far.
 */
#if HAVE_CXX_17
bool verify_flag(std::optional<bool> set = std::nullopt);
#endif // HAVE_CXX_17

/***********************************************************************
 * inlined implementation
 */
inline
void
quick_exit(char const* descr) {
  if (descr) {
    if (errno)
      std::perror(descr);
    else
      std::fprintf(stderr, "%s\n", descr);
  } else if (errno) {
    std::perror("");
  }
  std::fflush(stdout);
  std::fflush(stderr);
#if NDEBUG
#else
  breakpoint();
#endif
  std::quick_exit(EXIT_FAILURE);
}

inline
void
quick_exit(char const* descr, char const* file, unsigned line) {
  std::fprintf(stderr, "FAIL: %s(%u): ", file, line);
  quick_exit(descr);
}

inline
void
terminate(char const* descr) {
  if (descr) {
    if (errno)
      std::perror(descr);
    else
      std::fprintf(stderr, "%s\n", descr);
  } else if (errno) {
    std::perror("");
  }
  std::terminate();
}

inline
void
terminate(char const* descr, char const* file, unsigned line) {
  std::fprintf(stderr, "FAIL: %s(%u): ", file, line);
  terminate(descr);
}

inline
bool
running_under_debugger() {
  char const* const env = std::getenv("RUNNING_UNDER_DEBUGGER");
  bool yes = env && *env;
  return yes;
}

inline
void
breakpoint(bool unconditional) {
  if (running_under_debugger() || unconditional) {
#if defined(__linux__)
    asm("int $3");            // PC platform: software breakpoint
#else
    std::raise(SIGTRAP);
#endif
  }
}

inline
void
verify_error(char const* msg, char const* file, unsigned line) {
  std::fprintf(stderr, "Verification failed! %s at %s(%u)\n", msg, file, line);
#if HAVE_CXX_17
  verify_flag(false);
#endif // HAVE_CXX_17
}

#if HAVE_CXX_17
inline
bool
verify_flag(std::optional<bool> set) {
  static std::atomic_bool result {true};
  if (set) result = *set;
  return result;
}
#endif // HAVE_CXX_17
} /* base */
