//
// Created by Robin on 2024/4/9.
//
#define LOG_TAG "AndroidMediaPlayerCallback"

#include "android_media_player_callback.h"
#include "logging.h"

AndroidMediaPlayerCallback::AndroidMediaPlayerCallback(JavaVM *jvm, jobject callback_obj) {
  jvm_ = jvm;
  JNIEnv *env = AttachCurrentThread();
  callback_obj_ = env->NewGlobalRef(callback_obj);
//  DetachCurrentThread();
}

AndroidMediaPlayerCallback::~AndroidMediaPlayerCallback() {
  JNIEnv *env = AttachCurrentThread();
  env->DeleteGlobalRef(callback_obj_);
  DetachCurrentThread();
}

void AndroidMediaPlayerCallback::OnCompletion() {
  JNIEnv *env = AttachCurrentThread();
  jclass callback_class = env->GetObjectClass(callback_obj_);
  jmethodID callback_method_id = env->GetMethodID(callback_class, "onCompletion", "()V");
  env->CallVoidMethod(callback_obj_, callback_method_id);
//  DetachCurrentThread();
}

JNIEnv *AndroidMediaPlayerCallback::AttachCurrentThread() {
  JNIEnv *env = nullptr;
  if (jvm_->AttachCurrentThread(&env, nullptr) != JNI_OK) {
    LOGE("AttachCurrentThread failed");
  }

  return env;
}

void AndroidMediaPlayerCallback::DetachCurrentThread() {
  if (jvm_->DetachCurrentThread() != JNI_OK) {
    LOGE("DetachCurrentThread failed");
  }
}
