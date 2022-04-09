/* -*- coding: raw-text-unix; -*-
 *
 * Basic real-time program.
 */
#include <preempt/all.h>

int main(int argc, char *argv[])
{
  preempt::this_process::begin_realtime();
  {
    // your code
  }
  preempt::this_process::end_realtime();

  return global_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
