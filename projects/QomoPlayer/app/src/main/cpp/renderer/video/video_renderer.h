//
// Created by Robin on 2024/3/26.
//

#pragma once

#include "frame_callback.h"

#include <memory>

class VideoRenderer {
 public:
  VideoRenderer(FrameCallback *callback);
  ~VideoRenderer();

  void OnDraw();

 private:
  FrameCallback *callback_;
};