#define LOG_TAG "OpenSLPlayer"

#include "opensles_player.h"
#include "logging.h"

#include <cassert>

#ifndef CHECK_SL_ERROR
#define CHECK_SL_ERROR(result) assert(SL_RESULT_SUCCESS == (result))
#endif

OpenSlEsPlayer::OpenSlEsPlayer() : engine_object_(nullptr),
                                   engine_interface_(nullptr),
                                   out_mix_object_(nullptr),
                                   output_mix_environmental_reverb_(nullptr),
                                   reverb_settings_(SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT),
                                   audio_player_object_(nullptr),
                                   audio_player_interface_(nullptr),
                                   audio_player_buffer_queue_(nullptr),
                                   sample_rate_(0),
                                   channels_(0) {}

OpenSlEsPlayer::~OpenSlEsPlayer() = default;

int OpenSlEsPlayer::Initialize(const std::string &audio_source_path,
                               int sample_rate,
                               int buffer_size,
                               int channels) {
  audio_source_path_ = audio_source_path;
  sample_rate_ = sample_rate * 1000;
  channels_ = channels;
  pcm_data_ = std::vector<uint8_t>(sample_rate * buffer_size * channels);
  CreateEngine();
  return 0;
}

void OpenSlEsPlayer::CreateEngine() {
  // step 1. create engine
  SLresult result = slCreateEngine(&engine_object_, 0, nullptr, 0, nullptr, nullptr);
  CHECK_SL_ERROR(result);

  // step 2. realize the engine
  result = (*engine_object_)->Realize(engine_object_, SL_BOOLEAN_FALSE);
  CHECK_SL_ERROR(result);

  // step 3. get the engine interface which is needed in order to create other objects
  result = (*engine_object_)->GetInterface(engine_object_, SL_IID_ENGINE, &engine_interface_);
  CHECK_SL_ERROR(result);

  SLDataLocator_AndroidSimpleBufferQueue buffer_queue = {
      SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
      2
  };
  SLDataFormat_PCM fmt = {
      SL_DATAFORMAT_PCM,
      static_cast<SLuint32>(channels_),
      sample_rate_,
      SL_PCMSAMPLEFORMAT_FIXED_16,
      SL_PCMSAMPLEFORMAT_FIXED_16,
      SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
      SL_BYTEORDER_LITTLEENDIAN
  };
  SLDataSource audio_source = {
      &buffer_queue,
      &fmt
  };

  // step 4. create output mix
  const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
  const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

  result = (*engine_interface_)->CreateOutputMix(engine_interface_, &out_mix_object_, 1, mids, mreq);
  CHECK_SL_ERROR(result);

  result = (*out_mix_object_)->Realize(out_mix_object_, SL_BOOLEAN_FALSE);
  CHECK_SL_ERROR(result);

  result = (*out_mix_object_)->GetInterface(out_mix_object_, SL_IID_ENVIRONMENTALREVERB, &output_mix_environmental_reverb_);

  if (result == SL_RESULT_SUCCESS) {
    result = (*output_mix_environmental_reverb_)->SetEnvironmentalReverbProperties(output_mix_environmental_reverb_, &reverb_settings_);
    if (result != SL_RESULT_SUCCESS) {
      LOGI("warning: failed to set reverb properties");
    }
  }

  // step 5. create audio player
  SLDataLocator_OutputMix locator_output_mix = {
      SL_DATALOCATOR_OUTPUTMIX,
      out_mix_object_
  };
  SLDataSink audio_sink = {
      &locator_output_mix,
      nullptr
  };
  const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
  const SLboolean req[1] = {SL_BOOLEAN_TRUE};
  result = (*engine_interface_)->CreateAudioPlayer(engine_interface_, &audio_player_object_, &audio_source, &audio_sink, 1, ids, req);
  CHECK_SL_ERROR(result);

  result = (*audio_player_object_)->Realize(audio_player_object_, SL_BOOLEAN_FALSE);
  CHECK_SL_ERROR(result);

  result = (*audio_player_object_)->GetInterface(audio_player_object_, SL_IID_PLAY, &audio_player_interface_);
  CHECK_SL_ERROR(result);

  result = (*audio_player_object_)->GetInterface(audio_player_object_, SL_IID_BUFFERQUEUE, &audio_player_buffer_queue_);
  CHECK_SL_ERROR(result);

  // step 6. register callback on the buffer queue
  result = (*audio_player_buffer_queue_)->RegisterCallback(audio_player_buffer_queue_, BufferQueueCallback, this);
  CHECK_SL_ERROR(result);

  audio_source_file_ = std::ifstream(audio_source_path_, std::ios::binary);
  if (!audio_source_file_.is_open()) {
    LOGE("error: failed to open audio source file");
  }
}

void OpenSlEsPlayer::UnInitialize() {
  LOGI("UnInitialize");
}

void OpenSlEsPlayer::Start() {
  // step 7. start playing
  SLresult result = (*audio_player_interface_)->SetPlayState(audio_player_interface_, SL_PLAYSTATE_PLAYING);
  CHECK_SL_ERROR(result);

  BufferQueueCallback(audio_player_buffer_queue_, this);
}

void OpenSlEsPlayer::Stop() {
  if (audio_player_interface_) {
    (*audio_player_interface_)->SetPlayState(audio_player_interface_, SL_PLAYSTATE_STOPPED);
  }
}

void OpenSlEsPlayer::Destroy() {
  // step 1. destroy audio player
  if (audio_player_object_) {
    (*audio_player_object_)->Destroy(audio_player_object_);
    audio_player_object_ = nullptr;
    audio_player_interface_ = nullptr;
    audio_player_buffer_queue_ = nullptr;
  }

  // step 2. destroy output mix
  if (out_mix_object_) {
    (*out_mix_object_)->Destroy(out_mix_object_);
    out_mix_object_ = nullptr;
    output_mix_environmental_reverb_ = nullptr;
  }

  // step 3. destroy engine
  if (engine_object_) {
    (*engine_object_)->Destroy(engine_object_);
    engine_object_ = nullptr;
    engine_interface_ = nullptr;
  }

  // step 4. close audio source file
  if (audio_source_file_.is_open()) {
    audio_source_file_.close();
  }
}

void OpenSlEsPlayer::BufferQueueCallback(SLAndroidSimpleBufferQueueItf buffer_queue_itf,
                                         void *context) {
    auto *player = reinterpret_cast<OpenSlEsPlayer *>(context);
    if (!player) {
      LOGE("error: invalid context");
      return;
    }
    player->OnBufferQueueCallback();
}
void OpenSlEsPlayer::OnBufferQueueCallback() {
  if (!audio_source_file_.is_open()) {
    LOGE("error: audio source file is not open");
    return;
  }

  int pcm_data_size = static_cast<int>(pcm_data_.size());
  int read_bytes = audio_source_file_.read(reinterpret_cast<char *>(pcm_data_.data()), pcm_data_size).gcount();
  if (read_bytes > 0) {
    SLresult result = (*audio_player_buffer_queue_)->Enqueue(audio_player_buffer_queue_, pcm_data_.data(), read_bytes);
    CHECK_SL_ERROR(result);
  } else {
    LOGI("end of file");
  }
}
