#define LOG_TAG "QomoPlayer"

extern "C" {
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
}

#include <jni.h>
#include <string>
#include "logging.h"
#include <android/log.h>

extern "C" JNIEXPORT jstring
JNICALL
Java_com_qomo_qomoplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
//    std::string hello = "Hello from C++";
    std::string hello = avcodec_configuration();
    av_log_set_callback(ff_log_callback);
    av_log_set_level(AV_LOG_TRACE);
    av_log(nullptr, AV_LOG_DEBUG, "%s", "Log from av_log");
    LOGE("Log from android");
    return env->NewStringUTF(hello.c_str());
}