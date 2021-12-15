/*
 * SCHED_FIFO threads with equal priorities
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
#include <base/threading.h>
#include <base/verify.h>

#include <iostream>

int global_value;

void* decrement(void *expected_global_value);

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();

  global_value = 9;

  int expected[] { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

  /* Replace SCHED_FIFO by SCHED_RR and this test can go wrong. */
  base::thread handle[10] {
    base::thread(SCHED_FIFO, 1, decrement, &expected[0]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[1]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[2]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[3]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[4]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[5]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[6]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[7]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[8]),
    base::thread(SCHED_FIFO, 1, decrement, &expected[9]),
  };

  for (int i = 0; i < 10; ++i) {
    if (handle[i]) {
      handle[i].join();
    } else {
      std::cerr << handle[i].last_error << std::endl;
      return EXIT_FAILURE;
    }
  }

  preempt::this_process::end_realtime();

  VERIFY(global_value == -1);

  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void* decrement(void *expected)
{
  VERIFY(global_value == *(int*)(expected));
  global_value--;
  return nullptr;
}
