/* -*-coding:raw-text-unix-*-
 *
 * base/debug.h -- debugger, breakpoints, quick_exit() and VERIFY()
 */
#pragma once

#include <base/details/cc.h>    // NORETURN

#include <atomic>
#include <optional>
#include <exception>            // std::terminate()
#include <iostream>

#include <cstdarg>              // va_list
#include <cstdlib>              // getenv
#include <cstdio>               // perror, fprintf
#include <cstring>              // strerror
#include <csignal>              // raise
#include <cassert>
#include <errno.h>

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
void global_verify_flag(bool);

/**
 * @return True if all @ref VERIFY() macros evaluated true so far.
 */
bool global_verify_flag();

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
void breakpoint(bool unconditional = false);

/**
 * 1. Flushes streams.
 * 2. Calls @ref base::breakpoint() unless NDEBUG is defined.
 * 3. Calls std::terminate()
 *
 * Before quitting std::terminate() will destruct global/static objects
 * including thread objects in the process in order to free resources.
 *
 * std::terminate() is the preferred way to exit ctors/dtors in multi-threaded
 * programs. Note the std::thread dtor calls std::terminate() if the thread is
 * unjoined.
 *
 * @see base::quick_exit()
 */
void terminate(char const* descr) NORETURN;
void terminate(char const* descr, char const* file, unsigned line) NORETURN;

/**
 * 1. Flushes streams.
 * 2. Calls @ref base::breakpoint() unless NDEBUG is defined.
 * 3. Calls std::quick_exit(EXIT_FAILURE)
 *
 * std::quick_exit() is the preferred way to exit multi-threaded programs in
 * case of runtime errors. When a runtime errors occurs many threads will be
 * unjoined but calling std::terminate() would call dtors of global/static
 * objects anyway.
 *
 * The idea is that std::quick_exit() will not free resources because that could
 * produce subsequent errors. This means std::quick_exit() will not destruct
 * global/static objects including thread objects. The process leaves any
 * cleanup work to the kernel (and possibly the hardware).
 *
 * @see base::terminate()
 */
void quick_exit(char const* descr = nullptr) NORETURN;
void quick_exit(char const* descr, char const* file, unsigned line) NORETURN;

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
  global_verify_flag(false);
}
} /* base */
