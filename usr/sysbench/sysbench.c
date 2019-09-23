//
// Created by SNAKE on 9/21/19.
//

#include "sysbench.h"

/* Barrier to ensure we start the benchmark run when all workers are ready */
static struct thread_barrier worker_barrier;

/* Barrier to signal reporting threads */
//static struct thread_barrier report_barrier;

static spinlock_t spinlock;

static struct thread **workers;

static int worker_routine(void *arg);
int event_run(long thread_id);
static int worker_started_callback(void *arg);

static int start_workers(void);
static int init_sync(void);
static int finish_up(void);
static int stop_workers(void);

static void report_cumulative(void);

int main(int argc, char *argv[]) {
  sb_option_init();
  sb_test_init();

  ASSERT(sb_option_parse(argc, argv));
  sb_option_print();

  ASSERT(init_sync());
  ASSERT(init_timers());

  ASSERT(start_workers());
  ASSERT(stop_workers());

  report_cumulative();
  ASSERT(finish_up());
}

int worker_routine(void *arg) {
  long thread_id = (long) arg;
  int rc = 0;

  if (current_test->ops.thread_init != NULL && current_test->ops.thread_init(thread_id) != 0) {
    printf("Worker thread (#%ld) failed to initialize!", thread_id);
    sb_globals.error = 1;
    thread_barrier_wait(&worker_barrier);
    return SB_INIT_FAIL;
  }

  if (thread_barrier_wait(&worker_barrier) < 0)
    return SB_INIT_FAIL;

  rc = event_run(thread_id); // run the event thread

  if (rc != 0)
    sb_globals.error = 1;
  else if (current_test->ops.thread_done != NULL)
    current_test->ops.thread_done(thread_id);

  return SB_OK;
}


/* function to run all events of the test */
int event_run(long thread_id) {
  int rc = 0;
  for (;;) {
    sb_timer_start(&timers[thread_id]);
    rc = current_test->ops.event_run(0);
    sb_timer_stop(&timers[thread_id]);

    printf("%d ", disp_get_core_id());

//    acquire_spinlock(&spinlock);
    if (timers[thread_id].events == sb_globals.events)
      break;
//    release_spinlock(&spinlock);
  }

  return rc;
}

int worker_started_callback(void *arg) {
  if (sb_globals.error)
    return 1;
  (void) arg;
  printf("Threads started!\n");

  return 0;
}

int start_workers(void) {
  workers = (struct thread **) malloc(sb_globals.threads * sizeof(struct thread *));
  if (!workers)
    return SB_MALLOC_FAIL;

  for (long i = 0; i < sb_globals.threads; ++i) {
    if (!(workers[i] = thread_create(worker_routine, (void *) i))) {
      finish_up();
      return SB_THREAD_CREATE_FAIL;
    }
  }
  return SB_OK;
}

int init_sync(void) {
  int barrier_threads = 1 /* main thread */
                        + sb_globals.threads;
  if (thread_barrier_init(
    &worker_barrier,
    barrier_threads,
    worker_started_callback, NULL))
    return SB_INIT_FAIL;

  init_spinlock(&spinlock);
  return SB_OK;
}

int stop_workers(void) {
  if (thread_barrier_wait(&worker_barrier) < 0)
    return SB_INIT_FAIL;

  for (int i = 0; i < sb_globals.threads; ++i)
    if (thread_join(workers[i], 0)) {
      finish_up();
      return SB_INIT_FAIL;
    }

  return SB_OK;
}
sb_timer_t t;

void report_cumulative(void) {
  sb_stat_t stat;

  for (size_t i = 0; i < sb_globals.threads; i++)
    sb_timer_checkpoint(&timers[i], &timers_copy[i]);

  sb_timer_init(&t);

  for (size_t i = 0; i < sb_globals.threads; i++)
    t = sb_timer_merge(&t, &timers_copy[i]);

  /* Calculate aggregate latency values */
  stat.latency_min = NS2SEC(sb_timer_min(&t));
  stat.latency_max = NS2SEC(sb_timer_max(&t));
  stat.latency_avg = NS2SEC(sb_timer_avg(&t));
  stat.latency_sum = NS2SEC(sb_timer_sum(&t));

//  assert(t.events == sb_globals.events);

  if (current_test && current_test->ops.report_cumulative)
    current_test->ops.report_cumulative(&stat);
  else
    sb_report_cumulative(&stat);
}

void sb_report_cumulative(sb_stat_t *stat) {
  const unsigned int nthreads = sb_globals.threads;

  printf("\n");
  printf("Throughput:\n");
  printf("    events/s (eps):                      %.4f\n",
         sb_globals.events / stat->latency_sum);
  printf("    time elapsed:                        %.4fs\n",
         stat->latency_sum);
  printf("    total number of events:              %d\n",
         sb_globals.events);

  printf("\n");

  printf("Latency (ms):\n");
  printf("         min: %39.2f\n",
         SEC2MS(stat->latency_min));
  printf("         avg: %39.2f\n",
         SEC2MS(stat->latency_avg));
  printf("         max: %39.2f\n",
         SEC2MS(stat->latency_max));

  printf("         sum: %39.2f\n",
         SEC2MS(stat->latency_sum));
  printf("\n");

  /* Aggregate temporary timers copy */

  /* Calculate and print events distribution by threads */
  const double events_avg = (double) t.events / nthreads;
  const double time_avg = stat->latency_sum / nthreads;

  double events_stddev = 0;
  double time_stddev = 0;

  for (unsigned i = 0; i < nthreads; i++) {
    double diff = fabs(events_avg - timers_copy[i].events);
    events_stddev += diff * diff;

    diff = fabs(time_avg - NS2SEC(sb_timer_sum(&timers_copy[i])));
    time_stddev += diff * diff;
  }
  events_stddev = sqrt(events_stddev / nthreads);
  time_stddev = sqrt(time_stddev / nthreads);

  printf("Threads fairness:\n");
  printf("    events (avg/stddev):           %.4f/%3.2f\n",
         events_avg, events_stddev);
  printf("    execution time (avg/stddev):   %.4f/%3.2f\n",
         time_avg, time_stddev);
  printf("\n");

}

int finish_up(void) {
  free(workers);
  free(timers);
  free(timers_copy);

  if (current_test->ops.cleanup != NULL && current_test->ops.cleanup() != 0)
    return 1;

  if (current_test->ops.done != NULL)
    (*current_test->ops.done)();
  return sb_globals.error != 0;
}