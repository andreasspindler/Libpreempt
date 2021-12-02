/* -*-coding:raw-text-unix-*-
 *
 * preempt/process.h -- realtime process
 */
#pragma once

#include <base/posix.h>

namespace preempt {
pid_t
get_current_process_id();

pid_t
get_parent_process_id();

namespace this_process {
/**
 * The only thing known to mess with the realtime scheduling policies is the
 * memory manager. To disable pagefaults all pages must be locked before calling
 * pthread_create() for SCHED_FIFO or SCHED_RR.
 *
 * After this function returns the code is protected from page locks. Therefore
 * it is safe to start realtime threads.
 */
void
begin_realtime();

/**
 * Undo @ref begin_realtime.
 */
void
end_realtime();

/**
 * Define RLIM_INFINITY for RLIMIT_MEMLOCK effectively allowing the process
 * to lock all virtual RAM.
 */
bool
unlimit_lock_pages();

/**
 * @brief Lock current and future memory
 *
 * Locks all pages mapped into the address space of the calling process
 * (code, data and stack segment, shared libraries, user space kernel data,
 * shared memory, and memory-mapped files) so that no page faults occur
 * after calling this function.
 *
 * All mapped pages are guaranteed to be resident in RAM when the call
 * returns; the pages are guaranteed to stay in RAM until @ref
 * unlock_all_pages is called.
 *
 * Without doing this globals and statics are loaded on page-faults. One
 * effect is that realtime threads are not always started in
 * pthread_create() order. This phenomen is evident in regular kernels as
 * well as in PREEMPT_RT-patched kernels.
 *
 * @see Linux RT manual, http://linuxrealtime.org/index.php/Main_Page
 */
bool
lock_all_pages();

/**
 * Undo the effects of @ref lock_all_pages.
 */
bool
unlock_all_pages();

/**
 * Disable writing of core file.
 */
bool
disable_core_file();

void
yield();
} // this_process

void
request_CPU_latency(unsigned c_state);

void
reset_CPU_latency();
} // preempt
