//
// Created by Robin on 2024/3/20.
//

#include "av_synchronizer.h"
#include "media_frame.h"
#include "sw_video_decoder.h"
#include "logging.h"

AVSynchronizer::AVSynchronizer() : decoder_thread_(nullptr),
                                   is_on_decoding_(false) {}

AVSynchronizer::~AVSynchronizer() {
  Finalize();
}

bool AVSynchronizer::Init(const std::string &url, int decode_type) {
  if (decode_type == SWDecoder) {
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
  is_on_decoding_ = false;
}

void AVSynchronizer::DecoderLoop() {
  while (is_on_decoding_) {
    //TODO
  }
}
