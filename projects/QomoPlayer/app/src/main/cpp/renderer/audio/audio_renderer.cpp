//
// Created by Robin on 2024/3/26.
//

#define LOG_TAG "AudioRenderer"

#include "audio_renderer.h"
#include "logging.h"

AudioRenderer::AudioRenderer(int channels, int sample_rate, int sample_fmt, FrameCallback *callback)
    :
                             is_playing_(false),
                             is_stopped_(true),
                             audio_renderer_thread_(nullptr),
                             channels_(channels),
                             sample_rate_(sample_rate),
                             callback_(callback) {
  open_sl_es_player_ = std::make_unique<OpenSlEsPlayer>();
  open_sl_es_player_->Init(sample_rate, channels, sample_fmt, 2, callback);
}

AudioRenderer::~AudioRenderer() {
  Stop();
}

void AudioRenderer::RenderAudioFrame() {
  LOGI("enter");
  AVFrame *frame = nullptr;
  int ret = callback_->OnFrameNeeded(&frame, AVMEDIA_TYPE_AUDIO);
  if (ret == AVERROR_EOF) {
    LOGI("EOF\n");
    is_eof_ = true;
  }

  if (!frame) {
    LOGE("frame is null\n");
    return;
  }

  LOGI("sleep_for %ld nanoseconds\n", frame->pkt_duration);
  std::this_thread::sleep_for(std::chrono::nanoseconds(frame->pkt_duration));
  //TODO: play sound
  av_frame_unref(frame);
  LOGI("leave");
}

void AudioRenderer::Start() {
  LOGI("enter");
  open_sl_es_player_->Start();
//  if (audio_renderer_thread_) {
//    LOGE("AudioRenderer already started");
//    return;
//  }
//
//  is_playing_ = true;
//  is_stopped_ = false;
//  audio_renderer_thread_ = std::make_unique<std::thread>(&AudioRenderer::AudioRenderLoop, this);
  LOGI("leave");
}

void AudioRenderer::Pause() {
  std::unique_lock<std::mutex> lock(mutex_);
  is_playing_ = false;
}

void AudioRenderer::Resume() {
  std::unique_lock<std::mutex> lock(mutex_);
  is_playing_ = true;
  cv_.notify_one();
}

void AudioRenderer::Stop() {
  if (is_stopped_) {
    LOGW("AudioRenderer already stopped\n");
    return;
  }

  open_sl_es_player_->Stop();
  open_sl_es_player_->Destroy();
//  std::unique_lock<std::mutex> lock(mutex_);
//  is_stopped_ = true;
//  cv_.notify_all();
//  audio_renderer_thread_->join();
}

void AudioRenderer::AudioRenderLoop() {
  while (!is_stopped_) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return (is_playing_ && !is_eof_) || is_stopped_; });

    if (is_stopped_) {
      LOGI("AudioRenderLoop stop\n");
      break;
    }

    RenderAudioFrame();
  }
}
