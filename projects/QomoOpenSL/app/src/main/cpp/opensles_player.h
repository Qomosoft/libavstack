#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <string>
#include <vector>
#include <fstream>

class OpenSlEsPlayer {
 public:
  OpenSlEsPlayer();
  virtual ~OpenSlEsPlayer();
  int Initialize(const std::string &audio_source_path,
                 int sample_rate,
                 int buffer_size,
                 int channels);
  void UnInitialize();
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
  std::string audio_source_path_;
  std::ifstream audio_source_file_;
};
