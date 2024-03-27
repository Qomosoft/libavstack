//
// Created by Robin on 2024/3/26.
//

#include "frame_callback.h"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

class AudioRenderer {
 public:
  AudioRenderer(int channels, int sample_rate, FrameCallback *callback);
  ~AudioRenderer();
  void Start();
  void Pause();
  void Resume();
  void Stop();

 private:
  void RenderAudioFrame();
  void AudioRenderLoop();

 private:
  FrameCallback *callback_;
  std::unique_ptr<std::thread> audio_renderer_thread_;
  int channels_;
  int sample_rate_;
  bool is_playing_;
  bool is_stopped_;
  bool is_eof_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;
};