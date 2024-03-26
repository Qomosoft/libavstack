//
// Created by Robin on 2024/3/18.
//
#pragma once
#define LOG_TAG "SWVideoDecoder"

extern "C" {
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
}

#include "video_decoder.h"

#include <list>

using AVPacketPtr = std::shared_ptr<AVPacket>;

class SWVideoDecoder : public VideoDecoder {
 public:
  SWVideoDecoder();
  ~SWVideoDecoder();
  int Init(const std::string &uri) override;
  int Finalize() override;
  int DecodeFrames(float duration, std::list<AVFrame *> *frames) override;

 private:
  int OpenCodecContext(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, AVMediaType type);

 private:
  AVFormatContext *fmt_ctx_;
  AVCodecContext  *audio_dec_ctx_;
  AVCodecContext  *video_dec_ctx_;
  AVPacket *packet_;
  int video_stream_index_;
  int audio_stream_index_;
};