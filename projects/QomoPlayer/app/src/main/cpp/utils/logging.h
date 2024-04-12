#pragma once

#include "media_player_stats.h"

#include <cstdarg>
#include <cstdio>
#include <mutex>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifndef LOG_TAG
#define LOG_TAG "Logging"
#endif

void trace(int prio, const char *tag, const char *file, const int line, const char *func, const char *log_level, const char *fmt, ...);
void ff_log_callback(void *avcl, int level, const char *fmt, va_list vl);

#define LOGI(...) trace(ANDROID_LOG_INFO, LOG_TAG, __FILE__, __LINE__, __FUNCTION__, "info", ##__VA_ARGS__)
#define LOGW(...) trace(ANDROID_LOG_WARN, LOG_TAG, __FILE__, __LINE__, __FUNCTION__, "warn", ##__VA_ARGS__)
#define LOGE(...) trace(ANDROID_LOG_ERROR, LOG_TAG, __FILE__, __LINE__, __FUNCTION__, "error", ##__VA_ARGS__)

#define CHECK(state, msg) \
  if (state) {            \
    LOGE("%s", msg);      \
    return;               \
  }

#define TIME_EVENT(event) \
  static std::once_flag flag; \
  std::call_once(flag, [] { event = Stats::Now(); });
