#define LOG_TAG "QomoOpenSLJNI"

#include "opensles_player.h"
#include "logging.h"

#include <jni.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_qomo_qomoopensl_QomoOpenSl_nativeCreate(JNIEnv *env, jobject thiz) {
  auto *player = new OpenSlEsPlayer();
  return reinterpret_cast<jlong>(player);
}

JNIEXPORT jint JNICALL
Java_com_qomo_qomoopensl_QomoOpenSl_nativeInit(JNIEnv *env, jobject thiz, jlong native_handle,
                                               jstring source_path,
                                               jint sample_rate,
                                               jint buffer_size,
                                               jint channels) {
  auto *player = reinterpret_cast<OpenSlEsPlayer *>(native_handle);
  if (!player) {
    LOGE("error: invalid handle");
    return -1;
  }

  const char *source_path_str = env->GetStringUTFChars(source_path, nullptr);
  int result = player->Initialize(source_path_str, sample_rate, buffer_size, channels);
  env->ReleaseStringUTFChars(source_path, source_path_str);
  return result;
}

JNIEXPORT void JNICALL
Java_com_qomo_qomoopensl_QomoOpenSl_nativeStart(JNIEnv *env, jobject thiz, jlong native_handle) {
  auto *player = reinterpret_cast<OpenSlEsPlayer *>(native_handle);
  if (!player) {
    LOGE("error: invalid handle");
  }

  player->Start();
}

JNIEXPORT void JNICALL
Java_com_qomo_qomoopensl_QomoOpenSl_nativeStop(JNIEnv *env,
                                               jobject thiz,
                                               jlong native_handle) {
  auto *player = reinterpret_cast<OpenSlEsPlayer *>(native_handle);
  if (!player) {
    LOGE("error: invalid handle");
    return;
  }

  player->Stop();
}

JNIEXPORT void JNICALL
Java_com_qomo_qomoopensl_QomoOpenSl_nativeRelease(JNIEnv *env,
                                                  jobject thiz,
                                                  jlong native_handle) {
  auto *player = reinterpret_cast<OpenSlEsPlayer *>(native_handle);
  if (!player) {
    LOGE("error: invalid handle");
    return;
  }

  player->UnInitialize();
  player->Destroy();
}

#ifdef __cplusplus
}
#endif