//
// Created by Robin on 2024/3/20.
//

#define LOG_TAG "AVSynchronizer"

#include "av_synchronizer.h"
#include "sw_video_decoder.h"
#include "logging.h"

static const int kHWDecoder = 0;
static const int kSWDecoder = 1;

static const float kMinBufferedDuration = 0.5f; // seconds
static const float kMaxBufferedDuration = 0.8f; // seconds
static const float kSyncMaxTimeDiff = 0.05f; // seconds
static const float kDefaultAudioBufferDuration = 0.03f; // seconds

AVSynchronizer::AVSynchronizer() : decoder_thread_(nullptr),
                                   is_on_decoding_(false),
                                   min_buffered_duration_(kMinBufferedDuration),
                                   max_buffered_duration_(kMaxBufferedDuration),
                                   sync_max_time_diff_(kSyncMaxTimeDiff),
                                   buffered_duration_(0.0f) {}

AVSynchronizer::~AVSynchronizer() {
  Finalize();
}

bool AVSynchronizer::Init(const std::string &url, int decode_type) {
  if (decode_type == kSWDecoder) {
    decoder_ = std::make_unique<SWVideoDecoder>();
  } else {
    decoder_ = std::make_unique<SWVideoDecoder>();
  }

  if (!decoder_) {
    LOGE("Failed to create decoder\n");
    return false;
  }

  audio_frame_buffer_ = std::make_shared<std::queue<AVFrame *>>();
  video_frame_buffer_ = std::make_shared<std::queue<AVFrame *>>();

  decoder_->Init(url);

  return true;
}

void AVSynchronizer::Finalize() {
  if (!decoder_thread_) {
    is_on_decoding_ = false;
    cv_.notify_all();
    decoder_thread_->join();
    decoder_thread_ = nullptr;
  }
}

void AVSynchronizer::Start() {
  LOGI("enter");
  if (decoder_thread_) {
    LOGW("already started!\n");
    return;
  }

  is_on_decoding_ = true;
  decoder_thread_ = std::make_unique<std::thread>(&AVSynchronizer::DecoderLoop, this);
}

void AVSynchronizer::Stop() {
  LOGI("enter");
  Finalize();
}

int AVSynchronizer::DecodeFrames() {
  float duration = 0.1f;
  int ret;
  if (is_seeking_) {
    while (!audio_frame_buffer_->empty()) {
      AVFrame *frame = audio_frame_buffer_->front();
      av_frame_unref(frame);
      audio_frame_buffer_->pop();
    }
    while (!video_frame_buffer_->empty()) {
      AVFrame *frame = video_frame_buffer_->front();
      av_frame_unref(frame);
      video_frame_buffer_->pop();
    }

    decoder_->Seek(seek_position_);
    buffered_duration_ = 0;
    is_seeking_ = false;
  }

  while (buffered_duration_ < max_buffered_duration_) {
    std::list<AVFrame *> frames;
    ret = decoder_->DecodeFrames(duration, &frames);
    if (ret != 0) {
      LOGE("DecodeFrames failed %s\n", av_err2str(ret));
      break;
    }

    for (auto frame : frames) {
      if (frame->pict_type == AV_PICTURE_TYPE_NONE) {
        float time_unit = decoder_->GetAudioTimeUnit();
        buffered_duration_ += frame->pkt_duration * time_unit;
        audio_frame_buffer_->emplace(frame);
      } else {
        video_frame_buffer_->emplace(frame);
      }
    }
  }

  return ret;
}

int AVSynchronizer::Seek(float seconds) {
  LOGI("seconds=%f", seconds);
  is_eof_ = false;
  seek_position_ = seconds;
  is_seeking_ = true;
  return 0;
}

void AVSynchronizer::DecoderLoop() {
  while (is_on_decoding_) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return (buffered_duration_ < max_buffered_duration_ && !is_eof_) || !is_on_decoding_; });
    if (!is_on_decoding_) break;

    int ret = DecodeFrames();
    if (ret == AVERROR_EOF) is_eof_ = true;
  }
}

int AVSynchronizer::OnFrameNeeded(AVFrame **frame, AVMediaType type) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (is_eof_) return AVERROR_EOF;

  std::shared_ptr<std::queue<AVFrame *>> buffer_queue;

  if (type == AVMEDIA_TYPE_AUDIO) {
    buffer_queue = audio_frame_buffer_;
  } else if (type == AVMEDIA_TYPE_VIDEO) {
    buffer_queue = video_frame_buffer_;
  }

  if (buffer_queue->empty()) {
    LOGW("buffered %s buffered_frame not available\n", av_get_media_type_string(type));
    return -1;
  }

  AVFrame *buffered_frame = nullptr;
  if (type == AVMEDIA_TYPE_AUDIO) {
    buffered_frame = buffer_queue->front();
    buffer_queue->pop();
    float time_unit = decoder_->GetAudioTimeUnit();
    current_playback_position_ = buffered_frame->pts * time_unit;
    current_audio_frame_duration_ = buffered_frame->pkt_duration * time_unit;
    buffered_duration_ -= current_audio_frame_duration_;
  } else if (type == AVMEDIA_TYPE_VIDEO) {
    while (true) {
      buffered_frame = buffer_queue->front();
      if (is_first_frame_) {
        buffer_queue->pop();
        is_first_frame_ = false;
        break;
      }

      //TODO: check if the current_audio_frame_duration_ correct
      float time_unit = decoder_->GetVideoTimeUnit();
      float delta = (current_playback_position_ - current_audio_frame_duration_) - buffered_frame->pts * time_unit;
      //视频比音频快了好多，继续渲染上一帧
      if (delta < -sync_max_time_diff_) {
//        LOGI("video faster than current play position, diff=%f", delta);
        buffered_frame = nullptr;
        break;
        //视频比音频慢了好多，需要从queue中继续取并做丢帧处理，直到拿到合适的帧
      } else if (delta > sync_max_time_diff_) {
//        LOGI("video slower than current play position, diff=%f", delta);
        buffer_queue->pop();
        av_frame_unref(buffered_frame);
        continue;
      } else {
//        LOGI("diff=%f", delta);
        buffer_queue->pop();
        break;
      }
    }
  }

  *frame = buffered_frame;

  cv_.notify_one();

  return 0;
}
int AVSynchronizer::channels() const {
  return decoder_->channels();
}
int AVSynchronizer::sample_rate() const {
  return decoder_->sample_rate();
}
int AVSynchronizer::sample_fmt() const {
  return decoder_->sample_fmt();
}
