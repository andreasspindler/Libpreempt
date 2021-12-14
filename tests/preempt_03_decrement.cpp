/*
 * Strictly check the order in which SCHED_FIFO threads are scheduled.
 *
 * Runs threads under SCHED_FIFO realtime policy with different priorities. Each
 * thread:
 *  - read global integer and fail unless it has a certain value
 *  - decrement the integer
 *
 * The decrement operation is neither atomic nor implemented using a lock. This
 * is unnecessary as long as the threads were started with the same or a lower
 * priority than the last FIFO thread.
 */
#include <preempt/process.h>
#include <base/pthread.h>
#include <base/debug.h>

#include <cstdlib>
#include <memory>
#include <vector>

using namespace std;

int global_value;

void* decrement(void *expected_global_value);

template <unsigned Policy, int N>
void
run(int (&priority_table)[N]);

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();

  // FIFO: decreasing or equal priorities
  { int tab[] = {  1,  1,  1 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 10, 10, 10 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 30, 20, 10 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 30, 29, 28,
                  20, 19, 18 }; run<SCHED_FIFO>(tab); }

  // RR: decreasing priorities
  { int tab[] = { 30, 20, 10 }; run<SCHED_RR>(tab); }
  { int tab[] = { 30, 29, 28,
                  27, 26, 25 }; run<SCHED_RR>(tab); }

  preempt::this_process::end_realtime();

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void*
decrement(void *expected)
{
  VERIFY(global_value == *(int*)(expected));
  global_value--;
  return nullptr;
}

template <unsigned Policy, int N>
void
run(int (&priority_table)[N])
{
  /* create expected values for each of the N threads */
  global_value = N;
  shared_ptr<int> expected[N];

  for (int i = 0; i < N; i++) {
    VERIFY(i == 0 || priority_table[i] <= priority_table[i - 1]);
    expected[i] = make_shared<int>(N - i);
  }

  /* start and join threads */
  base::pthread context[N];
  for (int i = 0; i < N; i++) {
    context[i] = base::pthread(Policy, priority_table[i], decrement, expected[i].get());
    if (context[i].joinable) {
      continue;
    } else {
      std::cerr << context[i].last_error << std::endl;
      exit(EXIT_FAILURE);
    }
    VERIFY(context[i]);
  }

  for (auto c : context) {
    VERIFY(c);
    c.join();
  }

  VERIFY(global_value == 0);
}
