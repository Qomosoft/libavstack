//
// Created by robin on 2024/4/12.
//

#pragma once
#include <string>

class BlockProfiler {
 public:
  using Clock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using MilliSeconds = std::chrono::milliseconds;

  BlockProfiler(const std::string &msg);
  ~BlockProfiler();

 private:
  std::string msg_;
  TimePoint start_time_point_;
};