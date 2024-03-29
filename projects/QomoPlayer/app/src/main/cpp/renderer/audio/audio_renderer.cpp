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
                             channels_(channels),
                             sample_rate_(sample_rate),
                             callback_(callback) {
  open_sl_es_renderer_ = std::make_unique<OpenSLESRenderer>();
  open_sl_es_renderer_->Init(sample_rate, channels, sample_fmt, 2, callback);
}

AudioRenderer::~AudioRenderer() {
  Stop();
}

void AudioRenderer::Start() {
  LOGI("enter");
  open_sl_es_renderer_->Start();
  is_stopped_ = false;
  LOGI("leave");
}

void AudioRenderer::Pause() {
  if (!is_playing_) {
    LOGW("AudioRenderer was already paused\n");
    return;
  }
  is_playing_ = false;
  open_sl_es_renderer_->Stop();
}

void AudioRenderer::Resume() {
  if (is_playing_) {
    LOGW("AudioRenderer was already resumed\n");
    return;
  }
  is_playing_ = true;
  open_sl_es_renderer_->Start();
}

void AudioRenderer::Stop() {
  if (is_stopped_) {
    LOGW("AudioRenderer was already stopped\n");
    return;
  }

  open_sl_es_renderer_->Stop();
  open_sl_es_renderer_->Destroy();
}
