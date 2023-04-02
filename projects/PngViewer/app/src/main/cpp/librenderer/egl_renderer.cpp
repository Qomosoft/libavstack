#define LOG_TAG "EglRenderer"

#include "egl_renderer.h"
#include "shader.h"
#include "logging.h"

static const std::vector<uint8_t> kRed = {0xff, 0, 0, 0xff};
static const std::vector<uint8_t> kGreen = {0, 0xff, 0, 0xff};
static const std::vector<uint8_t> kBlue = {0, 0, 0xff, 0xff};

static const GLuint kAttribIndexVertex = 0;
static const GLuint kAttribIndexTexcoord = 1;

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
    egl_surface_(nullptr),
    egl_(nullptr),
    shader_(nullptr) {}

EglRenderer::~EglRenderer() {}

bool EglRenderer::Start() {
  LOGI("called");
  render_thread_ = std::make_unique<std::thread>(&EglRenderer::RenderLoop, this);
  return true;
}

void EglRenderer::Stop() {
  LOGI("called");
  /*send message to render thread to Stop rendering*/
  {
    std::unique_lock<std::mutex> lock(mutex_);
    msg_queue_.push(MSG_RENDER_LOOP_EXIT);
  }
  cv_.notify_all();
  render_thread_->join();
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
  LOGI("width:%d; height:%d", width, height);
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
  LOGI("start");
  while (true) {
    std::unique_lock<std::mutex> lock(mutex_);
    /*process incoming messages*/
    if (!msg_queue_.empty()) {
      int msg = msg_queue_.front();
      msg_queue_.pop();
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
  LOGI("stop");

  return;
}

bool EglRenderer::Initialize() {
  LOGI("called");
  egl_ = std::make_unique<QomoEgl>();
  egl_->Initialize();
  egl_surface_ = egl_->CreateWindowSurface(window_);
  egl_->MakeCurrent(egl_surface_);

  texture_ = CreateTexture();
  UpdateTexImage();

  shader_ = std::make_unique<Shader>(kVertexShader, kFragmentShader);
  shader_->BindAttribLocation(kAttribIndexTexcoord, "position");
  shader_->BindAttribLocation(kAttribIndexTexcoord, "texcoord");
  shader_->Use();

  return true;
}

void EglRenderer::Destroy() {
  LOGI("called");
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
  LOGI("called");
  glViewport(0, 0, width_, height_);
  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader_->Use();
  static const GLfloat vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
  glVertexAttribPointer(kAttribIndexVertex, 2, GL_FLOAT, 0, 0, vertices);
  glEnableVertexAttribArray(kAttribIndexVertex);
  static const GLfloat tex_coords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
  glVertexAttribPointer(kAttribIndexTexcoord, 2, GL_FLOAT, 0, 0, tex_coords);
  glEnableVertexAttribArray(kAttribIndexTexcoord);
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
