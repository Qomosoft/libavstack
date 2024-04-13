//
// Created by Robin on 2024/3/26.
//

#pragma once

#include "frame.h"
extern "C" {
#include "libavutil/frame.h"
}

class FrameCallback {
 public:
  virtual int OnFrameNeeded(Frame **frame, AVMediaType type) = 0;
};