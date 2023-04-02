#ifndef PIC_PREVIEW_CONTROLLER_H
#define PIC_PREVIEW_CONTROLLER_H

#include "qomo_egl.h"
#include "shader.h"

#include <queue>
#include <unistd.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class EglRenderer {
 public:
  EglRenderer();
  ~EglRenderer();

  bool Start();
  void Stop();
  void SetWindow(ANativeWindow* window);
  void OnWindowSizeChanged(int width, int height);

 private:
  GLuint CreateTexture();
  bool CheckGlError(const char *op);
  // Helper method for starting the thread
  // RenderLoop is called in a rendering thread started in Start() method
  // It creates rendering context and renders scene until Stop() is called
  void RenderLoop();
  bool Initialize();
  void UpdateTexImage();
  void DrawFrame();
  void Destroy();

 private:
  int width_;
  int height_;
  enum RenderThreadMessage {
    MSG_NONE = 0, MSG_WINDOW_SET, MSG_RENDER_LOOP_EXIT
  };

  std::unique_ptr<std::thread> render_thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<int> msg_queue_;

  // android g_window, supported by NDK r5 and newer
  ANativeWindow* window_;

  std::unique_ptr<QomoEgl> egl_;
  std::unique_ptr<Shader> shader_;
  EGLSurface egl_surface_;
  GLuint texture_;

};

#endif // PIC_PREVIEW_CONTROLLER_H
