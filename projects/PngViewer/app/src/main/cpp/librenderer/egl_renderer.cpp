#define LOG_TAG "EglRenderer"

#include "egl_renderer.h"
#include "shader.h"
#include "logging.h"

static const std::vector<uint8_t> kRed = {0xff, 0, 0, 0xff};
static const std::vector<uint8_t> kGreen = {0, 0xff, 0, 0xff};
static const std::vector<uint8_t> kBlue = {0, 0, 0xff, 0xff};

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

EglRenderer::EglRenderer() :
    egl_surface_(0), egl_(nullptr) {
  LOGI("VideoDutePlayerController instance created");
  shader_ = std::make_unique<Shader>(kVertexShader, kFragmentShader);
  this->width_ = 720;
  this->height_ = 720;
  attribute_vertex_index_ = 0;
  attribute_texcoord_index_ = 1;
}

EglRenderer::~EglRenderer() {
  LOGI("VideoDutePlayerController instance destroyed");
}

bool EglRenderer::Start() {
  LOGI("Creating VideoDutePlayerController thread");
  render_thread_ = std::make_unique<std::thread>(&EglRenderer::RenderLoop, this);
  return true;
}

void EglRenderer::Stop() {
  LOGI("Stopping VideoDutePlayerController Render thread");
  /*send message to render thread to Stop rendering*/
  {
    std::unique_lock<std::mutex> lock(mutex_);
    msg_queue_.push(MSG_RENDER_LOOP_EXIT);
  }
  cv_.notify_all();

  LOGI("we will join render thread Stop");
  render_thread_->join();
  LOGI("VideoDutePlayerController Render thread stopped");
}

void EglRenderer::SetWindow(ANativeWindow *window) {
  /*notify render thread that g_window has changed*/
  {
    std::unique_lock<std::mutex> lock(mutex_);
    msg_queue_.push(MSG_WINDOW_SET);
    window_ = window;
  }
  cv_.notify_one();
}

void EglRenderer::OnWindowSizeChanged(int width, int height) {
  LOGI("VideoDutePlayerController::OnWindowSizeChanged width:%d; height:%d", width, height);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    this->width_ = width;
    this->height_ = height;
    msg_queue_.push(MSG_NONE);
  }
  cv_.notify_one();
}

void EglRenderer::RenderLoop() {
  bool renderingEnabled = true;
  LOGI("RenderLoop()");
  while (true) {
    std::unique_lock<std::mutex> lock(mutex_);
    /*process incoming messages*/
    if (!msg_queue_.empty()) {
      int msg = msg_queue_.front();
      msg_queue_.pop();
      LOGI("looping msg: %d", msg);
      switch (msg) {
        case MSG_WINDOW_SET:
          Initialize();
          break;
        case MSG_RENDER_LOOP_EXIT:
          renderingEnabled = false;
          Destroy();
          break;
        default:
          break;
      }
    }

    if (egl_) {
      egl_->MakeCurrent(egl_surface_);
      DrawFrame();
    }

    cv_.wait(lock, [=] { return !renderingEnabled || !msg_queue_.empty(); });
    if (!renderingEnabled) break;
  }
  LOGI("Render loop exits");

  return;
}

bool EglRenderer::Initialize() {
  egl_ = std::make_unique<QomoEgl>();
  egl_->Initialize();
  egl_surface_ = egl_->CreateWindowSurface(window_);
  egl_->MakeCurrent(egl_surface_);

  texture_ = CreateTexture();
  this->UpdateTexImage();
  shader_->BindAttribLocation(attribute_texcoord_index_, "position");
  shader_->BindAttribLocation(attribute_texcoord_index_, "texcoord");
  shader_->Use();

  LOGI("Initializing context Success");
  return true;
}

void EglRenderer::Destroy() {
  LOGI("dealloc renderer ...");
  if (texture_) {
    glDeleteTextures(1, &texture_);
  }
  if (egl_) {
    egl_->ReleaseSurface(egl_surface_);
    egl_->Release();
    egl_ = nullptr;
  }
  return;
}

void EglRenderer::UpdateTexImage() {
  std::vector<uint8_t> data;
  int width = 210, height = 210;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      if (j < height / 3) {
        data.insert(data.end(), kRed.begin(), kRed.end());
      } else if (j >= height / 3 && j < height * 2 / 3) {
        data.insert(data.end(), kGreen.begin(), kGreen.end());
      } else {
        data.insert(data.end(), kBlue.begin(), kBlue.end());
      }
    }
  }

  // load rgba data to texture_
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  if (CheckGlError("glBindTexture")) {
    return;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

void EglRenderer::DrawFrame() {
  LOGI("frame_width=%d, frame_height=%d", width_, height_);
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
}

GLuint EglRenderer::CreateTexture() {
  GLuint texture;
  glGenTextures(1, &texture);
  if (CheckGlError("CreateTexture")) LOGE("CreateTexture failed");

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
    LOGI("error::after %s() glError (0x%x)\n", op, error);
    return true;
  }
  return false;
}
