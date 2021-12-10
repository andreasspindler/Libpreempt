/* -*-coding:utf-8-unix-*-
 *
 * base/algorithm.h
 */
#pragma once

#include <algorithm>

#include <array>
#include <vector>

namespace base {
/**
 * Simple ascending sort.
 */
template <typename T>
int bubble_sort(std::vector<T> &v) {
  bool swapped;
  int result = 0;
  for (int i = 1; i < v.size(); i++) {
    swapped = false;
    for (int j = 0; j < v.size() - 1; j++) {
      // ascending<=, descending=>
      if (v[j + 1] <= v[j]) {
        std::swap(v[j], v[j + 1]);
        swapped = true;
        result++;
      }
    }
    if (not swapped)
      break;
  }
  return result;
}

template <typename T, std::size_t N>
int bubble_sort(std::array<T, N> &v) {
  bool swapped;
  int result = 0;
  for (int i = 1; i < N; i++) {
    swapped = false;
    for (int j = 0; j < N - 1; j++) {
      // ascending<=, descending=>
      if (v[j + 1] <= v[j]) {
        std::swap(v[j], v[j + 1]);
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
