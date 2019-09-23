//
// Created by SNAKE on 9/21/19.
//

#ifndef SYSBENCH_SYSBENCH_H
#define SYSBENCH_SYSBENCH_H

#include "sb_option.h"
#include "sb_test.h"
#include "sb_util.h"
#include "sb_timer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <barrelfish/threads.h>

struct {
  int threads CK_CC_CACHELINE;
  const char *testname, *cmdname;
  sb_test_t *test;
} sb_globals CK_CC_CACHELINE;

/* Global execution timer */
sb_timer_t sb_exec_timer CK_CC_CACHELINE;

/* timers for checkpoint reports */
sb_timer_t sb_intermediate_timer;
sb_timer_t sb_checkpoint_timer;

#endif //SYSBENCH_SYSBENCH_H
