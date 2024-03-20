//
// Created by Robin on 2024/3/19.
//
#pragma once

#include <cstdint>

typedef enum CodecType {
  SWDecoder,
  HWDecoder
} CodecType;

typedef enum MediaType {
  MediaTypeNone,
  MediaTypeAudio,
  MediaTypeVideo
} MediaType;

class MediaFrame {
 public:
  MediaFrame();
  ~MediaFrame() = default;
  float position;
  float duration;
  virtual MediaType GetType() = 0;
};

class AudioFrame : public MediaFrame {
 public:
  AudioFrame();
  ~AudioFrame();
  uint8_t *samples_;
  int size_;
  virtual MediaType GetType();
};

class VideoFrame : public MediaFrame {
 public:
  VideoFrame();
  ~VideoFrame();
  uint8_t *luma;
  uint8_t *chromaB;
  uint8_t *chromaR;
  int width_;
  int height_;
  virtual MediaType GetType();
};