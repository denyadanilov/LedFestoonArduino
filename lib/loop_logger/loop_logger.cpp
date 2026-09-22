#include "loop_logger.h"
#include "adapter.h"
#include <cstdio>

#define LOG_ITERATIONS_NUM 100000

unsigned long loop_iteration_num_ = 0;
unsigned long total_iteration_time_ = 0;
char log_buffer_[32] = {0};

bool try_log_loop_time(unsigned long time);

void log_loop_time(void (*command)()) {
  auto time = get_micros_from_start();
  command();
  try_log_loop_time(time);
}

void print_loop_time() {
  auto average_iteration_time = total_iteration_time_ / loop_iteration_num_;
  snprintf(log_buffer_, sizeof(log_buffer_), "Average iteration time: %lu",
           average_iteration_time);
  log_message(log_buffer_);
}

bool try_log_loop_time(unsigned long micros) {
  auto iteration_time = get_micros_from_start() - micros;
  loop_iteration_num_++;
  total_iteration_time_ += iteration_time;
  if (loop_iteration_num_ % LOG_ITERATIONS_NUM == 0) {
    print_loop_time();
    return true;
  }
  return false;
}