//
// Created by Robin on 2024/3/18.
//
#pragma once

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
  int DecodeFrames(float duration,
                   float *decoded_duration,
                   FrameQueuePtr audio_q,
                   FrameQueuePtr video_q) override;
  int Seek(float seconds) override;

  float GetAudioTimeUnit() const override;
  float GetVideoTimeUnit() const override;

  int channels() const override;
  int sample_rate() const override;
  int sample_fmt() const override;

 private:
  int OpenCodecContext(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, AVMediaType type);
  int DecodeFrame(AVCodecContext *dec,
                  AVPacket *pkt,
                  FrameQueuePtr frame_q,
                  AVFrame *frame,
                  float *decoded_duration);
  void ProcessVideoFrame(AVFrame *src, float time_unit, Frame *dst, bool convert = false);
  void ProcessAudioFrame(AVFrame *src, float time_unit, Frame *dst);

  void SeekToPosition(float seconds);

 private:
  AVFormatContext *fmt_ctx_;
  AVCodecContext  *audio_dec_ctx_;
  AVCodecContext  *video_dec_ctx_;
  AVPacket *packet_;
  AVFrame *frame_;
  AVFrame *processed_video_frame_;
  int video_stream_index_;
  int audio_stream_index_;
  int channels_;
  int sample_rate_;
  int sample_fmt_;

  SwsContext *sws_context_ = nullptr;
  SwrContext *swr_context_ = nullptr;
};