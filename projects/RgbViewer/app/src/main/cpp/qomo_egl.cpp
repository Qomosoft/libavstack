#define LOG_TAG "QomoEgl"

#include "qomo_egl.h"
#include "logging.h"

QomoEgl::QomoEgl() : egl_context_(EGL_NO_CONTEXT), egl_display_(EGL_NO_DISPLAY) {}

QomoEgl::~QomoEgl() {
  egl_context_ = EGL_NO_CONTEXT;
  egl_display_ = EGL_NO_DISPLAY;
}

int QomoEgl::Initialize() {
  return Initialize(nullptr);
}

int QomoEgl::Initialize(EGLContext shared_ctx) {
  LOGI("Initialize shared_ctx=%p", shared_ctx);
  EGLint num_configs;

  const EGLint attribs[] = {
      EGL_BUFFER_SIZE, 32,
      EGL_ALPHA_SIZE,   8,
      EGL_BLUE_SIZE,    8,
      EGL_GREEN_SIZE,   8,
      EGL_RED_SIZE,     8,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
  };

  if ((egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    LOGE("eglGetDisplay return %d", eglGetError());
    return -1;
  }

  if (!eglInitialize(egl_display_, 0, 0)) {
    LOGE("eglInitialize return %d", eglGetError());
    return -1;
  }

  if (!eglChooseConfig(egl_display_, attribs, &egl_config_, 1, &num_configs)) {
    LOGE("eglChooseConfig return %d", eglGetError());
    Release();
    return -1;
  }

  EGLint egl_ctx_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  if (!(egl_context_ = eglCreateContext(egl_display_, egl_config_,
                                       shared_ctx == nullptr ? EGL_NO_CONTEXT : shared_ctx,
                                       egl_ctx_attributes))) {
    LOGE("eglCreateContext return %d", eglGetError());
    Release();
    return -1;
  }

  return 0;
}

EGLSurface QomoEgl::CreateWindowSurface(ANativeWindow *window) {
  EGLSurface egl_surface = EGL_NO_SURFACE;
  EGLint format;
  if (!eglGetConfigAttrib(egl_display_, egl_config_, EGL_NATIVE_VISUAL_ID, &format)) {
    LOGE("eglGetConfigAttrib return %d", eglGetError());
    Release();
    return egl_surface;
  }

  ANativeWindow_setBuffersGeometry(window, 0, 0, format);
  if ((egl_surface = eglCreateWindowSurface(egl_display_, egl_config_, window, 0)) == nullptr) {
    LOGE("eglCreateWindowSurface return %d", eglGetError());
  }

  return egl_surface;
}

EGLSurface QomoEgl::CreateOffscreenSurface(int width, int height) {
  EGLSurface egl_surface;
  EGLint pbuffer_attributes[] = {
      EGL_WIDTH,
      width,
      EGL_HEIGHT,
      height,
      EGL_NONE,
      EGL_NONE
  };

  if ((egl_surface = eglCreatePbufferSurface(egl_display_, egl_config_, pbuffer_attributes)) == nullptr) {
    LOGE("eglCreatePbufferSurface return %d", eglGetError());
  }

  return egl_surface;
}

int QomoEgl::MakeCurrent(EGLSurface egl_surface) {
  if (!eglMakeCurrent(egl_display_, egl_surface, egl_surface, egl_context_)) {
    LOGE("eglMakeCurrent return %d", eglGetError());
    return -1;
  }

  return 0;
}

int QomoEgl::SwapBuffers(EGLSurface egl_surface) {
  if (!eglSwapBuffers(egl_display_, egl_surface)) {
    LOGE("eglSwapBuffers return %d", eglGetError());
    return -1;
  }

  return 0;
}

void QomoEgl::ReleaseSurface(EGLSurface egl_surface) {
  LOGI("ReleaseSurface egl_surface=%p", egl_surface);

  if (!eglDestroySurface(egl_display_, egl_surface)) {
    LOGE("eglDestroySurface return %d", eglGetError());
  }
}

void QomoEgl::Release() {
  LOGI("Release");

  if (!eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
    LOGE("eglMakeCurrent return %d", eglGetError());
  }

  if (!eglDestroyContext(egl_display_, egl_context_)) {
    LOGE("eglDestroyContext return %d", eglGetError());
  }

  egl_display_ = EGL_NO_DISPLAY;
  egl_context_ = EGL_NO_CONTEXT;
}

EGLDisplay QomoEgl::egl_display() const {
  return egl_display_;
}

EGLContext QomoEgl::egl_context() const {
  return egl_context_;
}

EGLConfig QomoEgl::egl_config() const {
  return egl_config_;
}
