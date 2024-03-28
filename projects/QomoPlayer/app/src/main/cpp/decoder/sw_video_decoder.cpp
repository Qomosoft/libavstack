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
    LOGE("Could not open data source: %s, return %d\n", uri.c_str(), ret);
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
  av_dump_format(fmt_ctx_, 0, uri.c_str(), 0);

  packet_ = av_packet_alloc();
  frame_ = av_frame_alloc();
  if (!packet_) {
    LOGE("alloc packet failed\n");
  }

  return 0;
}

int SWVideoDecoder::Finalize() {
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

//    if (is_eof) {
//      ret = DecodeFrame(audio_dec_ctx_, nullptr, frames, frame_, &decoded_duration);
//      ret = DecodeFrame(video_dec_ctx_, nullptr, frames, frame_, &decoded_duration);
//      break;
//    }

    ret = DecodeFrame(dec, packet_, frames, frame_, &decoded_duration);
  }


//    avcodec_send_packet(dec, packet_);
//
//    while (true) {
//      ret = avcodec_receive_frame(dec, frame_);
//      if (ret < 0) {
//        if (ret == AVERROR_EOF) {
//          LOGI("Decode return AVERROR_EOF\n");
//          ret = 0;
//        } else if (ret == AVERROR(EAGAIN)) {
////        LOGI("Decode return EAGAIN\n");
//          ret = 0;
//        } else {
//          LOGE("Decode failed, return %d\n", ret);
//          return ret;
//        }
//        break;
//      }
//
//      if (packet_->stream_index == audio_stream_index_) {
////        LOGI("receive audio frame pkt_pos=%d, pkt_duration=%d", frame_->pkt_pos, frame_->pkt_duration);
//        decoded_duration += frame_->pkt_duration / 1000000.0f;
//      } else if (packet_->stream_index == video_stream_index_) {
////        LOGI("receive video frame pict_type=%d", frame_->pict_type);
//      }
//
//      AVFrame *frame = av_frame_clone(frame_);
//      frames->emplace_back(frame);
//      av_packet_unref(packet_);
//    }
//  }

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
                                const AVPacket *pkt,
                                std::list<AVFrame *> *frames,
                                AVFrame *frame,
                                float *decoded_duration) {
  int ret = 0;
  ret = avcodec_send_packet(dec, pkt);
  if (ret < 0) {
    LOGE("avcodec_send_packet failed %s", av_err2str(ret));
    return ret;
  }

  while (ret >= 0) {
    ret = avcodec_receive_frame(dec, frame);
    if (ret < 0) {
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
        return 0;
      }

      LOGE("avcodec_receive_frame failed %s", av_err2str(ret));
      return ret;
    }

    if (dec->codec->type == AVMEDIA_TYPE_AUDIO) {
      *decoded_duration += frame->pkt_duration / 1000000.0f;
    }

//    AVFrame *frame = av_frame_clone(frame_);
    frames->emplace_back(av_frame_clone(frame));
    av_frame_unref(frame);
    av_packet_unref(packet_);
  }

  return ret;
}

int SWVideoDecoder::channels() const {
  return channels_;
}

int SWVideoDecoder::sample_rate() const {
  return sample_rate_;
}

