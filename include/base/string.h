/* -*-coding:raw-text-unix-*-
 *
 * base/string.h -- string utilities
 */
#pragma once

#include <string>
#include <vector>
#include <cstdio>

#include <base/details/cc.h>

namespace base {
/**
 * Join std::strings at ' '.
 */
inline
std::string
join(std::vector<std::string> const &v, char sep = ' ') {
  using namespace std;
  string R;
  for (vector<string>::const_iterator i = v.begin(); i != v.end(); i++) {
    R += *i;
    if (i == v.end() - 1)
      R += sep;
  }
  return R;
}

/**
 * Split std::string at separator, skip empty tokens.
 */
inline
std::vector<std::string>
split(std::string const& str, char sep = ' ') {
  using namespace std;
  vector<string> result;
  size_t pos = 0, stop;
  while ((stop = str.find(sep, pos)) != string::npos) {
    if (stop != pos)          // skip empty tokens
      result.push_back(str.substr(pos, stop - pos));
    pos = stop + 1;
  }
  if (stop != pos)
    result.push_back(str.substr(pos));
  return result;
}

/**
 * Sprintf for std::string.
 */
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
} /* base */
