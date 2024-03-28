//
// Created by Robin on 2024/3/26.
//

#include "opensles_renderer.h"
#include "frame_callback.h"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

class AudioRenderer {
 public:
  AudioRenderer(int channels,
                int sample_rate,
                int sample_fmt,
                FrameCallback *callback);
  ~AudioRenderer();
  void Start();
  void Pause();
  void Resume();
  void Stop();

 private:
  std::unique_ptr<OpenSLESRenderer> open_sl_es_renderer_;
  FrameCallback *callback_;
  int channels_;
  int sample_rate_;
  bool is_playing_;
  bool is_stopped_;
  bool is_eof_ = false;
};