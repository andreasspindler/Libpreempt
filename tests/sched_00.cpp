/*
 * SCHED_FIFO test
 *
 * Runs realtime threads that decrement an unguarded global integer. Each thread
 * expects a certain value before decrementing the integer so the order which
 * realtime threads are scheduled is strictly checked, or the process fails.
 */
#include <base/details/system.h>
#include <base/debug.h>
#include <atomic>

std::atomic<int> value;

void* decrement(void *expected_value)
{
  assert(value == *(int*)(expected_value));
  value--;
  //::sched_yield();
  return nullptr;
}

pthread_t
create_thread(int policy, int priority, void* (*)(void*), void* argp);

int main(int argc, char *argv[])
{
  /* Create 10 threads with the same realtime priority. Because of SCHED_FIFO
     threads are executed in the order of their creation. The thread may or may
     not have started when pthread_create() but SCHED_FIFO is still respected.
     The only thing know to mess with the scheduling policy is the memory
     manager. To disale pagefaults all pages must be locked before calling
     pthread_create() with realtime policy.
  */
  base::unlimit_lock_pages();
  base::lock_all_pages();

  /* SCHED_FIFO: On multicore systems the kernel will not run multiple threads
     on multiple cores simultaneously, which would violate the defined behavior.
     A process with this scheduling policy, when selected for running, will
     continue to use the CPU until either it is blocked by an I/O request, it is
     preempted by a higher priority process (FIFO or RR), or it calls
     sched_yield().

     SCHED_FIFO tasks belong to the cathegory of the real-time (RT) processes.
     Real-time processes are in charge of critical tasks whose execution cannot
     be interrupted. These tasks are usually involved in multimedia processing.
     Because RT processes prevent any other SCHED_OTHER process from running,
     their life-time must be very short.

     SCHED_RR: Replace SCHED_FIFO by SCHED_RR and this test can go wrong with a
     probability of approx. 0.1%. The only reason why this probability is not
     higher is the shortness of decrement().

     https://man7.org/linux/man-pages/man7/sched.7.html
  */
  value = 9;
  int expected[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  pthread_t a[] {
    create_thread(SCHED_FIFO, 10, decrement, &expected[9]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[8]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[7]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[6]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[5]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[4]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[3]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[2]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[1]),
    create_thread(SCHED_FIFO, 10, decrement, &expected[0])
  };
  for (auto h : a) {
    pthread_join(h, 0);
  }

  base::unlock_all_pages();
  return EXIT_SUCCESS;
}

pthread_t
create_thread(int policy, int priority, void* (*fun)(void*), void* argp)
{
  pthread_attr_t* const attr = new pthread_attr_t;
  pthread_attr_init(attr);
  pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(attr, policy);
  sched_param* const sch = new sched_param;
  sch->sched_priority = priority;
  pthread_attr_setschedparam(attr, sch);
  pthread_t result;
  pthread_create(&result, attr, fun, argp)
    && BADSYSCALL("pthread_create");
  return result;
}
