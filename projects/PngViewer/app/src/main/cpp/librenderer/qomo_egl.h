#pragma once

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

class QomoEgl {
 public:
  QomoEgl();

  ~QomoEgl();

  int Initialize();

  int Initialize(EGLContext shared_ctx);

  EGLSurface CreateWindowSurface(ANativeWindow *window);

  EGLSurface CreateOffscreenSurface(int width, int height);

  int MakeCurrent(EGLSurface egl_surface);

  int SwapBuffers(EGLSurface egl_surface);

  void ReleaseSurface(EGLSurface egl_surface);

  void Release();

  EGLDisplay egl_display() const;

  EGLContext egl_context() const;

  EGLConfig egl_config() const;

 private:
  EGLDisplay egl_display_;
  EGLConfig egl_config_;
  EGLContext egl_context_;
};