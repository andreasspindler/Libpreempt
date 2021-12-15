/* -*-coding:utf-8-unix-*-
 *
 * base/utility.h -- shared_ptr_less, ptr_cast, atomic_ref
 *
 * In general this header contains tools that are not available in a specific
 * C++ standard yet. For example, std::atomic_ref are is only available in C++20
 * and std::optional not before C++17.
 */
#pragma once

#include <base/details/cc.h>

#include <functional>
#include <memory>               // std::shared_ptr
#include <mutex>                // std::lock_guard
#include <utility>
#include <optional>

#include <cassert>

#ifndef NO_BASE_MACROS
#define BASE_LINE_VAR(var)      var ## __LINE__
#define BASE_STD_GUARD(lock)    std::lock_guard<decltype(lock)> BASE_LINE_VAR(g) {lock}
#endif // NO_BASE_MACROS

namespace base {
/**
 * std::less for raw pointers.
 */
template <typename T, typename C = std::less<T>>
struct ptr_less
  : std::binary_function<T*, T*, bool> {
  bool operator()(T const* a, T const* b) const {
    return C()(*a, *b);
  }
};

/**
 * std::less for std::shared_ptr.
 */
template <typename T, typename C = std::less<T>>
struct shared_ptr_less :
    std::binary_function<std::shared_ptr<T>, std::shared_ptr<T>, bool> {
  bool operator()(std::shared_ptr<T> const& a,
                  std::shared_ptr<T> const& b) const {
    return C()(*a, *b);
  }
};

/**
 * Like std::atomic_ref
 */
template<typename T>
using atomic_ref = std::shared_ptr<std::atomic<T>>;

/**
 * @example
 *
 *     class X;
 *     base::atomic_ref<T> ref {make_atomic_ref(X{})};
 *     ...
 *     ref = make_atomic_ref(X{});
 *
 * Thread-safety is only guaranteed for the context object within the
 * smart-pointer and not the hosted value.
 */
template<typename T>
atomic_ref<T>
make_atomic_ref(T const& value) {
  return std::make_shared<std::atomic<T>>(value);
}

/**
 * @example
 *
 *     // p is a shared_ptr<Base>
 *     shared_ptr<Derived> p2 = dynamic_task_cast<Derived>(p);
 *
 * @param U: TaskThread-derived type.
 *
 * @param T: U-derived type.
 *
 * @result When the returned pointer is empty the task types are
 *         incompatible.
 */
template <class T, class U>
std::shared_ptr<T>
ptr_cast(const std::shared_ptr<U>& r) noexcept {
  if (auto p = dynamic_cast<typename std::shared_ptr<T>::element_type*>(r.get())) {
    /* Aliasing constructor: shares ownership information with r, but holds an
       unrelated and unmanaged pointer ptr. */
    return std::shared_ptr<T>(r, p);
  } else {
    return std::shared_ptr<T>();
  }
}

template <class T, class U>
std::shared_ptr<T>
static_ptr_cast(const std::shared_ptr<U>& r) noexcept {
  auto p = static_cast<typename std::shared_ptr<T>::element_type*>(r.get());
  return std::shared_ptr<T>(r, p);
}

/**
 * @brief Read optional value or throw
 *
 * Tool function to deal with optional values. Will stop in debugger when the
 * optional value is not available.
 *
 * May throw std::bad_optional_access.
 *
 * @see optional_default
 */
template <typename T_opt>
auto
optional_value(T_opt o) {
  assert(o.has_value());
  auto result{o.value()};
  return result;
}

/**
 * @brief Read optional or return default in case of std::nullopt
 *
 * Prevent exceptions in ctors when the class does not want to deal with
 * optional attributes and std::bad_optional_access exceptions.
 *
 * @see optional_value
 */
template <typename T, template <typename> class T_opt>
T
optional_default(T_opt<T> o, T def = T{}) noexcept {
  return o.value_or(def);
}
} /* base */
