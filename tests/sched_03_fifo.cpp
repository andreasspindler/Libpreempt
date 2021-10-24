/*
 * SCHED_FIFO test -- different priorities
 *
 * Runs threads under SCHED_FIFO realtime policy with different priorities. Each
 * threads decrement an unguarded global integer. Before doing this it expects a
 * certain value so that the order in which realtime threads are scheduled is
 * strictly checked.
 *
 * Note that the decrement operation is neither atomic nor implemented using a
 * lock. This is unnecessary as long as the threads were started with the same
 * or a lower priority than the last FIFO thread.
 */
#include <preempt/process.h>
#include <preempt/posix.h>

#include <base/verify.h>

#include <cstdlib>              // exit()
#include <memory>
#include <vector>

using namespace std;
using namespace preempt;

int global_value;

void* decrement(void *expected_global_value);

template <unsigned Policy, int N>
void
run(int (&priority_table)[N]);

int main(int argc, char *argv[])
{
  this_process::begin_realtime();

  // FIFO: decreasing or equal priorities
  { int tab[] = {  0,  0,  0 }; run<SCHED_FIFO>(tab); }
  { int tab[] = {  1,  1,  1 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 10, 10, 10 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 30, 20, 10 }; run<SCHED_FIFO>(tab); }
  { int tab[] = { 30, 29, 28,
                  20, 19, 18 }; run<SCHED_FIFO>(tab); }

  // RR: decreasing priorities
  { int tab[] = { 30, 20, 10 }; run<SCHED_RR>(tab); }
  { int tab[] = { 30, 29, 28,
                  27, 26, 25 }; run<SCHED_RR>(tab); }

  this_process::end_realtime();

  return EXIT_SUCCESS;
}

void* decrement(void *expected)
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
  pthread::context handle[N];
  for (int i = 0; i < N; i++) {
    handle[i] = pthread::create(decrement, expected[i].get(), Policy, priority_table[i]);
    VERIFY(handle[i]);
  }

  for (auto h : handle) {
    VERIFY(h);
    pthread::join(h);
  }

  /* after N decrement() the value has to be 0 again */
  if (global_value != 0)
    exit(EXIT_FAILURE);
}
