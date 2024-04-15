//
// Created by Robin on 2024/3/26.
//

#define LOG_TAG "VideoRenderer"

extern "C" {
#include "libavutil/imgutils.h"
}

#include "video_renderer.h"
#include "logging.h"
#include "block_profiler.h"
#include "frame.h"

static const AVPixelFormat kOutPixFmt = AV_PIX_FMT_RGBA;

VideoRenderer::VideoRenderer(FrameCallback *callback) : callback_(callback) {}

VideoRenderer::~VideoRenderer() { }

int VideoRenderer::Init() {
  egl_renderer_ = std::make_unique<EglRenderer>();
  egl_renderer_->Initialize();
  return 0;
}

int VideoRenderer::SetWindow(ANativeWindow *window) {
  egl_renderer_->SetWindow(window);
  return 0;
}

int VideoRenderer::SetWindowSize(int width, int height) {
//  width_ = width;
//  height_ = height;
  return 0;
}

int VideoRenderer::Start() {
  egl_renderer_->Start();
  return 0;
}

void VideoRenderer::OnDraw() {
//  LOGI("enter");
  Frame *frame = nullptr;
  int ret = callback_->OnFrameNeeded(&frame, AVMEDIA_TYPE_VIDEO);
  if (!frame) return;
//  LOGI("draw frame %ld\n", frame->pkt_pos);
  egl_renderer_->DrawRgb(frame, frame->GetWidth(), frame->GetHeight());
}
