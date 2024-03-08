#define LOG_TAG "QomoPlayer"

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
#include <android/log.h>

extern "C" JNIEXPORT jstring
JNICALL
Java_com_qomo_qomoplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::stringstream ss;
    ss << "libavcodec:" << AV_STRINGIFY(LIBAVCODEC_VERSION) <<
       "\nlibavformat:" << AV_STRINGIFY(LIBAVFORMAT_VERSION) <<
       "\nlibavutil:" << AV_STRINGIFY(LIBAVUTIL_VERSION) <<
       "\nlibavfilter:" << AV_STRINGIFY(LIBAVFILTER_VERSION) <<
       "\nlibavswresample:" << AV_STRINGIFY(LIBSWRESAMPLE_VERSION) <<
       "\nlibavswscale:" << AV_STRINGIFY(LIBSWSCALE_VERSION) <<
       "\navcodec_configure:" << avcodec_configuration() <<
       "\navcodec_license:" << avcodec_license();
    std::string hello = ss.str();

    av_log_set_callback(ff_log_callback);
    av_log_set_level(AV_LOG_TRACE);
    av_log(nullptr, AV_LOG_DEBUG, "%s", "Log from av_log");
    LOGE("Log from android");
    return env->NewStringUTF(hello.c_str());
}