#include "pic_preview_controller.h"

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#define LOG_TAG "PngViewer"

static ANativeWindow *window = nullptr;
static PicPreviewController *controller = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_init(JNIEnv *env, jobject thiz, jstring pic_path) {
    controller = new PicPreviewController();
    char *png_path = const_cast<char*>(env->GetStringUTFChars(pic_path, nullptr));
    controller->start(png_path);
    env->ReleaseStringUTFChars(pic_path, png_path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_setSurface(JNIEnv *env, jobject thiz,
                                             jobject surface) {
    if (nullptr != surface && nullptr != controller) {
        window = ANativeWindow_fromSurface(env, surface);
        LOGI("Get window %p", window);
        controller->setWindow(window);
    } else if(nullptr != window) {
        LOGI("Releasing window");
        ANativeWindow_release(window);
        window = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_resetSize(JNIEnv *env, jobject thiz, jint width,
                                            jint height) {
    if (nullptr != controller) {
        controller->resetSize(width, height);
    } else {
        LOGE("controller is null");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_stop(JNIEnv *env, jobject thiz) {
    if (nullptr != controller) {
        controller->stop();
        delete controller;
        controller = nullptr;
    } else {
        LOGE("controller is null");
    }
}