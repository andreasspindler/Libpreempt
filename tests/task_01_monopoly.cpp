/*
 * MonoTask: class with std::thread attribute
 *
 * PolyTask: class with std::vector<std::thread> attribute
 */
#include <vector>
#include <thread>
#include <string>

#include <preempt/task.h>
#include <base/debug.h>

namespace pre = preempt;

template <typename Thread> using MonoTask = pre::mono_task<Thread>;
template <typename Thread> using PolyTask = pre::poly_task<Thread>;

/***********************************************************************
 * test code
 */
class Run {
public:
  void fun1(int n) const { VERIFY(n == 1); }
  void fun2(int const& n) const { VERIFY(n == 2); }
  void fun3(int n) { VERIFY(n == 3); }
} t;

template <typename Thread>
void test()
{
  int const x = 2;

  MonoTask<Thread> mono;
  PolyTask<Thread> poly;

  mono.spawn(&Run::fun1, &t, 1);
  mono.spawn(&Run::fun2, &t, std::ref(x));
  mono.spawn(&Run::fun3, &t, 3);
  for (int i = 0; i < 3; ++i) {
    poly.spawn(&Run::fun1, &t, 1);
    poly.spawn(&Run::fun2, &t, std::ref(x));
    poly.spawn(&Run::fun3, &t, 3);
  }
  mono.join();
  poly.join();
}

void test(int priority, int policy)
{
  int const x = 3;

  MonoTask<pre::thread> mono;
  PolyTask<pre::thread> poly;

  mono.spawn(&Run::fun1, &t, 1).change_scheduling(policy, priority);
  mono.spawn(&Run::fun2, &t, 2).change_scheduling(policy, priority);
  mono.spawn(&Run::fun3, &t, std::ref(x)).change_scheduling(policy, priority);
  for (int i = 0; i < 3; ++i) {
    poly.spawn(&Run::fun1, &t, 1).change_scheduling(policy, priority);
    poly.spawn(&Run::fun2, &t, 2).change_scheduling(policy, priority);
    poly.spawn(&Run::fun3, &t, std::ref(x)).change_scheduling(policy, priority);
  }
  mono.join();
  poly.join();
}

/***********************************************************************
 * main function
 */
int main(int argc, char *argv[])
{
  /* no priority given = not realtime */
  test<std::thread>();
  test<pre::thread>();

  /* priority given = realtime */
  test(1, SCHED_FIFO);
  test(2, SCHED_RR);

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
