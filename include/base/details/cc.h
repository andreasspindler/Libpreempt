/* -*-coding:raw-text-unix-*-
 *
 * base/details/cc.h
 */
#pragma once

/***********************************************
 * Test for GCC 4.9.2+
 *
 * GCC 4.8.1: C++11 complete
 *
 * GCC 4.9.x: C++14 features
 */
#if __GNUC__ > 4 || (__GNUC__ == 4 &&                                   \
                     (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 &&     \
                                             __GNUC_PATCHLEVEL__ >= 0)))
#else
#error GCC < 4.9.2
#endif

/***********************************************
 * The build system must have enabled POSIX
 *
 * There are various POSIX APIs. The program must define which parts
 * of POSIX are required.
 *
 * _GNU_SOURCE includes everything: ISO C89, ISO C99, POSIX.1,
 * POSIX.2, BSD, SVID, X/Open, LFS, and GNU extensions. In the cases
 * where POSIX.1 conflicts with BSD, the POSIX definitions take
 * precedence.
 */
#if defined(_GNU_SOURCE)
#else
#error compile flag '-D_GNU_SOURCE' missing
#endif

/***********************************************
 * Test for C++0x or better
 *
 * HAVE_CXX_17 is define for C++17 or later.
 *
 * HAVE_CXX_14 is define for C++14 or later.
 *
 * HAVE_CXX_11 is define for C++14 or later.
 *
 * HAVE_CXX_0X is only defined for C++0x as implemented by gcc 4.9.2+
 */
#if   __cplusplus >  201402L
#define HAVE_CXX_17 1
#elif __cplusplus == 201402L    // -std=c++14
#define HAVE_CXX_14 1
#elif __cplusplus == 201103L    // -std=c++11 or -std=c++0x
#if __GXX_EXPERIMENTAL_CXX0X__
#define HAVE_CXX_0X 1
#else
#define HAVE_CXX_11 1
#endif /* __GXX_EXPERIMENTAL_CXX0X__ */
#elif __cplusplus == 199711L
#error C++98
#else
#error C++??
#endif /* __cplusplus */

#if     HAVE_CXX_17
#define HAVE_CXX_14 1
#endif
#if     HAVE_CXX_14
#define HAVE_CXX_11 1
#endif

#if HAVE_CXX_11 || HAVE_CXX_0X
#define HAVE_STD_ATOMIC 1
#else
#define HAVE_STD_ATOMIC 0
#endif

#if __GNUC__ && HAVE_CXX_11 /* or C99 */
#   define HAVE_LONG_LONG 1
#elif _MSC_VER
#   define HAVE_LONG_LONG (_INTEGRAL_MAX_BITS == 64)
#endif

#define TOKEN_PASTE(a, b)   a##b
#define TOKEN_CAT(  a, b)   TOKEN_PASTE(a, b)

/* only GCC and Clang */
#define ALWAYS_INLINE __attribute__((always_inline))
#define NORETURN      __attribute__((noreturn))
