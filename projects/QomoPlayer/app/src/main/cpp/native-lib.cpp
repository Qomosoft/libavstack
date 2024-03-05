extern "C" {
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"
#include "libswresample/swresample.h"
#include "libavutil/samplefmt.h"
#include "libavutil/timestamp.h"
#include "libavformat/avformat.h"
}

#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring

JNICALL
Java_com_qomo_qomoplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
//    std::string hello = "Hello from C++";
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}