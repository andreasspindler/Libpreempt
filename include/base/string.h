/* -*-coding:raw-text-unix-*-
 *
 * base/string.h -- string utilities
 */
#pragma once

#include <string>
#include <cstdio>

namespace base {
template <typename ...Args>
std::string
sprintf(std::string format, Args && ...args) {
  auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
  std::string buffer(size + 1, '\0');
  std::sprintf(&buffer[0], format.c_str(), std::forward<Args>(args)...);
  return buffer;
}

inline
bool
is_quoted(std::string str, char front = '"', char back = '"') {
  return str.empty() ? false : str.front() == front && str.back() == back;
}

inline
std::string
unquote(std::string str, char front = '"', char back = '"') {
  if (is_quoted(str, front, back)) {
    return str.substr(1, str.length() - 2);
  }
  return str;
}
}
