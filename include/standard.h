/* -*-mode: c++; coding:raw-text-unix;-*-
 *
 * standard.h -- include all headers of the C++17 standard library
 *
 * For those who dare to let the compiler do the compilation time.
 */
#include <base/details/cc.h>

#if   HAVE_CXX_17
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <future>
#include <map>
#elif HAVE_CXX_14
#elif HAVE_CXX_11
#endif
