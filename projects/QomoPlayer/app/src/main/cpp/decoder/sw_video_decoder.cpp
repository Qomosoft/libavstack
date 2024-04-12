//
// Created by Robin on 2024/3/18.
//
#define LOG_TAG "SWVideoDecoder"

#include "sw_video_decoder.h"
#include "logging.h"

#include <sstream>

SWVideoDecoder::SWVideoDecoder()
    : fmt_ctx_(nullptr),
      audio_dec_ctx_(nullptr),
      video_dec_ctx_( nullptr),
      audio_stream_index_(-1),
      video_stream_index_(-1),
      channels_(0),
      sample_rate_(0)
      {}

SWVideoDecoder::~SWVideoDecoder() {}
int SWVideoDecoder::Init(const std::string &uri) {
  std::stringstream ss;
  ss << "\nlibavcodec:" << AV_STRINGIFY(LIBAVCODEC_VERSION) <<
     "\nlibavformat:" << AV_STRINGIFY(LIBAVFORMAT_VERSION) <<
     "\nlibavutil:" << AV_STRINGIFY(LIBAVUTIL_VERSION) <<
     "\nlibavfilter:" << AV_STRINGIFY(LIBAVFILTER_VERSION) <<
     "\nlibavswresample:" << AV_STRINGIFY(LIBSWRESAMPLE_VERSION) <<
     "\nlibavswscale:" << AV_STRINGIFY(LIBSWSCALE_VERSION) <<
     "\navcodec_configure:" << avcodec_configuration() <<
     "\navcodec_license:" << avcodec_license();
  std::string version = ss.str();

  av_log_set_level(AV_LOG_INFO);
  av_log_set_callback(ff_log_callback);

  LOGI("ffmpeg version: %s", version.c_str());
  LOGI("uri=%s", uri.c_str());
  int ret = avformat_open_input(&fmt_ctx_, uri.c_str(), nullptr, nullptr);
  if (ret < 0) {
    LOGE("Could not open data source: %s, return %s\n", uri.c_str(), av_err2str(ret));
    return ret;
  }

  ret = avformat_find_stream_info(fmt_ctx_, nullptr);
  if (ret < 0) {
    LOGE("Could not find stream information, return %d\n", ret);
    return ret;
  }

  ret = OpenCodecContext(&audio_stream_index_, &audio_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_AUDIO);
  if (ret != 0) {
    LOGE("Failed to open %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
  }

  ret = OpenCodecContext(&video_stream_index_, &video_dec_ctx_, fmt_ctx_, AVMEDIA_TYPE_VIDEO);
  if (ret != 0) {
    LOGE("Failed to open %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
  }
  channels_ = audio_dec_ctx_->channels;
  sample_rate_ = audio_dec_ctx_->sample_rate;
  sample_fmt_ = audio_dec_ctx_->sample_fmt;
  AVSampleFormat format = audio_dec_ctx_->sample_fmt;
  av_dump_format(fmt_ctx_, 0, uri.c_str(), 0);

  packet_ = av_packet_alloc();
  frame_ = av_frame_alloc();
  if (!packet_) {
    LOGE("alloc packet failed\n");
  }

  return 0;
}

int SWVideoDecoder::Finalize() {
  av_packet_free(&packet_);
  av_frame_free(&frame_);
  avcodec_free_context(&audio_dec_ctx_);
  avcodec_free_context(&video_dec_ctx_);
  avformat_close_input(&fmt_ctx_);

  return 0;
}

int SWVideoDecoder::DecodeFrames(float duration, std::list<AVFrame *> *frames) {
  int ret = 0;
  float decoded_duration = 0.0f;
  AVCodecContext *dec = nullptr;
  bool is_eof = false;
  while (decoded_duration <= duration) {
    ret = av_read_frame(fmt_ctx_, packet_);
    if (ret < 0) {
      if (ret == AVERROR_EOF) {
        LOGI("av_read_frame return %s", av_err2str(ret));
        is_eof = true;
        DecodeFrame(audio_dec_ctx_, nullptr, frames, frame_, &decoded_duration);
        DecodeFrame(video_dec_ctx_, nullptr, frames, frame_, &decoded_duration);
        break;
      } else {
        LOGE("av_read_frame return %s\n", av_err2str(ret));
        break;
      }
    }

    if (packet_->stream_index == audio_stream_index_) {
      dec = audio_dec_ctx_;
    } else if (packet_->stream_index == video_stream_index_) {
      dec = video_dec_ctx_;
    }

    ret = DecodeFrame(dec, packet_, frames, frame_, &decoded_duration);
    av_packet_unref(packet_);
  }


  return is_eof ? AVERROR_EOF : ret;
}

int SWVideoDecoder::OpenCodecContext(int *stream_idx,
                                     AVCodecContext **dec_ctx,
                                     AVFormatContext *fmt_ctx,
                                     AVMediaType type) {
  int ret, stream_index;
  AVStream *stream = nullptr;
  AVCodec *codec = nullptr;
  AVDictionary *opts = nullptr;

  ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
  if (ret < 0) {
    LOGE("Could not find %s stream", av_get_media_type_string(type));
    return ret;
  }

  stream_index = ret;
  stream = fmt_ctx_->streams[stream_index];

  codec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!codec) {
    LOGE("Failed to find %s codec\n", av_get_media_type_string(type));
    return AVERROR(EINVAL);
  }

  *dec_ctx = avcodec_alloc_context3(codec);
  if (!*dec_ctx) {
    LOGE("Failed to allocate the %s codec context\n");
    return AVERROR(ENOMEM);
  }

  ret = avcodec_parameters_to_context(*dec_ctx, stream->codecpar);
  if (ret < 0) {
    LOGE("Failed to copy %s codec parameters to decoder context\n");
    return ret;
  }

  ret = avcodec_open2(*dec_ctx, codec, &opts);
  if (ret < 0) {
    LOGE("Failed to open %s codec\n", av_get_media_type_string(type));
    return ret;
  }

  *stream_idx = stream_index;

  return 0;
}

int SWVideoDecoder::DecodeFrame(AVCodecContext *dec,
                                AVPacket *pkt,
                                std::list<AVFrame *> *frames,
                                AVFrame *frame,
                                float *decoded_duration) {
  int ret = avcodec_send_packet(dec, pkt);
  if (ret < 0) {
    LOGE("avcodec_send_packet failed %s", av_err2str(ret));
    return ret;
  }

  while (true) {
    ret = avcodec_receive_frame(dec, frame_);
    if (ret < 0) {
      av_frame_unref(frame_);
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
        return 0;
      }

      LOGE("avcodec_receive_frame failed %s", av_err2str(ret));
      return ret;
    }

    if (dec->codec->type == AVMEDIA_TYPE_AUDIO) {
      float time_unit = av_q2d(fmt_ctx_->streams[audio_stream_index_]->time_base);
      float duration_time = frame->pkt_duration * time_unit;
      *decoded_duration += duration_time;
      float pts_time = frame->pts * time_unit;
      float dts_time = frame->pkt_dts * time_unit;
//      LOGI("receive %s frame pkt_pos=%d, pkt_duration=%d, duration_time=%f, pts=%d, pts_time=%f, "
//           "dts=%d, dts_time=%f", av_get_media_type_string(dec->codec->type),
//           frame_->pkt_pos, frame_->pkt_duration, duration_time,
//           frame->pts, pts_time, frame->pkt_dts, dts_time);
    } else {
      float time_unit = av_q2d(fmt_ctx_->streams[video_stream_index_]->time_base);
      float duration_time = frame->pkt_duration * time_unit;
      float pts_time = frame->pts * time_unit;
      float dts_time = frame->pkt_dts * time_unit;
//      LOGI("receive %s frame pkt_pos=%d, pkt_duration=%d, duration_time=%f, pts=%d, pts_time=%f, "
//           "dts=%d, dts_time=%f", av_get_media_type_string(dec->codec->type),
//           frame_->pkt_pos, frame_->pkt_duration, duration_time,
//           frame->pts, pts_time, frame->pkt_dts, dts_time);
    }

    frames->emplace_back(av_frame_clone(frame_));
    av_frame_unref(frame_);
  }

  return ret;
}

void SWVideoDecoder::SeekToPosition(float seconds) {
  int64_t seek_target = seconds * 1000000;
  int64_t seek_min = INT64_MIN;
  int64_t seek_max = INT64_MAX;
  int ret = avformat_seek_file(fmt_ctx_, -1, seek_min, seek_target, seek_max, 0);
  if (ret < 0) {
    LOGE("avformat_seek_file failed: %s", av_err2str(ret));
  } else {
    avcodec_flush_buffers(audio_dec_ctx_);
    avcodec_flush_buffers(video_dec_ctx_);
  }
}

int SWVideoDecoder::Seek(float seconds) {
  SeekToPosition(seconds);
  return 0;
}

float SWVideoDecoder::GetAudioTimeUnit() const {
  return av_q2d(fmt_ctx_->streams[audio_stream_index_]->time_base);
}

float SWVideoDecoder::GetVideoTimeUnit() const {
  return av_q2d(fmt_ctx_->streams[video_stream_index_]->time_base);
}

int SWVideoDecoder::channels() const {
  return channels_;
}

int SWVideoDecoder::sample_rate() const {
  return sample_rate_;
}
int SWVideoDecoder::sample_fmt() const {
  return sample_fmt_;
}
