#include <jni.h>
#include <android/native_window_jni.h>

#include "egl_renderer.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_qomo_rgbviewer_RgbViewer_nativeCreate(JNIEnv *env, jobject thiz) {
  EglRenderer *renderer = new EglRenderer();
  if (!renderer) LOGE("EglRender ctor failed");

  return reinterpret_cast<jlong >(renderer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_rgbviewer_RgbViewer_nativeDestroy(JNIEnv *env, jobject thiz,
                                                jlong nativeHandler) {
  EglRenderer *renderer = reinterpret_cast<EglRenderer*>(nativeHandler);
  if (!renderer) {
    LOGE("nativeHandler is null");
    return;
  }

  renderer->Stop();
  delete renderer;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_qomo_rgbviewer_RgbViewer_nativeInitialize(JNIEnv *env, jobject thiz,
                                                   jlong nativeHandler,
                                                   jobject surface) {
  EglRenderer *renderer = reinterpret_cast<EglRenderer*>(nativeHandler);
  if (!renderer) {
    LOGE("nativeHandler is null");
    return -1;
  }

  if (!surface) {
    LOGE("surface is null");
    return -1;
  }
  ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
  renderer->SetWindow(window);
  renderer->Start();

  return renderer->Initialize();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_qomo_rgbviewer_RgbViewer_nativeOnWindowSizeChanged(JNIEnv *env, jobject thiz,
                                                            jlong nativeHandler,
                                                            jint width,
                                                            jint height) {
  EglRenderer *renderer = reinterpret_cast<EglRenderer*>(nativeHandler);
  if (!renderer) {
    LOGE("nativeHandler is null");
    return -1;
  }

  return renderer->SetWindowSize(width, height);
}