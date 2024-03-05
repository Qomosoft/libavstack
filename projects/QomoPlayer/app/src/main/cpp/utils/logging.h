#pragma once

#include <cstdarg>
#include <cstdio>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifndef LOG_TAG
#define LOG_TAG "Logging"
#endif

void trace(int prio, const char *tag, const char *file, const int line, const char *func, const char *log_level, const char *fmt, ...);
void ff_log_callback(void *avcl, int level, const char *fmt, va_list vl);

#define LOGI(...) trace(ANDROID_LOG_INFO, LOG_TAG, __FILE__, __LINE__, __FUNCTION__, "info", ##__VA_ARGS__)
#define LOGE(...) trace(ANDROID_LOG_ERROR, LOG_TAG, __FILE__, __LINE__, __FUNCTION__, "error", ##__VA_ARGS__)
