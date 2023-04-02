#include "logging.h"

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
