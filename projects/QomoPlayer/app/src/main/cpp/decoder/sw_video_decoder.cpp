//
// Created by Robin on 2024/3/18.
//
#define LOG_TAG "SWVideoDecoder"

extern "C" {
#include "libavutil/imgutils.h"
}

#include "sw_video_decoder.h"
#include "logging.h"
#include "block_profiler.h"

#include <sstream>

static const AVPixelFormat kOutPixFmt = AV_PIX_FMT_RGBA;

static const int kOutSampleRate = 44100;
static const int kOutChannels = 2;
static const AVSampleFormat kOutSampleFmt = AV_SAMPLE_FMT_S16;
static const int kMaxInputSamples = 1024;


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
  processed_video_frame_ = av_frame_alloc();
  if (!packet_) {
    LOGE("alloc packet failed\n");
  }

  swr_context_ = swr_alloc_set_opts(nullptr, av_get_default_channel_layout(kOutChannels),
                                    kOutSampleFmt,
                                    kOutSampleRate,
                                    av_get_default_channel_layout(channels_),
                                    static_cast<AVSampleFormat>(sample_fmt_),
                                    sample_rate_,
                                    0, nullptr);
  swr_init(swr_context_);

  sws_context_ = sws_getContext(video_dec_ctx_->width, video_dec_ctx_->height,
                                video_dec_ctx_->pix_fmt,
                                video_dec_ctx_->width, video_dec_ctx_->height, kOutPixFmt,
                                SWS_BILINEAR, nullptr, nullptr, nullptr);
  if (!sws_context_) {
    LOGE("sws_getContext failed\n");
    return -1;
  }

  return 0;
}

int SWVideoDecoder::Finalize() {
  av_packet_free(&packet_);
  av_frame_free(&frame_);
  av_frame_free(&processed_video_frame_);
  avcodec_free_context(&audio_dec_ctx_);
  avcodec_free_context(&video_dec_ctx_);
  avformat_close_input(&fmt_ctx_);
  sws_freeContext(sws_context_);
  swr_free(&swr_context_);

  return 0;
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
                                FrameQueuePtr frame_q,
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

//    Frame *dst_frame = new Frame();
    Frame *dst_frame = &(frame_q->push_back_peek());
    if (dec->codec->type == AVMEDIA_TYPE_AUDIO) {
      TIME_EVENT(Stats::first_audio_decoded_time_pt);
      float time_unit = av_q2d(fmt_ctx_->streams[audio_stream_index_]->time_base);
      float duration_time = frame->pkt_duration * time_unit;
      *decoded_duration += duration_time;
      ProcessAudioFrame(frame, time_unit, dst_frame);
      float pts_time = frame->pts * time_unit;
      float dts_time = frame->pkt_dts * time_unit;
//      LOGI("receive %s frame pkt_pos=%d, pkt_duration=%d, duration_time=%f, pts=%d, pts_time=%f, "
//           "dts=%d, dts_time=%f", av_get_media_type_string(dec->codec->type),
//           frame_->pkt_pos, frame_->pkt_duration, duration_time,
//           frame->pts, pts_time, frame->pkt_dts, dts_time);
    } else {
      TIME_EVENT(Stats::first_video_decoded_time_pt);
      float time_unit = av_q2d(fmt_ctx_->streams[video_stream_index_]->time_base);
      float duration_time = frame->pkt_duration * time_unit;
      ProcessVideoFrame(frame, time_unit, dst_frame, false);
      float pts_time = frame->pts * time_unit;
      float dts_time = frame->pkt_dts * time_unit;
//      LOGI("receive %s frame pkt_pos=%d, pkt_duration=%d, duration_time=%f, pts=%d, pts_time=%f, "
//           "dts=%d, dts_time=%f", av_get_media_type_string(dec->codec->type),
//           frame_->pkt_pos, frame_->pkt_duration, duration_time,
//           frame->pts, pts_time, frame->pkt_dts, dts_time);
    }

    if (dec->codec->type == AVMEDIA_TYPE_AUDIO) {
//      LOGI("recv audio frame: %s", dst_frame->ToString().c_str());
    } else {
//      LOGI("recv video frame: %s", dst_frame->ToString().c_str());
    }
//    frame_q->emplace(dst_frame);
//    frames->emplace_back(av_frame_clone(frame_));
    frame_q->push_back(*dst_frame);
    av_frame_unref(frame_);
  }

  return ret;
}

int SWVideoDecoder::DecodeFrames(float duration,
                                 float *decoded_duration,
                                 FrameQueuePtr audio_q,
                                 FrameQueuePtr video_q) {
  int ret = 0;
//  float decoded_duration = 0.0f;
  AVCodecContext *dec = nullptr;
  FrameQueuePtr queue = nullptr;
  bool is_eof = false;
  while (*decoded_duration <= duration) {
    ret = av_read_frame(fmt_ctx_, packet_);
    if (ret < 0) {
      if (ret == AVERROR_EOF) {
        LOGI("av_read_frame return %s", av_err2str(ret));
        is_eof = true;
        DecodeFrame(audio_dec_ctx_, nullptr, audio_q, frame_, decoded_duration);
        DecodeFrame(video_dec_ctx_, nullptr, video_q, frame_, decoded_duration);
        break;
      } else {
        LOGE("av_read_frame return %s\n", av_err2str(ret));
        break;
      }
    }

    if (packet_->stream_index == audio_stream_index_) {
      TIME_EVENT(Stats::recv_first_audio_pkt_time_pt);
      dec = audio_dec_ctx_;
      queue = audio_q;
    } else if (packet_->stream_index == video_stream_index_) {
      TIME_EVENT(Stats::recv_first_video_pkt_time_pt);
      dec = video_dec_ctx_;
      queue = video_q;
    }

    ret = DecodeFrame(dec, packet_, queue, frame_, decoded_duration);
    av_packet_unref(packet_);
  }


  return is_eof ? AVERROR_EOF : ret;
}

void SWVideoDecoder::ProcessVideoFrame(AVFrame *src, float time_unit, Frame *dst, bool convert) {
//  BlockProfiler profiler("ProcessVideoFrame");
  int ret;
  int processed_data_size;
  if (convert) {
    processed_data_size = av_image_get_buffer_size(kOutPixFmt, src->width, src->height, 1);
  } else {
    processed_data_size = av_image_get_buffer_size(video_dec_ctx_->pix_fmt, src->width, src->height, 1);
  }
//  uint8_t *processed_data = new uint8_t[processed_data_size];
  uint8_t *processed_data = dst->GetData();
  if (processed_data == nullptr) {
    processed_data = new uint8_t[processed_data_size];
  }

  if (convert) {
    av_image_fill_arrays(processed_video_frame_->data, processed_video_frame_->linesize, processed_data,
                         kOutPixFmt, src->width, src->height, 1);
    ret = sws_scale(sws_context_,
                    src->data,
                    src->linesize,
                    0,
                    src->height,
                    processed_video_frame_->data,
                    processed_video_frame_->linesize);
    av_frame_unref(processed_video_frame_);
    if (ret < 0) {
      LOGE("sws_scale failed %s", av_err2str(ret));
    }
    dst->SetPixFmt(kOutPixFmt);
  } else {
    av_image_copy_to_buffer(processed_data, processed_data_size, src->data, src->linesize, video_dec_ctx_->pix_fmt, src->width, src->height, 1);
    dst->SetPixFmt(video_dec_ctx_->pix_fmt);
  }

  dst->SetMediaType(kMediaTypeVideo);
  dst->SetData(processed_data);
  dst->SetSize(processed_data_size);
  dst->SetDuration(src->pkt_duration * time_unit);
  dst->SetPts(src->pts * time_unit);
  dst->SetWidth(src->width);
  dst->SetHeight(src->height);
}

void SWVideoDecoder::ProcessAudioFrame(AVFrame *src, float time_unit, Frame *dst) {
//  BlockProfiler profiler("ProcessAudioFrame");
  int input_nb_samples = av_rescale_rnd(kMaxInputSamples, kOutSampleRate, sample_rate_, AV_ROUND_UP);
  int out_data_size = av_samples_get_buffer_size(nullptr, kOutChannels, input_nb_samples,
                                                 (AVSampleFormat) kOutSampleFmt, 1);
  uint8_t *data = dst->GetData();
  if (data == nullptr) {
    data = new uint8_t[out_data_size];
  }
  int out_nb_samples = swr_convert(swr_context_, &data, src->nb_samples, (const uint8_t**) src->data, src->nb_samples);
  dst->SetMediaType(kMediaTypeAudio);
  dst->SetData(data);
  dst->SetSize(out_data_size);
  dst->SetDuration(src->pkt_duration * time_unit);
  dst->SetPts(src->pts * time_unit);
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
