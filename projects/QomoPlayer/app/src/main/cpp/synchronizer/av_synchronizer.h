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

class AVSynchronizer {
 public:
  AVSynchronizer();
  ~AVSynchronizer();
  bool Init(const std::string &url, int decode_type);
  void Finalize();
  void Start();
  void Stop();

 private:
  void DecoderLoop();

 private:
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<std::thread> decoder_thread_;
  bool is_on_decoding_;
  std::mutex mutex_;
  std::condition_variable cv_;
};