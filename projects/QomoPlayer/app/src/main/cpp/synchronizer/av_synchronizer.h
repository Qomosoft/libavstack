//
// Created by Robin on 2024/3/20.
//
#pragma once
#include "video_decoder.h"

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

static const float kMinBufferedDuration = 0.5f;
static const float kMaxBufferedDuration = 0.8f;
static const float kSyncMaxTimeDiff = 0.05f;

class AVSynchronizer {
 public:
  AVSynchronizer();
  ~AVSynchronizer();
  bool Init(const std::string &url, int decode_type);
  void Finalize();
  void Start();
  void Stop();

 private:
  void DecodeFrames();
  void DecoderLoop();

 private:
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<std::thread> decoder_thread_;
  bool is_on_decoding_;
  std::mutex mutex_;
  std::condition_variable cv_;

  std::queue<AVFrame *> audio_frame_buffer_;
  std::queue<AVFrame *> video_frame_buffer_;

  float min_buffered_duration_;
  float max_buffered_duration_;
  float sync_max_time_diff_;
  float buffered_duration_;
};