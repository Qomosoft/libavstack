//
// Created by robin on 2024/4/12.
//

#pragma once
#include <chrono>
#include <string>

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;
using MilliSeconds = std::chrono::milliseconds;

struct Stats {
  static TimePoint Now();
  static std::string ToString();

  static TimePoint on_init_time_pt;
  static TimePoint on_prepared_start_time_pt;
  static TimePoint on_prepared_end_time_pt;
  static TimePoint recv_first_audio_pkt_time_pt;
  static TimePoint recv_first_video_pkt_time_pt;
  static TimePoint first_audio_decoded_time_pt;
  static TimePoint first_video_decoded_time_pt;
  static TimePoint first_audio_frame_rendered_time_pt;
  static TimePoint first_video_frame_rendered_time_pt;
};