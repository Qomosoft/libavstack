#pragma once

extern "C" {
#include "libswscale/swscale.h"
}

#include "qomo_egl.h"
#include "shader.h"
#include "libavutil/frame.h"
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

  void DrawRgb(AVFrame *frame, int frame_width, int frame_height);

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
//  std::unique_ptr<RgbGenerator> rgb_generator_;
  std::queue<std::function<void()>> render_tasks_;
  std::condition_variable cv_;
  std::mutex queue_mutex_;
  ANativeWindow *window_;
  EGLSurface egl_surface_;
  GLuint rgb_texture_;
  int attribute_vertex_index_;
  int attribute_texcoord_index_;
  int width_;
  int height_;
  SwsContext *sws_context_ = nullptr;
  std::vector<uint8_t> rgb_data_;
};