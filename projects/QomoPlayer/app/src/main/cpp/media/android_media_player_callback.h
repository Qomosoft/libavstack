//
// Created by Robin on 2024/4/9.
//

#pragma once
#include "media_player_callback.h"

#include <jni.h>

class AndroidMediaPlayerCallback : public MediaPlayerCallback {
 public:
  AndroidMediaPlayerCallback(JavaVM *jvm, jobject callback_obj);
  ~AndroidMediaPlayerCallback();
  virtual void OnCompletion();

 private:
  JNIEnv *AttachCurrentThread();
  void DetachCurrentThread();

  JavaVM *jvm_ = nullptr;
  jobject callback_obj_ = nullptr;
};