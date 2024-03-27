//
// Created by Robin on 2024/3/18.
//

#include "media_player.h"
#include "logging.h"

MediaPlayer::~MediaPlayer() {
  Finalize();
}

void MediaPlayer::Init(int decoder_type) {
  LOGI("enter");
  decoder_type_ = decoder_type;
}

void MediaPlayer::Finalize() {
  LOGI("enter");
  Stop();
}

void MediaPlayer::SetDataSource(const std::string &path) {
  LOGI("path=%s", path.c_str());
  path_ = path;
}

void MediaPlayer::Prepare() {
  LOGI("enter");
  synchronizer_ = std::make_shared<AVSynchronizer>();
  synchronizer_->Init(path_, decoder_type_);
  int channels = synchronizer_->channels();
  int sample_rate = synchronizer_->sample_rate();
  audio_renderer_ = std::make_unique<AudioRenderer>(channels, sample_rate, this);
  LOGI("channels=%d, sample_rate=%d\n", channels, sample_rate);
  video_renderer_ = std::make_unique<VideoRenderer>(this);
  synchronizer_->Start();
}

void MediaPlayer::PrepareAsync() {
  //TODO
}

void MediaPlayer::Start() {
  LOGI("enter");
  if (is_playing_) {
    LOGW("already started\n");
    return;
  }

  is_playing_ = true;
  is_stopped_ = false;
  audio_renderer_->Start();
}

void MediaPlayer::Stop() {
  LOGI("enter");
  if (is_stopped_) {
    LOGW("already stopped\n");
    return;
  }

  is_stopped_ = true;
  audio_renderer_->Stop();
}

void MediaPlayer::Pause() {
  LOGI("enter");
  if (!is_playing_) {
    LOGW("already paused\n");
    return;
  }

  is_playing_ = false;
  audio_renderer_->Pause();
}

void MediaPlayer::Seek(float position) {

}

int MediaPlayer::OnFrameNeeded(AVFrame **frame, AVMediaType type) {
  if (type == AVMEDIA_TYPE_AUDIO) {
    int ret =  synchronizer_->OnFrameNeeded(frame, type);
    video_renderer_->OnDraw();
    return ret;
  }

  return synchronizer_->OnFrameNeeded(frame, type);
}
