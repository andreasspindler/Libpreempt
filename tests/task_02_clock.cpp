/*
 * Find out execution and idle time of thread functions.
 *
 * The purpose is to get an impression of how much time a function actually
 * consumes from its slice given by the kernel. Of course this is more relevant
 * for realtime threads.
 *
 * Note that in a computer program there is no "time" - only time points and
 * durations.
 */
#include <base/testing.h>

using namespace std;
using namespace chrono;

using deadline_error = std::logic_error;

/**
 * This task runs a thread that measures the time it consumes.
 *
 * @param LogicalTs: Logical time slice. Abort the process if ever more than
 *                   this number of milliseconds pass in the function body.
 */
template <int LogicalTs>
class SafeTask {
public:
  SafeTask();

  void run();
  void join();

  /** Time of run() */
  long long run_min = -1, run_max = -1;

  /** Time between invocations of run() */
  long long idle_min = -1, idle_max = -1;

private:
  high_resolution_clock::time_point stop;

  /** The thread that executes run() */
  std::thread thread;
};

template <int LogicalTs>
void
printt(std::string, SafeTask<LogicalTs> const&);

int
main(int argc, char *argv[])
{
  SafeTask<10> task10ms;
  SafeTask<20> task20ms;
  SafeTask<30> task30ms;
  try {
    task10ms.join();
    printt("task10ms", task10ms);
    task20ms.join();
    printt("task20ms", task20ms);
    task30ms.join();
    printt("task30ms", task30ms);
    return EXIT_SUCCESS;
  } catch (deadline_error e) {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
}

template <int LogicalTs>
void
printt(std::string label, SafeTask<LogicalTs> const& t) {
  cerr << label << ": run() min/max/avg = " << t.run_min << "/" << t.run_max << "/" << (t.run_min + t.run_max) / 2 << " nsec" << endl;
  cerr << label << ": idle  min/max/avg = " << t.idle_min << "/" << t.idle_max << "/" << (t.idle_min + t.idle_max) / 2 << " nsec" << endl;
}

template <int LogicalTs>
SafeTask<LogicalTs>::SafeTask()
  : stop {high_resolution_clock::now()},
    thread {&SafeTask::run, this} { }

template <int LogicalTs>
void
SafeTask<LogicalTs>::join() {
  thread.join();
}

template <int LogicalTs>
void
SafeTask<LogicalTs>::run()
{
  high_resolution_clock::time_point start = high_resolution_clock::now();
  high_resolution_clock::time_point deadline = start + milliseconds {LogicalTs};
  auto us = duration_cast<nanoseconds>(start - stop);
  if (idle_min == -1 || us.count() < idle_min) idle_min = us.count();
  if (idle_max == -1 || us.count() > idle_max) idle_max = us.count();
  {
    /* wait 3ms */
    std::this_thread::sleep_for(std::chrono::milliseconds      {1});
    std::this_thread::sleep_for(std::chrono::microseconds   {1000});
    std::this_thread::sleep_for(std::chrono::nanoseconds {1000000});
  }
  stop = high_resolution_clock::now();
  us = duration_cast<nanoseconds>(stop - start);
  if (run_min == -1 || us.count() < run_min) run_min = us.count();
  if (run_max == -1 || us.count() > run_max) run_max = us.count();
  if (stop > deadline) {
    /* test failed */
    throw deadline_error {__FUNCTION__};
  }
}
