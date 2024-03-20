//
// Created by Robin on 2024/3/19.
//

#include "media_frame.h"

MediaFrame::MediaFrame() : position(0.0f), duration(0.0f) {}

AudioFrame::AudioFrame() :samples_(nullptr), size_(0) {}

AudioFrame::~AudioFrame() {
  if (!samples_) {
    delete [] samples_;

  }
}

MediaType AudioFrame::GetType() {
  return MediaTypeAudio;
}

VideoFrame::VideoFrame() {

}
VideoFrame::~VideoFrame() {
  if (!luma) {
    delete [] luma;
    luma = nullptr;
  }

  if (!chromaB) {
    delete [] chromaB;
    chromaB = nullptr;
  }

  if (!chromaR) {
    delete [] chromaR;
    chromaR = nullptr;
  }
}

MediaType VideoFrame::GetType() {
  return MediaTypeVideo;
}
