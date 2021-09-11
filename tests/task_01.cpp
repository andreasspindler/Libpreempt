/*
 * MonoTask: class with std::thread attribute
 *
 * PolyTask: class with std::vector<std::thread> attribute
 */
#include <vector>
#include <thread>
#include <string>
#include <cassert>

using namespace std;

template <unsigned>
class Worker;

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

class Test {
public:
  void eins(int n) const { assert(n == 1); }
  void zwei(int n) const { assert(n == 2); }
  void drei(int const& n) const { assert(n == 3); }
};

int main(int argc, char *argv[])
{
  Test t;
  int x = 3;

  MonoTask m;
  PolyTask p;

  for (int i = 0; i < 10; ++i) {
    m.spawn(&Test::eins, &t, 1);
    m.spawn(&Test::zwei, &t, 2);
    m.spawn(&Test::drei, &t, std::ref(x));
    for (int i = 0; i < 3; ++i) {
      p.spawn(&Test::eins, &t, 1);
      p.spawn(&Test::zwei, &t, 2);
      p.spawn(&Test::drei, &t, std::ref(x));
    }
  }
  m.join();
  p.join();
  return 0;
}

/***********************************************
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
