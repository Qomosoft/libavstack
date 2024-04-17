//
// Created by Robin on 2024/3/18.
//
#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
}

#include <list>
#include <string>
#include <queue>
#include <memory>

#include "frame.h"
#include "circular_buffer.hpp"

using FrameQueuePtr = std::shared_ptr<circular_buffer<Frame>>;

class VideoDecoder {
 public:
  VideoDecoder() = default;
  virtual ~VideoDecoder() = default;
  virtual int Init(const std::string &uri) = 0;
  virtual int Finalize() = 0;
  virtual int DecodeFrames(float duration, float *decoded_duration, FrameQueuePtr audio_q, FrameQueuePtr video_q) = 0;
  virtual int Seek(float seconds) = 0;

  virtual float GetAudioTimeUnit() const = 0;
  virtual float GetVideoTimeUnit() const = 0;

  virtual int channels() const = 0;
  virtual int sample_rate() const = 0;
  virtual int sample_fmt() const = 0;
};