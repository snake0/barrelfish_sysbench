//
// Created by SNAKE on 9/21/19.
//

#include "sb_timer.h"
#include "sb_util.h"
#include "sysbench.h"
#include <barrelfish/barrelfish.h>
/* Some functions for simple time operations */

/* initialize timer */

void sb_timer_init(sb_timer_t *t) {
  SB_COMPILE_TIME_ASSERT(sizeof(sb_timer_t) % CK_MD_CACHELINE == 0);

  memset(&t->time_start, 0, sizeof(struct timespec));
  memset(&t->time_end, 0, sizeof(struct timespec));

  init_spinlock(&t->lock);

  sb_timer_reset(t);
}

/* Reset timer counters, but leave the current state intact */

void sb_timer_reset(sb_timer_t *t) {
  t->min_time = UINT64_MAX;
  t->max_time = 0;
  t->sum_time = 0;
  t->events = 0;
  t->queue_time = 0;
}

/* Clone a timer */

void sb_timer_copy(sb_timer_t *to, sb_timer_t *from) {
  memcpy(to, from, sizeof(sb_timer_t));

  init_spinlock(&to->lock);
}

/* check whether the timer is running */

bool sb_timer_running(sb_timer_t *t) {
  return TIMESPEC_DIFF(t->time_start, t->time_end) > 0;
}

/*
  get time elapsed since the previous call to sb_timer_current() for the
  specified timer without stopping it.  The first call returns time elapsed
  since the timer was started.
*/

uint64_t sb_timer_current(sb_timer_t *t) {
  struct timespec tmp;
  uint64_t res;

  SB_GETTIME(&tmp);
  res = TIMESPEC_DIFF(tmp, t->time_start);
  t->time_start = tmp;

  return res;
}

/*
  Atomically reset a given timer after copying its state into the timer pointed
  to by 'old'.
*/

void sb_timer_checkpoint(sb_timer_t *t, sb_timer_t *old) {
  acquire_spinlock(&t->lock);

  memcpy(old, t, sizeof(*old));
  old->lock = false;

  sb_timer_reset(t);

  release_spinlock(&t->lock);
}

/* get average time per event */


uint64_t sb_timer_avg(sb_timer_t *t) {
  if (t->events == 0)
    return 0; /* return zero if there were no events */
  return (t->sum_time / t->events);
}


/* get total time for all events */


uint64_t sb_timer_sum(sb_timer_t *t) {
  return t->sum_time;
}


/* get minimum time */


uint64_t sb_timer_min(sb_timer_t *t) {
  if (t->events == 0)
    return 0;
  return t->min_time;
}


/* get maximum time */


uint64_t sb_timer_max(sb_timer_t *t) {
  return t->max_time;
}


/* sum data from several timers. used in summing data from multiple threads */


sb_timer_t sb_timer_merge(sb_timer_t *t1, sb_timer_t *t2) {
  sb_timer_t t;

  /* Initialize to avoid warnings */
  memset(&t, 0, sizeof(sb_timer_t));

  t.sum_time = t1->sum_time + t2->sum_time;
  t.events = t1->events + t2->events;

  if (t1->max_time > t2->max_time)
    t.max_time = t1->max_time;
  else
    t.max_time = t2->max_time;

  if (t1->min_time < t2->min_time)
    t.min_time = t1->min_time;
  else
    t.min_time = t2->min_time;

  return t;
}


int init_timers(void) {

/* init per thread timers */
  size_t size = sizeof(sb_timer_t) * sb_globals.threads;

  timers = (sb_timer_t*)malloc(size);
  timers_copy = (sb_timer_t*)malloc(size);

  if (timers == NULL || timers_copy == NULL)
    return SB_INIT_FAIL;

  memset(timers, 0, size);
  memset(timers_copy, 0, size);
  for (unsigned i = 0; i < sb_globals.threads; i++)
    sb_timer_init(&timers[i]);

  return SB_OK;
}
