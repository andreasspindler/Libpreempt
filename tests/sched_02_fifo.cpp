/*
 * SCHED_FIFO test -- equal priorities
 *
 * Runs threads under SCHED_FIFO realtime policy with same priority that
 * decrement an unguarded global integer. Each thread expects a certain value
 * before decrementing the integer so the order which realtime threads are
 * scheduled is strictly checked, or the process fails.
 *
 * Note that the decrement operation is neither atomic nor implemented using a
 * lock. This is unnecessary because although the threads have the same
 * priority, they are executed in FIFO, i.e. in the order in which
 * pthread_create() was called.
 */
#include <preempt/process.h>
#include <preempt/pthread.h>

#include <cassert>

using namespace preempt;

int global_value;

void* decrement(void *expected_global_value);

int main(int argc, char *argv[])
{
  global_value = 9;

  int expected[] { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  int priority = 1;

  this_process::begin_realtime();

  /* Replace SCHED_FIFO by SCHED_RR and this test can go wrong with a
     probability of approx. 0.1%. The only reason why this probability is not
     higher is the shortness of decrement(). */
  pthread::context handle[10] {
    pthread::create(decrement, &expected[0], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[1], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[2], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[3], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[4], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[5], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[6], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[7], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[8], SCHED_FIFO, priority),
    pthread::create(decrement, &expected[9], SCHED_FIFO, priority),
  };

  for (int i = 0; i < 10; ++i) {
    assert(handle[i]);
    pthread::join(handle[i]);
  }

  this_process::end_realtime();

  if (global_value == -1) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}

void* decrement(void *expected)
{
  assert(global_value == *(int*)(expected));
  global_value--;
  return nullptr;
}
