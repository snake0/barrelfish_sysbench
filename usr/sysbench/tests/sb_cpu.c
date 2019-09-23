//
// Created by SNAKE on 9/21/19.
//

#include "sb_cpu.h"
#include "sb_test.h"
#include "sb_option.h"
#include "sb_list.h"
#include "sysbench.h"
#include <barrelfish/domain.h>

/* CPU test arguments */
static sb_option_t cpu_options[] =
  {
    SB_OPT("cpu-max-prime", "upper limit for primes generator", "10000", INT),

    SB_OPT_END
  };
/* CPU test operations */
static int cpu_init(void);
static void cpu_print_mode(void);
static void cpu_report_cumulative(sb_stat_t *);
static int cpu_done(void);
static int cpu_thread_run(int);

sb_test_t cpu_test =
  {
    .sname = "cpu",
    .lname = "CPU performance test",
    .ops = {
      .init = cpu_init,
      .print_mode = cpu_print_mode,
      .report_cumulative = cpu_report_cumulative,
      .event_run = cpu_thread_run,
      .done = cpu_done
    },

  };

void register_cpu(void) {
  for (int i = 0; cpu_options[i].type != SB_ARG_TYPE_NULL; ++i)
    sb_list_add(cpu_test.options, cpu_options[i]);
  sb_list_add(tests, cpu_test);
}


int cpu_init(void) {
  printf("cpu test init\n");
  return 0;
}

void cpu_print_mode(void) {

}

int cpu_thread_run(int c) {
  printf("cpu run on core %d\n", disp_get_core_id());
}


void cpu_report_cumulative(sb_stat_t *stat) {
  printf("CPU speed:");
  printf("    events per second: %8.2f",
         stat->events / stat->time_interval);

  sb_report_cumulative(stat);
}

int cpu_done(void) {
  printf("cpu test done\n");
  return 0;
}
