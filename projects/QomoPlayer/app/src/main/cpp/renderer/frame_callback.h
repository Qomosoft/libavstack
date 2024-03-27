//
// Created by Robin on 2024/3/26.
//

#pragma once

extern "C" {
#include "libavutil/frame.h"
}

class FrameCallback {
 public:
  virtual int OnFrameNeeded(AVFrame **frame, AVMediaType type) = 0;
};