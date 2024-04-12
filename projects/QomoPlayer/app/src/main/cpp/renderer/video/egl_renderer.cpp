#define LOG_TAG "EglRenderer"

extern "C" {
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

#include "egl_renderer.h"
#include "logging.h"

static const AVPixelFormat kOutPixFmt = AV_PIX_FMT_RGBA;

static const char* kVertexShader =
    "attribute vec4 position;   \n"
    "attribute vec2 texcoord;   \n"
    "varying vec2 v_texcoord;   \n"
    "void main(void)            \n"
    "{                          \n"
    "   gl_Position = position; \n"
    "   v_texcoord = texcoord;  \n"
    "}                          \n";

static const char* kFragmentShader =
    "varying highp vec2 v_texcoord;                         \n"
    "uniform sampler2D yuvTexSampler;                       \n"
    "void main() {                                          \n"
    "  gl_FragColor = texture2D(yuvTexSampler, v_texcoord); \n"
    "}                                                      \n";

EglRenderer::EglRenderer()
    : render_stop_(false),
      attribute_vertex_index_(0),
      attribute_texcoord_index_(1) {}

EglRenderer::~EglRenderer() {
  if (!render_stop_) Stop();
}

int EglRenderer::Initialize() {
  egl_ = std::make_unique<QomoEgl>();
  egl_->Initialize();

  return 0;
}

int EglRenderer::SetWindow(ANativeWindow *window) {
  LOGI("%p", window);
  if (!window) {
    LOGE("window is null");
    return -1;
  }

  PostOnRenderThread([=] {
    window_ = window;
    egl_surface_ = egl_->CreateWindowSurface(window_);
    egl_->MakeCurrent(egl_surface_);

    rgb_texture_ = CreateRgbTexture();
    LOGI("CreateRgbTexture rgb_texture=%d", rgb_texture_);

    shader_ = std::make_unique<Shader>(kVertexShader, kFragmentShader);
    shader_->BindAttribLocation(attribute_vertex_index_, "position");
    shader_->BindAttribLocation(attribute_texcoord_index_, "texcoord");
    shader_->Use();
  });
  return 0;
}

int EglRenderer::Start() {
  render_thread_ = std::make_unique<std::thread>(&EglRenderer::RenderLoop, this);
  return 0;
}

int EglRenderer::Stop() {
  LOGI("Stop");
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    render_stop_ = true;
  }

  cv_.notify_all();
  render_thread_->join();
  if (sws_context_) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  return 0;
}

int EglRenderer::SetWindowSize(int width, int height) {
//  LOGI("width=%d, height=%d", width, height);
  width_ = width;
  height_ = height;

  return 0;
}

void EglRenderer::DrawRgb(uint8_t *frame, int frame_width, int frame_height) {
//  LOGI("frame_width=%d, frame_height=%d", frame_width, frame_height);
  PostOnRenderThread([=] {
//    LOGI("start");
    SetWindowSize(frame_width, frame_height);

    // load rgb data to texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rgb_texture_);
    if (CheckGlError("glBindTexture")) {
      return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    if (CheckGlError("glTexImage2D")) {
      return;
    }

    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_->Use();
    static const GLfloat vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
    glVertexAttribPointer(attribute_vertex_index_, 2, GL_FLOAT, 0, 0, vertices);
    glEnableVertexAttribArray(attribute_vertex_index_);
    static const GLfloat tex_coords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    glVertexAttribPointer(attribute_texcoord_index_, 2, GL_FLOAT, 0, 0, tex_coords);
    glEnableVertexAttribArray(attribute_texcoord_index_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (egl_->SwapBuffers(egl_surface_) != 0) LOGE("SwapBuffers failed");
//    LOGI("end");
  });
}

template<typename F, typename... Args>
void EglRenderer::PostOnRenderThread(F &&f, Args &&... args) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    if (render_stop_) {
      LOGE("enqueue task to stopped render thread");
      return;
    }

    render_tasks_.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  }

  cv_.notify_one();
}

void EglRenderer::RenderLoop() {
  LOGI("start");
  while (true) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    cv_.wait(lock, [=] { return render_stop_ || !render_tasks_.empty(); });
    if (render_stop_ && render_tasks_.empty()) {
      LOGI("stop");
      return;
    }

    auto task = std::move(render_tasks_.front());
    this->render_tasks_.pop();
    task();
  }
}

GLuint EglRenderer::CreateRgbTexture() {
  GLuint texture = -1;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return texture;
}

bool EglRenderer::CheckGlError(const char* op) {
  GLint error;

  for (error = glGetError(); error; error = glGetError()) {
    LOGE("after %s() glError (0x%x)\n", op, error);
    return true;
  }

  return false;
}

