//
// Created by Robin on 2024/3/20.
//
#pragma once
#include "video_decoder.h"
#include "frame_callback.h"
#include "frame.h"

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
  int Seek(float seconds);

  int OnFrameNeeded(Frame **frame, AVMediaType type) override;

  int channels() const;
  int sample_rate() const;
  int sample_fmt() const;

 private:
  int DecodeFrames();
  void DecoderLoop();

 private:
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<std::thread> decoder_thread_;
  bool is_on_decoding_;
  bool is_first_frame_ = true;
  bool is_eof_ = false;
  bool is_seeking_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;

  std::shared_ptr<std::queue<Frame *>> audio_frame_buffers_;
  std::shared_ptr<std::queue<Frame *>> video_frame_buffers_;

  float min_buffered_duration_;
  float max_buffered_duration_;
  float sync_max_time_diff_;
  float buffered_duration_;
  float current_playback_position_ = 0;
  float current_audio_frame_duration_ = 0;
  float seek_position_ = 0;
};