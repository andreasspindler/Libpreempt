/* -*-coding:raw-text-unix-*-
 *
 * base/idioms.h -- common C++ idioms
 */
#pragma once

#include <mutex>

#include <base/utility.h>     // BASE_STD_GUARD

namespace base {
/**
 * @brief Basic runnable interface
 */
class runnable {
public:
  /**
   * Create task.
   *
   * WARNING: Do not call start() in the ctor. The thread may start
   * before this method returns. In C++ the vtable-ptr is set at the
   * end of the ctor, so that @ref run() could be called as pure
   * virtual function.
   */
  virtual void start() { }
  virtual void stop() { }
  virtual void run() = 0;
};

/**
 * @brief Not copyable
 *
 * There are no "noncopyable objects", i.e. there is no case where a
 * noncopyable pointer is needed. Deriving from noncopyable doesn't
 * count as derived from a base class.
 *
 * For this reason:
 *
 * - virtual dtor is not required
 *
 * - always inherit this class "private"
 */
class noncopyable
{
protected:
  noncopyable() { }
  ~noncopyable() { }
private:
  noncopyable(const noncopyable&);
  noncopyable& operator = (const noncopyable&);
};

/**
 * Basic lock interface.
 *
 * @ref https://en.cppreference.com/w/cpp/named_req/BasicLockable
 */
class lockable
{
protected:
  lockable() { }
public:
  virtual ~lockable() { }

  virtual void lock() const noexcept = 0;
  virtual bool try_lock() const noexcept { return false; }
  virtual bool try_lock_for(unsigned long = 0) const noexcept { return false; }
  virtual void unlock() const noexcept = 0;
};

/**
 * Dependency Injection (DI) design pattern.
 */
template <typename Params>
class depends_on {
public:
  void   set_parameters(Params const&);
  Params get_parameters() const;
private:
  Params params_;
  std::mutex lock_;
};

/***********************************************************************
 * inlined implementation
 */
template <typename Params>
void
depends_on<Params>::set_parameters(Params const& par) {
  BASE_STD_GUARD(lock_);
  params_ = par;
}

template <typename Params>
Params
depends_on<Params>::get_parameters() const {
  BASE_STD_GUARD(lock_);
  Params result {params_};
  return result;             // use deferred temporary materialization (C++17)
}
} /* base */
