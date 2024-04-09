#pragma once

extern "C" {
#include "libswresample/swresample.h"
}

#include "frame_callback.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <string>
#include <vector>
#include <fstream>

class OpenSLESRenderer {
 public:
  OpenSLESRenderer();
  virtual ~OpenSLESRenderer();
  int Init(int sample_rate,
           int channels,
           int sample_fmt,
           int buffer_size,
           FrameCallback *callback);
  void Finalize();
  void Start();
  void Stop();
  void Destroy();

 private:
  void CreateEngine();
  static void BufferQueueCallback(SLAndroidSimpleBufferQueueItf buffer_queue_itf, void *context);
  void OnBufferQueueCallback();

 private:
  SLObjectItf engine_object_;
  SLEngineItf engine_interface_;
  SLObjectItf out_mix_object_;
  SLEnvironmentalReverbItf output_mix_environmental_reverb_;
  SLEnvironmentalReverbSettings reverb_settings_;
  SLObjectItf audio_player_object_;
  SLPlayItf audio_player_interface_;
  SLAndroidSimpleBufferQueueItf audio_player_buffer_queue_;

  SLmilliHertz sample_rate_;
  int channels_;
  std::vector<uint8_t> pcm_data_;

  FrameCallback *callback_;
  SwrContext *swr_context_;
  bool is_eof_ = false;
};
