#define LOG_TAG "OpenSLPlayer"

#include "opensles_renderer.h"
#include "logging.h"

#include <cassert>

#ifndef CHECK_SL_ERROR
#define CHECK_SL_ERROR(result) assert(SL_RESULT_SUCCESS == (result))
#endif

static const int kOutSampleRate = 44100;
static const int kOutChannels = 2;
static const AVSampleFormat kOutSampleFmt = AV_SAMPLE_FMT_S16;
static const int kMaxInputSamples = 1024;

OpenSLESRenderer::OpenSLESRenderer() : engine_object_(nullptr),
                                       engine_interface_(nullptr),
                                       out_mix_object_(nullptr),
                                       output_mix_environmental_reverb_(nullptr),
                                       reverb_settings_(SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT),
                                       audio_player_object_(nullptr),
                                       audio_player_interface_(nullptr),
                                       audio_player_buffer_queue_(nullptr),
                                       sample_rate_(0),
                                       channels_(0) {}

OpenSLESRenderer::~OpenSLESRenderer() = default;

int OpenSLESRenderer::Init(int sample_rate,
                           int channels,
                           int sample_fmt,
                           int buffer_size,
                           FrameCallback *callback) {
  LOGI("sample_rate=%d, channels=%d, buffer_size=%d\n", sample_rate, channels, buffer_size);
  callback_ = callback;
  sample_rate_ = sample_rate * 1000;
  channels_ = channels;
//  pcm_data_ = std::vector<uint8_t>(sample_rate * buffer_size * channels);
  CreateEngine();
  swr_context_ = swr_alloc_set_opts(nullptr, av_get_default_channel_layout(kOutChannels),
                                    kOutSampleFmt,
                                    kOutSampleRate,
                                    av_get_default_channel_layout(channels),
                                    static_cast<AVSampleFormat>(sample_fmt),
                                    sample_rate,
                                    0, nullptr);
  swr_init(swr_context_);
  int input_nb_samples = av_rescale_rnd(kMaxInputSamples, kOutSampleRate, sample_rate, AV_ROUND_UP);
  int out_data_size = av_samples_get_buffer_size(nullptr, kOutChannels, input_nb_samples,
                                                 (AVSampleFormat) kOutSampleFmt, 1);
  pcm_data_ = std::vector<uint8_t>(out_data_size);

  return 0;
}

void OpenSLESRenderer::CreateEngine() {
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
}

void OpenSLESRenderer::Finalize() {
  LOGI("Finalize");
}

void OpenSLESRenderer::Start() {
  // step 7. start playing
  SLresult result = (*audio_player_interface_)->SetPlayState(audio_player_interface_, SL_PLAYSTATE_PLAYING);
  CHECK_SL_ERROR(result);
  is_eof_ = false;
  BufferQueueCallback(audio_player_buffer_queue_, this);
}

void OpenSLESRenderer::Stop() {
  if (audio_player_interface_) {
    (*audio_player_interface_)->SetPlayState(audio_player_interface_, SL_PLAYSTATE_STOPPED);
  }
}

void OpenSLESRenderer::Destroy() {
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

  swr_free(&swr_context_);
}

void OpenSLESRenderer::BufferQueueCallback(SLAndroidSimpleBufferQueueItf buffer_queue_itf,
                                           void *context) {
    auto *player = reinterpret_cast<OpenSLESRenderer *>(context);
    if (!player) {
      LOGE("error: invalid context");
      return;
    }
    player->OnBufferQueueCallback();
}

void OpenSLESRenderer::OnBufferQueueCallback() {
  LOGI("enter");
  if (is_eof_) {
    LOGI("EOF\n");
    return;
  }

  int pcm_data_size = static_cast<int>(pcm_data_.size());
  AVFrame *frame = nullptr;
  int ret = callback_->OnFrameNeeded(&frame, AVMEDIA_TYPE_AUDIO);
  if (ret == AVERROR_EOF) {
    is_eof_ = true;
    LOGI("EOF\n");
  }

  if (!frame) {
    LOGE("frame is null\n");
    return;
  }

  uint8_t *data = pcm_data_.data();
  int out_nb_samples = swr_convert(swr_context_, &data, frame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
  av_frame_unref(frame);
  if (out_nb_samples > 0) {
    SLresult result = (*audio_player_buffer_queue_)->Enqueue(audio_player_buffer_queue_, data, pcm_data_size);
    CHECK_SL_ERROR(result);
  } else {
    LOGI("end of file");
  }
  LOGI("leave");
}
