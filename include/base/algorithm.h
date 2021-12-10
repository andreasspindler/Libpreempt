/* -*-coding:utf-8-unix-*-
 *
 * base/algorithm.h
 */
#pragma once

#include <algorithm>
#include <vector>

namespace base {
/**
 * Simple sort.
 */
template <typename T>
int bubble_sort(std::vector<T> &v) {
  bool swapped;
  int result = 0;
  for (size_t i = 1; i < v.size(); i++) {
    swapped = false;
    for (size_t j = 0; j < v.size() - 1; j++) {
      // ascending=<, descending=>
      if (v[j + 1] <= v[j]) {
        swap(v[j], v[j + 1]);
        swapped = true;
        result++;
      }
    }
    if (not swapped)
      break;
  }
  return result;
}
} /* base */
