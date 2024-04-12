//
// Created by robin on 2024/4/12.
//
#define LOG_TAG "BlockProfiler"

#include "block_profiler.h"
#include "logging.h"

BlockProfiler::BlockProfiler(const std::string &msg) : msg_(msg), start_time_point_(Clock::now()) {}

BlockProfiler::~BlockProfiler() {
  auto elapsed = std::chrono::duration_cast<MilliSeconds>(Clock::now() - start_time_point_);
  LOGI("%s elapsed:%lld ms", msg_.c_str(), elapsed.count());
}
