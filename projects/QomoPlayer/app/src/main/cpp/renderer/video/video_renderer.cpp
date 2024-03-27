//
// Created by Robin on 2024/3/26.
//

#define LOG_TAG "VideoRenderer"

#include "video_renderer.h"
#include "logging.h"

VideoRenderer::VideoRenderer(FrameCallback *callback) : callback_(callback) {}

VideoRenderer::~VideoRenderer() {}

void VideoRenderer::OnDraw() {
  LOGI("enter");
  AVFrame *frame = nullptr;
  int ret = callback_->OnFrameNeeded(&frame, AVMEDIA_TYPE_VIDEO);
  if (!frame) return;
  //TODO: draw frame
  LOGI("draw frame %ld\n", frame->pkt_pos);
  av_frame_unref(frame);
}
