//
// Created by Robin on 2024/3/18.
//
#pragma once

#include <list>
#include <string>
#include "media_frame.h"

using MediaFramePtr = std::shared_ptr<MediaFrame>;

class VideoDecoder {
 public:
  VideoDecoder() = default;
  virtual ~VideoDecoder() = default;
  virtual int Init(const std::string &uri) = 0;
  virtual int Finalize() = 0;
  virtual int DecodeFrames(float duration, std::list<MediaFramePtr> frames) = 0;
};