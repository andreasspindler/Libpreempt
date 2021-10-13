/*
 * By definition a task is an object containing one ore more worker threads.
 *
 * In this test we define two task classes.
 *
 * MonoTask: class with std::thread attribute
 *
 * PolyTask: class with std::vector<std::thread> attribute
 */
#include <vector>
#include <thread>
#include <string>

#include <base/verify.h>

using namespace std;

/**
 * Distribute work to a single thread.
 */
class MonoTask {
protected:
  std::thread thread_;
public:
  /** Join, then start a new member thread. */
  template<class Function, class... Args>
  void spawn(Function&&, Args&&...);

  /** Join member thread. */
  void join();
};

/**
 * Distribute work to multiple threads.
 */
class PolyTask {
protected:
  std::vector<std::thread> threads_;
public:
  /** Push a new member thread. */
  template<class Function, class... Args>
  void spawn(Function&&, Args&&...);

  /** Join all member threads. */
  void join();
};

/***********************************************************************
 * main function
 */
int main(int argc, char *argv[])
{
  class Test {
  public:
    void eins(int n) const { VERIFY(n == 1); }
    void zwei(int n) const { VERIFY(n == 2); }
    void drei(int const& n) const { VERIFY(n == 3); }
  } t;

  int x = 3;

  MonoTask mono;
  PolyTask poly;

  for (int i = 0; i < 10; ++i) {
    mono.spawn(&Test::eins, &t, 1);
    mono.spawn(&Test::zwei, &t, 2);
    mono.spawn(&Test::drei, &t, std::ref(x));
    for (int i = 0; i < 3; ++i) {
      poly.spawn(&Test::eins, &t, 1);
      poly.spawn(&Test::zwei, &t, 2);
      poly.spawn(&Test::drei, &t, std::ref(x));
    }
  }
  mono.join();
  poly.join();
  return EXIT_SUCCESS;
}

/***********************************************************************
 * implementation
 */
template<class Function, class... Args>
void
MonoTask::spawn(Function&& f, Args&&... args) {
  join();
  thread_ = std::thread
    {std::forward<Function>(f), std::forward<Args>(args)...};
}

void
MonoTask::join() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

template<class Function, class... Args>
void
PolyTask::spawn(Function&& f, Args&&... args) {
  threads_.emplace_back
    (std::thread {std::forward<Function>(f), std::forward<Args>(args)...});
}

void
PolyTask::join() {
  for (auto& t : threads_) {
    t.join();
  }
}

/* EOF */
