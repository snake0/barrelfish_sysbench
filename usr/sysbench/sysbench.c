//
// Created by SNAKE on 9/21/19.
//

#include "sysbench.h"


int main(int argc, char *argv[]) {
  sb_option_init();
  sb_test_init();
  (sb_option_parse(argc, argv));
  sb_option_print();
}