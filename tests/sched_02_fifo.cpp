/*
 * SCHED_FIFO test
 *
 * Runs threads under SCHED_FIFO realtime policy with same priority that
 * decrement an unguarded global integer. Each thread expects a certain value
 * before decrementing the integer so the order which realtime threads are
 * scheduled is strictly checked, or the process fails.
 *
 * - SCHED_FIFO threads belong to the cathegory of the real-time (RT) processes.
 *   Real-time processes are in charge of critical tasks whose execution cannot
 *   be interrupted. These tasks are usually involved in multimedia processing.
 *
 * - Because RT processes prevent any other SCHED_OTHER process from running, in
 *   general their life-time should be [very] short.
 *
 * - The thread may or may not have started when pthread_create() returns but
 *   SCHED_FIFO threads are always executed in the order of their creation.
 *
 * - On multicore systems the kernel will not run multiple threads on multiple
 *   cores simultaneously, which would violate the defined behavior. A process
 *   with this scheduling policy, when selected for running, will continue to
 *   use the CPU until either it is blocked by an I/O request, it is preempted
 *   by a higher priority process (FIFO or RR), or it calls sched_yield().
 *
 * https://man7.org/linux/man-pages/man7/sched.7.html
 */
#include <preempt/process.h>
#include <preempt/pthread.h>

#include <cassert>

int global_value;

void* decrement(void *expected_global_value);

int main(int argc, char *argv[])
{
  using namespace preempt;

  this_process::begin_realtime();

  global_value = 9;

  int expected[] { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

  pthread::context a[] {
    /* Replace SCHED_FIFO by SCHED_RR and this test can go wrong with a
       probability of approx. 0.1%. The only reason why this probability is not
       higher is the shortness of decrement(). */
    pthread::create(decrement, &expected[0], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[1], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[2], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[3], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[4], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[5], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[6], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[7], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[8], SCHED_FIFO, 10),
    pthread::create(decrement, &expected[9], SCHED_FIFO, 10),
  };

  for (auto c : a) {
    pthread::join(c);
  }

  this_process::end_realtime();
}

void* decrement(void *expected)
{
  /* Since the thread priorities are graduated, it is not necessary to define
     global_value as an atomic int (the function is not interrupted by another
     SCHED_FIFO thread). */
  assert(global_value == *(int*)(expected));
  global_value--;
  return nullptr;
}
