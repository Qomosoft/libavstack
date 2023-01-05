#include <jni.h>

// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("pngviewer");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("pngviewer")
//      }
//    }
extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_init(JNIEnv *env, jobject thiz, jstring pic_path) {
    // TODO: implement init()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_setSurface(JNIEnv *env, jobject thiz,
                                             jobject surface) {
    // TODO: implement setSurface()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_resetSize(JNIEnv *env, jobject thiz, jint width,
                                            jint height) {
    // TODO: implement resetSize()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_qomo_pngviewer_PngViewer_stop(JNIEnv *env, jobject thiz) {
    // TODO: implement stop()
}