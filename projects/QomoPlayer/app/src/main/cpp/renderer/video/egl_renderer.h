#pragma once

extern "C" {
#include "libswscale/swscale.h"
}

#include "qomo_egl.h"
#include "shader.h"
#include "libavutil/frame.h"
#include "frame.h"
//#include "rgb_generator.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <thread>
#include <memory>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

class EglRenderer {
 public:
  EglRenderer();

  ~EglRenderer();

  int Initialize();

  int SetWindow(ANativeWindow *window);

  int Start();

  int Stop();

  int SetWindowSize(int width, int height);

  void DrawFrame(Frame* frame);

  void DrawRgb(Frame *frame, int frame_width, int frame_height);

  void DrawYuv(Frame *frame, int width, int height);

 private:

  template <typename F, typename...Args>
  void PostOnRenderThread(F &&f, Args &&...args);

  void RenderLoop();

  GLuint CreateRgbTexture();

  bool CheckGlError(const char *op);

 private:
  bool render_stop_;
  std::unique_ptr<QomoEgl> egl_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<std::thread> render_thread_;
  std::queue<std::function<void()>> render_tasks_;
  std::condition_variable cv_;
  std::mutex queue_mutex_;
  ANativeWindow *window_;
  EGLSurface egl_surface_;
  GLuint rgb_texture_;
  std::vector<GLuint> yuv_textures_;
  int width_;
  int height_;
};