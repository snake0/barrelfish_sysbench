/** \file
 *  \brief Example application using threads - a more complex 
 *         example using synchronisation
 */

/*
 * Copyright (c) 2010, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#include <stdio.h>
#include <string.h>

#include <barrelfish/barrelfish.h>

#include <barrelfish/threads.h>
#include <barrelfish_kpi/spinlocks_arch.h>

// data to pass to each thread
struct my_thread_data {
  int t_num;
  struct thread *t_id;
  struct thread_mutex *mutex;
  int *start_cntr;
  struct thread_sem *sem;
  int ret;
};
volatile int common = 0;
spinlock_t spinlock;

// the code that each thread runs
static int my_thread(void *data) {
  acquire_spinlock(&spinlock);
  ++common;
  release_spinlock(&spinlock);
}


int main(int argc, char *argv[]) {
  errval_t err;
  int num_threads = 0;

  // # of threads to start
  if (argc == 2) {
    num_threads = atoi(argv[1]);
    debug_printf("starting %d threads\n", num_threads);
  } else {
    printf("usage %s num_threads\n", argv[0]);
    return EXIT_FAILURE;
  }
  struct thread **p = (struct thread **) malloc(num_threads * sizeof(struct thread *));

  // start threads
  for (int i = 0; i < num_threads; i++) {
    p[i] = thread_create(my_thread, NULL);
    debug_printf("started thread %d\n", i);
  }

  for (int i = 0; i < num_threads; i++) {
    thread_join(my_thread, NULL);
    debug_printf("started thread %d\n", i);
  }
  debug_printf("finished.\n");

  free(p);

  return EXIT_SUCCESS;
}

