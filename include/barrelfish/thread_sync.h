/**
 * \file
 * \brief Thread synchronization definitions.
 */

/*
 * Copyright (c) 2011, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#ifndef LIBBARRELFISH_THREAD_SYNC_H
#define LIBBARRELFISH_THREAD_SYNC_H

#include <stdint.h>
#include <limits.h> // for INT_MAX

#include <barrelfish_kpi/spinlocks_arch.h>

/// A thread of execution
struct thread;

struct thread_mutex {
  volatile int locked;
  struct thread *queue;
  spinlock_t lock;
  struct thread *holder;
};
#ifndef __cplusplus
#       define THREAD_MUTEX_INITIALIZER \
    { .locked = 0, .queue = NULL, .lock = 0, .holder = NULL }
#else
#       define THREAD_MUTEX_INITIALIZER                                \
    { 0, (struct thread *)NULL, 0, (struct thread *)NULL }
#endif

struct thread_cond {
  struct thread *queue;
  spinlock_t lock;
};
#ifndef __cplusplus
#       define THREAD_COND_INITIALIZER \
    { .queue = NULL, .lock = 0 }
#else
#       define THREAD_COND_INITIALIZER \
    { (struct thread *)NULL, 0 }
#endif

struct thread_sem {
  volatile unsigned int value;
  struct thread *queue;
  spinlock_t lock;
};
#ifndef __cplusplus
#       define THREAD_SEM_INITIALIZER \
    { .value = 0, .queue = NULL, .lock = 0 }
#else
#       define THREAD_SEM_INITIALIZER \
    { 0, (struct thread *)NULL, 0 }
#endif

typedef int thread_once_t;
#define THREAD_ONCE_INIT INT_MAX

struct thread_barrier {
  unsigned int count;
  unsigned int init_count;
  unsigned int serial;
  struct thread_mutex mutex;
  struct thread_cond cond;
  int (*back_func)(void *);
  void *data;
  int error;
};


/*
#ifndef __cplusplus
#		define THREAD_BARRIER_INITIALIZER \
  { .count = 0, .init_count = 0,.serial = 0, .mutex = NULL, \
    .barrier = NULL, .reset = NULL }
#else
#		define THREAD_BARRIER_INITIALIZER \
  { 0, 0, (struct thread_sem *) NULL, \
    (struct thread_sem *) NULL, (struct thread_sem *) NULL }
#endif
*/
#endif
