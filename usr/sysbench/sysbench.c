//
// Created by SNAKE on 9/21/19.
//

#include "sysbench.h"
/*

 */
struct thread_barrier barrier;

volatile int count = 0;
spinlock_t spinlock;
int thread(void *arg);

int main(int argc, char *argv[]) {
  sb_option_init();
  sb_test_init();
  sb_option_parse(argc, argv);
  sb_option_print();
  int thread_num = -1;

  if (argc == 2) {
    thread_num = atoi(argv[1]);
  } else {
    printf("usage: ddd [thread_num]\n");
    return 1;
  }

  spinlock = 0;
  thread_barrier_init(&barrier, thread_num, 0, 0);
  struct thread **pids = (struct thread **) malloc(sizeof(struct thread *) * thread_num);
  for (int i = 0; i < thread_num; ++i) {
    pids[i] = thread_create(thread, 0);
  }
  printf("create done\n");

  for (int i = 0; i < thread_num; ++i) {
    thread_join(pids[i], 0);
  }
}

int thread(void *arg) {
  acquire_spinlock(&spinlock);
  ++count;
  release_spinlock(&spinlock);
  thread_barrier_wait(&barrier);
  printf("%d ",count);
  return count;
}