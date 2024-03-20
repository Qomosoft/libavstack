//
// Created by Robin on 2024/3/18.
//
#pragma once
#define LOG_TAG "MediaPlayer"

#include "media_player.h"
#include "av_synchronizer.h"

#include <string>
#include <memory>

class MediaPlayer {
 public:
  MediaPlayer() = default;
  virtual ~MediaPlayer() = default;
  void Init(int decoder_type);
  void Finalize();
  void SetDataSource(const std::string& path);
  void Prepare();
  void PrepareAsync();
  void Start();
  void Stop();
  void Pause();
  void Seek(float position);

 private:
  std::shared_ptr<AVSynchronizer> synchronizer_;
  std::string path_;
  int decoder_type_;
};