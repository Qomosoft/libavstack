#define LOG_TAG "PngViewer"
#include "egl_renderer.h"
#include "logging.h"

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>


static ANativeWindow *g_window = nullptr;
static EglRenderer *g_egl_render = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_init(JNIEnv *env, jobject thiz, jstring pic_path) {
  g_egl_render = new EglRenderer();
  char *png_path = const_cast<char*>(env->GetStringUTFChars(pic_path, nullptr));
  g_egl_render->Start();
  env->ReleaseStringUTFChars(pic_path, png_path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_setSurface(JNIEnv *env, jobject thiz,
                                             jobject surface) {
  if (nullptr != surface && nullptr != g_egl_render) {
    g_window = ANativeWindow_fromSurface(env, surface);
    LOGI("Get g_window %p", g_window);
    g_egl_render->SetWindow(g_window);
  } else if(nullptr != g_window) {
    LOGI("Releasing g_window");
    ANativeWindow_release(g_window);
    g_window = nullptr;
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_onWindowSizeChanged(JNIEnv *env, jobject thiz, jint width,
                                                      jint height) {
  if (nullptr != g_egl_render) {
    LOGI("OnWindowSizeChanged: %dx%d", width, height);
    g_egl_render->OnWindowSizeChanged(width, height);
  } else {
    LOGE("g_egl_render is null");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_stop(JNIEnv *env, jobject thiz) {
  if (nullptr != g_egl_render) {
    g_egl_render->Stop();
    delete g_egl_render;
    g_egl_render = nullptr;
  } else {
    LOGE("g_egl_render is null");
  }
}