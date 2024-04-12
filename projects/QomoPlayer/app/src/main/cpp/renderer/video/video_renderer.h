//
// Created by Robin on 2024/3/26.
//

#pragma once

#include "frame_callback.h"
#include "egl_renderer.h"

#include <memory>
#include <vector>

class VideoRenderer {
 public:
  VideoRenderer(FrameCallback *callback);
  ~VideoRenderer();

  int Init();
  int SetWindow(ANativeWindow *window);
  int SetWindowSize(int width, int height);
  int Start();

  void OnDraw();

 private:
  FrameCallback *callback_;
  std::unique_ptr<EglRenderer> egl_renderer_;
  SwsContext *sws_context_ = nullptr;
  std::vector<uint8_t> rgb_data_;
};