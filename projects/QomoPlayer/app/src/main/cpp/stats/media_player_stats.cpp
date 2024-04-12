//
// Created by robin on 2024/4/12.
//
#define LOG_TAG "Stats"

#include "media_player_stats.h"
#include "logging.h"

#include <sstream>

TimePoint Stats::Now() {
  return Clock::now();
}

MilliSeconds Duration(const TimePoint &start_tp, const TimePoint &end_tp) {
  return std::chrono::duration_cast<MilliSeconds>(end_tp - start_tp);
}

std::string Stats::ToString() {
  std::stringstream ss;
  MilliSeconds a_first_frame = Duration(on_init_time_pt, first_audio_frame_rendered_time_pt);
  MilliSeconds v_first_frame = Duration(on_init_time_pt, first_video_frame_rendered_time_pt);
  MilliSeconds a_first_pkt = Duration(on_init_time_pt, recv_first_audio_pkt_time_pt);
  MilliSeconds v_first_pkt = Duration(on_init_time_pt, recv_first_video_pkt_time_pt);
  MilliSeconds engine_create_elapsed = Duration(on_prepared_start_time_pt, on_prepared_end_time_pt);

  ss << "\n{"
        "\n\ta_first_frame:" << a_first_frame.count() << ","
        "\n\tv_first_frame:" << v_first_frame.count() << ","
        "\n\ta_first_pkt:" << a_first_pkt.count() << ","
        "\n\tv_first_pkt:" << v_first_pkt.count() << ","
        "\n\tengine_create_elapsed:" << engine_create_elapsed.count() << ","
        "\n}";

  return ss.str();
}

TimePoint Stats::on_init_time_pt;
TimePoint Stats::on_prepared_start_time_pt;
TimePoint Stats::on_prepared_end_time_pt;
TimePoint Stats::recv_first_audio_pkt_time_pt;
TimePoint Stats::recv_first_video_pkt_time_pt;
TimePoint Stats::first_audio_decoded_time_pt;
TimePoint Stats::first_video_decoded_time_pt;
TimePoint Stats::first_video_frame_rendered_time_pt;
TimePoint Stats::first_audio_frame_rendered_time_pt;
