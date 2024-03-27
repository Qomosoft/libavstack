//
// Created by Robin on 2024/3/18.
//
#pragma once
#define LOG_TAG "MediaPlayer"

#include "media_player.h"
#include "av_synchronizer.h"
#include "audio/audio_renderer.h"
#include "video/video_renderer.h"

#include <string>
#include <memory>

 class MediaPlayer : public FrameCallback {
 public:
  MediaPlayer() = default;
  virtual ~MediaPlayer();
  void Init(int decoder_type);
  void Finalize();
  void SetDataSource(const std::string& path);
  void Prepare();
  void PrepareAsync();
  void Start();
  void Stop();
  void Pause();
  void Seek(float position);

  int OnFrameNeeded(AVFrame **frame, AVMediaType type) override;

 private:
  std::shared_ptr<AVSynchronizer> synchronizer_;
  std::unique_ptr<AudioRenderer> audio_renderer_;
  std::unique_ptr<VideoRenderer> video_renderer_;

  std::string path_;
  int decoder_type_;
  bool is_playing_ = false;
  bool is_stopped_ = true;
};