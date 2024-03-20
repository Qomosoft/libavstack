#include "logging.h"

extern "C" {
#include "libavutil/avutil.h"
}

#include <sstream>

void trace(int prio, const char *tag, const char *file, const int line, const char *func, const char *log_level, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
#ifdef __ANDROID__
  std::stringstream ss;
  ss << file << ":" << line << "[" << func << "]" << log_level << ": " << fmt;
  const char *new_fmt = ss.str().c_str();
  __android_log_vprint(prio, tag, new_fmt, argp);
#else
  printf("%s:%d[%s]%s: ", file, line, func, log_level);
  vprintf(fmt, argp);
#endif
  va_end(argp);
}

#ifdef __ANDROID__
android_LogPriority FFLevelToAndLevel(int ff_level) {
  switch (ff_level) {
    case AV_LOG_PANIC:
      return ANDROID_LOG_FATAL;
    case AV_LOG_ERROR:
      return ANDROID_LOG_ERROR;
    case AV_LOG_WARNING:
      return ANDROID_LOG_WARN;
    case AV_LOG_INFO:
      return ANDROID_LOG_INFO;
    case AV_LOG_DEBUG:
      return ANDROID_LOG_DEBUG;
    case AV_LOG_TRACE:
    default:
      return ANDROID_LOG_VERBOSE;
  }
}
#endif // __ANDROID__

void ff_log_callback(void *avcl, int level, const char *fmt, va_list vl) {
  if (level > av_log_get_level()) return;
#ifdef __ANDROID__
  __android_log_vprint(FFLevelToAndLevel(level), "FFmpeg", fmt, vl);
#else
    vprintf(fmt, argp);
#endif
}

