//
// Created by Robin on 2024/3/26.
//

#define LOG_TAG "VideoRenderer"

extern "C" {
#include "libavutil/imgutils.h"
}

#include "video_renderer.h"
#include "logging.h"

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
  AVFrame *frame = nullptr;
  int ret = callback_->OnFrameNeeded(&frame, AVMEDIA_TYPE_VIDEO);
  if (!frame) return;
//  LOGI("draw frame %ld\n", frame->pkt_pos);
  if (sws_context_ == nullptr) {
    sws_context_ = sws_getContext(frame->width, frame->height,
                                  static_cast<AVPixelFormat>(frame->format),
                                  frame->width, frame->height, kOutPixFmt,
                                  SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!sws_context_) {
      LOGE("sws_getContext failed\n");
      return;
    }

    int rgb_data_size = av_image_get_buffer_size(kOutPixFmt, frame->width, frame->height, 1);
    rgb_data_ = std::vector<uint8_t>(rgb_data_size, 0);
  }

  uint8_t *rgb_data_array = rgb_data_.data();
  AVFrame *rgb_frame = av_frame_alloc();
  av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, rgb_data_array,
                       kOutPixFmt, frame->width, frame->height, 1);
  ret = sws_scale(sws_context_, frame->data, frame->linesize, 0, frame->height, rgb_frame->data, rgb_frame->linesize);
  if (ret < 0) {
    LOGE("sws_scale failed %s", av_err2str(ret));
  }
  egl_renderer_->DrawRgb(rgb_data_.data(), frame->width, frame->height);
  av_frame_free(&frame);
  av_frame_free(&rgb_frame);
}
