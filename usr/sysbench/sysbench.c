//
// Created by SNAKE on 9/21/19.
//

#include "sysbench.h"

static int run_test();

int main(int argc, char *argv[]) {
  sb_option_init();
  sb_test_init();
  sb_option_parse(argc, argv);
  sb_option_print();
  run_test();
}

struct thread_barrier barrier;

int run_test() {
  thread_barrier_init(&barrier, 100, 0, 0);
}