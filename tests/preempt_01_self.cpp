/* -*- coding: raw-text-unix; -*- */
#include <preempt/all.h>

int main(int argc, char *argv[])
{
  return get_verify_flag() ? EXIT_SUCCESS : EXIT_FAILURE;
}
