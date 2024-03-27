//
// Created by Robin on 2024/3/20.
//
#pragma once
#include "video_decoder.h"
#include "frame_callback.h"

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class AVSynchronizer : public FrameCallback {
 public:
  AVSynchronizer();
  ~AVSynchronizer();
  bool Init(const std::string &url, int decode_type);
  void Finalize();
  void Start();
  void Stop();

  int OnFrameNeeded(AVFrame **frame, AVMediaType type) override;

  int channels() const;
  int sample_rate() const;

 private:
  int DecodeFrames();
  void DecoderLoop();

 private:
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<std::thread> decoder_thread_;
  bool is_on_decoding_;
  bool is_first_frame_ = true;
  bool is_eof_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;

  std::shared_ptr<std::queue<AVFrame *>> audio_frame_buffer_;
  std::shared_ptr<std::queue<AVFrame *>> video_frame_buffer_;

  float min_buffered_duration_;
  float max_buffered_duration_;
  float sync_max_time_diff_;
  float buffered_duration_;
  float current_playback_position_ = 0;
  float current_audio_frame_duration_ = 0;
};