#define LOG_TAG "MediaPlayer"

extern "C" {
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
}

#include <jni.h>
#include <string>
#include <sstream>
#include "logging.h"
#include "media/media_player.h"
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1init(JNIEnv *env, jclass clazz, jint decoder_type) {
  //TODO: create native player engine
  MediaPlayer *engine = new MediaPlayer();
  engine->Init(decoder_type);
  return reinterpret_cast<jlong>(engine);
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1setup(JNIEnv *env, jobject thiz) {
  // TODO: implement native_setup()
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1finalize(JNIEnv *env, jobject thiz,
                                                           jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");

  delete engine;
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1setVideoScalingMode(JNIEnv *env, jobject thiz,
                                                                      jlong native_handle,
                                                                      jint mode) {
  // TODO: implement native_setVideoScalingMode()
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1setDisplay(JNIEnv *env, jobject thiz,
                                                             jlong native_handle, jobject sh) {
  // TODO: implement native_setDisplay()
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1setSurface(JNIEnv *env, jobject thiz,
                                                             jlong native_handle, jobject surface) {
  // TODO: implement native_setSurface()
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1setDataSource(JNIEnv *env, jobject thiz,
                                                                jlong native_handle, jstring path) {
  // TODO: implement native_setDataSource()
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  const char *native_path = env->GetStringUTFChars(path, nullptr);
  engine->SetDataSource(native_path);
  env->ReleaseStringUTFChars(path, native_path);
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1prepare(JNIEnv *env, jobject thiz,
                                                          jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  engine->Prepare();
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1prepareAsync(JNIEnv *env, jobject thiz,
                                                               jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  engine->PrepareAsync();
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1start(JNIEnv *env, jobject thiz,
                                                        jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  engine->Start();
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1pause(JNIEnv *env, jobject thiz,
                                                        jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  engine->Pause();
}
JNIEXPORT void JNICALL
Java_com_qomo_qomoplayer_media_QomoPlayer_native_1stop(JNIEnv *env, jobject thiz,
                                                       jlong native_handle) {
  MediaPlayer *engine = reinterpret_cast<MediaPlayer *>(native_handle);
  CHECK(!engine, "engine is null");
  engine->Stop();
}

#ifdef __cplusplus
}
#endif