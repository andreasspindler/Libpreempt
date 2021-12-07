#pragma once

#include <cstdlib>
#include <mutex>                // std::lock_guard

#ifndef NO_BASE_MACROS
#define BASE_LINE_VAR(var)      var ## __LINE__
#define BASE_STD_GUARD(lock)    std::lock_guard<decltype(lock)> BASE_LINE_VAR(g) {lock}
#endif // NO_BASE_MACROS
