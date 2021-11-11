/*
 * MonoTask: class with std::thread attribute
 *
 * PolyTask: class with std::vector<std::thread> attribute
 */
#include <vector>
#include <thread>
#include <string>

#include <preempt/thread.h>
#include <base/verify.h>

/**
 * Distributes work to a single thread.
 */
template <typename Thread>
class MonoTask {
protected:
  Thread thread_;
public:
  /** Join, then start a new member thread. */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /** Join the member thread. */
  void join();
};

/**
 * Distributes work to multiple threads.
 */
template <typename Thread>
class PolyTask {
protected:
  std::vector<Thread> threads_;
public:
  /** Push a new member thread. */
  template<class Function, class... Args>
  Thread& spawn(Function&&, Args&&...);

  /** Join all member threads. */
  void join();
};

/***********************************************************************
 * test code
 */
class Run {
public:
  void eins(int n) const { VERIFY(n == 1); }
  void zwei(int n) const { VERIFY(n == 2); }
  void drei(int const& n) const { VERIFY(n == 3); }
} t;

template <typename Thread>
void test_no_priority()
{
  int const x = 3;

  MonoTask<Thread> mono;
  PolyTask<Thread> poly;

  for (int i = 0; i < 10; ++i) {
    mono.spawn(&Run::eins, &t, 1);
    mono.spawn(&Run::zwei, &t, 2);
    mono.spawn(&Run::drei, &t, std::ref(x));
    for (int i = 0; i < 3; ++i) {
      poly.spawn(&Run::eins, &t, 1);
      poly.spawn(&Run::zwei, &t, 2);
      poly.spawn(&Run::drei, &t, std::ref(x));
    }
  }
  mono.join();
  poly.join();
}

void test_priority(int policy, int priority)
{
  int const x = 3;

  MonoTask<preempt::thread> mono;
  PolyTask<preempt::thread> poly;

  for (int i = 0; i < 10; ++i) {
    mono.spawn(&Run::eins, &t, 1).change_scheduling(policy, priority);
    mono.spawn(&Run::zwei, &t, 2).change_scheduling(policy, priority);
    mono.spawn(&Run::drei, &t, std::ref(x)).change_scheduling(policy, priority);
    for (int i = 0; i < 3; ++i) {
      poly.spawn(&Run::eins, &t, 1).change_scheduling(policy, priority);
      poly.spawn(&Run::zwei, &t, 2).change_scheduling(policy, priority);
      poly.spawn(&Run::drei, &t, std::ref(x)).change_scheduling(policy, priority);
    }
  }
  mono.join();
  poly.join();
}

/***********************************************************************
 * main function
 */
int main(int argc, char *argv[])
{
  test_no_priority<std::thread>();
  test_no_priority<preempt::thread>();
  test_priority(SCHED_FIFO, 1);

  return EXIT_SUCCESS;
}

/***********************************************************************
 * implementation
 */
template <class Thread>
template <class Function, class... Args>
Thread&
MonoTask<Thread>::spawn(Function&& f, Args&&... args) {
  join();
  thread_ = Thread
    {std::forward<Function>(f), std::forward<Args>(args)...};
  return thread_;
}

template <class Thread>
void
MonoTask<Thread>::join() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

template <class Thread>
template<class Function, class... Args>
Thread&
PolyTask<Thread>::spawn(Function&& f, Args&&... args) {
  threads_.emplace_back
    (Thread {std::forward<Function>(f), std::forward<Args>(args)...});
  return threads_.back();
}

template <class Thread>
void
PolyTask<Thread>::join() {
  for (auto& t : threads_) {
    t.join();
  }
}
