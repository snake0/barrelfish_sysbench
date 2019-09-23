//
// Created by SNAKE on 9/21/19.
//

#ifndef SYSBENCH_SYSBENCH_H
#define SYSBENCH_SYSBENCH_H

#include "sb_option.h"
#include "sb_test.h"
#include "sb_util.h"
#include "sb_timer.h"
#include "sb_counter.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <barrelfish/threads.h>
#include <barrelfish_kpi/spinlocks_arch.h>
#include <barrelfish/debug.h>


struct {
  int error CK_CC_CACHELINE;
  int threads CK_CC_CACHELINE;
  int events CK_CC_CACHELINE;
  const char *testname, *cmdname;

} sb_globals CK_CC_CACHELINE;

void sb_report_cumulative(sb_stat_t *stat);

#endif //SYSBENCH_SYSBENCH_H
