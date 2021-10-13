/* -*-coding:raw-text-unix-*-
 *
 * base/debug.h -- breakpoints, debugging and tracing
 */
#pragma once

#include <cstdarg>              // va_list
#include <cstdlib>              // getenv
#include <cstdio>               // perror, fprintf
#include <cstring>              // strerror
#include <ctime>
#include <csignal>              // raise
#include <cassert>

#include <base/details/cc.h>    // NORETURN
#include <errno.h>

namespace base {
  /**
   * @return True if the value of the environment variable DEBUG is
   *         defined and not empty.
   *
   * @ref breakpoint
   */
  inline
  bool
  running_under_debugger() {
#if NDEBUG
    return false;
#else
#ifdef DEBUG_VAR
#define X(n) #n
    static char const* env = std::getenv(X(DEBUG_VAR));
#undef X
#else
    static char const* env = std::getenv("DEBUG");
#endif // DEBUG_VAR
    static bool yes = env && *env;
    return yes;
#endif // NDEBUG
  }

  /**
   * Emit a software breakpoint.
   *
   * @param unconditional: If false emit only if @ref
   *                       running_under_debugger.
   */
  inline
  void
  breakpoint(bool unconditional = true) {
    if (running_under_debugger() || unconditional) {
#if defined(__linux__)
      asm("int $3");            // PC platform: software breakpoint
#else
      std::raise(SIGTRAP);
#endif
    }
  }

  /**
   * Prints descr and errno and the exits unconditionally with EXIT_FAILURE.
   *
   * Unless NDEBUG macro define call @ref breakpoint.
   *
   *     $ # Example 1
   *     $ LIBREALTIME_DEBUG=1 gdb --args executable args1 args2 args3
   *
   *     $ # Example 2
   *     $ LIBREALTIME_DEBUG=1 gdb executable
   *     (gdb) run args1 args2 args3
   *
   * This function exits with EXIT_FAILURE.
   */
  void fail(char const* descr) NORETURN;
  void fail(char const* descr, char const* file, unsigned line) NORETURN;

  inline
  void
  fail(char const* descr) {
    if (descr) {
      if (errno)
        std::perror(descr);
      else
        std::fprintf(stderr, "%s\n", descr);
    } else if (errno) {
      std::perror("");
    }
    std::fflush(stderr);
#if NDEBUG
#else
    breakpoint();
#endif
    std::exit(EXIT_FAILURE);
  }

  inline
  void
  fail(char const* descr, char const* file, unsigned line) {
    std::fprintf(stderr, "FAIL: %s(%u): ", file, line);
    fail(descr);
  }
#define TRACE             ::base::trace
#define VERIFY(expr)      do { if (!(expr)) ::base::fail(#expr, __FILE__, __LINE__); } while ((0))
#define BADSYSCALL(str)   (::base::fail(str, __FILE__, __LINE__), false)

  /**
   * Print to STDERR with timestamp prefix.
   */
  inline
  void
  trace(char const* fmt, ...) {
    std::va_list val;
    va_start(val, fmt);
    std::time_t now;
    std::time(&now);
    char* s = std::ctime(&now);
    *std::strchr(s, '\n') = '\0';
    std::fprintf(stderr, "%s  ", s);
    std::vfprintf(stderr, fmt, val);
    std::fflush(stderr);
    va_end(val);
  }
} /* base */
