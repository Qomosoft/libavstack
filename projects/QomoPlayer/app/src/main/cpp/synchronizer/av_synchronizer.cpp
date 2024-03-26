//
// Created by Robin on 2024/3/20.
//

#include "av_synchronizer.h"
#include "sw_video_decoder.h"
#include "logging.h"

static const int kHWDecoder = 0;
static const int kSWDecoder = 1;

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
  if (!decoder_thread_) {
    LOGW("already started!\n");
    return;
  }

  is_on_decoding_ = true;
  decoder_thread_ = std::make_unique<std::thread>(&AVSynchronizer::DecoderLoop, this);
}

void AVSynchronizer::Stop() {
  Finalize();
}

void AVSynchronizer::DecodeFrames() {
  int ret = 0;
  float duration = 0.1f;
  while (buffered_duration_ < max_buffered_duration_) {
    std::list<AVFrame *> frames;
    ret = decoder_->DecodeFrames(duration, &frames);
    if (ret != 0) {
      LOGE("DecodeFrames failed, return %d\n", ret);
      break;
    }

    for (auto frame : frames) {
      if (frame->pict_type == AV_PICTURE_TYPE_NONE) {
        buffered_duration_ += frame->pkt_duration;
        audio_frame_buffer_.emplace(frame);
      } else {
        video_frame_buffer_.emplace(frame);
      }
    }
  }
}

void AVSynchronizer::DecoderLoop() {
  while (is_on_decoding_) {
    std::unique_lock<std::mutex> lock;
    cv_.wait(lock, [this] { return buffered_duration_ < max_buffered_duration_ || !is_on_decoding_; });
    if (!is_on_decoding_) break;

    DecodeFrames();
  }
}
