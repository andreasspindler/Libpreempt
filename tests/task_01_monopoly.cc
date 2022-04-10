/*
 * MonoTask: class with one member thread (using std::thread or preempt::thread
 * attribute)
 *
 * PolyTask: class with many member threads (using std::vector<std::thread> or
 * std::vector<preempt::thread> attribute)
 */
#include <vector>
#include <thread>
#include <string>

#include <preempt/task.h>
#include <base/verify.h>

#include <preempt/process.h>

namespace pre = preempt;

/***********************************************************************
 * test code
 */
class Run {
public:
  void fun1(int n) const { VERIFY(n == 1); }
  void fun2(int const& n) const { VERIFY(n == 2); }
  void fun3(int n) { VERIFY(n == 3); }
} object;

template <typename Thread> using MonoTask = pre::mono_task<Thread>;
template <typename Thread> using PolyTask = pre::poly_task<Thread>;

/*
 * Thread = std::thread or preempt::thread.
 *
 * Either will run as ordinary thread (SCHED_OTHER, priority 0).
 */
template <typename Thread>
void test()
{
  MonoTask<Thread> mono;
  PolyTask<Thread> poly;

  int const x = 2;

  mono.spawn(&Run::fun1, &object, 1);
  mono.spawn(&Run::fun2, &object, std::ref(x));
  mono.spawn(&Run::fun3, &object, 3);
  for (int i = 0; i < 3; ++i) {
    poly.spawn(&Run::fun1, &object, 1);
    poly.spawn(&Run::fun2, &object, std::ref(x));
    poly.spawn(&Run::fun3, &object, 3);
  }
  poly.join();
  mono.join();
}

/*
 * preempt::thread with explicit scheduling and priority.
 *
 * preempt::thread is created with SCHED_OTHER and priority 0. To chance the
 * policy and priority call change_scheduling() on the spawned thread object.
 */
void test(int priority, int policy)
{
  int const local = 3;

  MonoTask<pre::thread> mono;
  PolyTask<pre::thread> poly;

  mono.spawn(&Run::fun1, &object, 1).change_scheduling(policy, priority);
  mono.spawn(&Run::fun2, &object, 2).change_scheduling(policy, priority);
  mono.spawn(&Run::fun3, &object, std::ref(local)).change_scheduling(policy, priority);
  for (int i = 0; i < 3; ++i) {
    poly.spawn(&Run::fun1, &object, 1).change_scheduling(policy, priority);
    poly.spawn(&Run::fun2, &object, 2).change_scheduling(policy, priority);
    poly.spawn(&Run::fun3, &object, std::ref(local)).change_scheduling(policy, priority);
  }
  poly.join();
  mono.join();
}

/***********************************************************************
 * main function
 */
int main(int argc, char *argv[])
{
  try {
    using namespace preempt;
    this_process::begin_realtime();
    {
      /* begin of test */

      /* no priority given = not realtime */
      test<std::thread>();
      test<pre::thread>();

      /* priority given = realtime (only pre::thread) */
      test(1, SCHED_FIFO);
      test(2, SCHED_RR);

      /* end of test */
    }
    this_process::end_realtime();
  } catch (std::runtime_error const& ex) {
    std::cerr << "catched: " << ex.what() << std::endl;
    // https://stackoverflow.com/questions/2443135/how-do-i-find-where-an-exception-was-thrown-in-c#2445569
    // base::terminate_with_backtrace()
    std::terminate();
  }

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
