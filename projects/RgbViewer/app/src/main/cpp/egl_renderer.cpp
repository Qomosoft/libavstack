#include "egl_renderer.h"
#include "logging.h"

#define LOG_TAG "EglRenderer"

static const char* kVertexShader =
    "#version 300 es                          \n"
    "layout(location = 0) in vec4 position;   \n"
    "layout(location = 1) in vec2 texcoord;   \n"
    "out vec2 v_texcoord;                     \n"
    "void main(void)                          \n"
    "{                                        \n"
    "   gl_Position = position;               \n"
    "   v_texcoord = texcoord;                \n"
    "}                                        \n";

static const char* kFragmentShader =
    "#version 300 es                                        \n"
    "in vec2 v_texcoord;                                    \n"
    "out vec4 fragColor;                                    \n"
    "uniform sampler2D rgbTexSampler;                       \n"
    "void main() {                                          \n"
    "  fragColor = texture(rgbTexSampler, v_texcoord);      \n"
    "}                                                      \n";

EglRenderer::EglRenderer()
    : render_stop_(false),
      attribute_vertex_index_(0),
      attribute_texcoord_index_(1) {}

EglRenderer::~EglRenderer() {
  if (!render_stop_) Stop();
}

int EglRenderer::Initialize(const std::vector<uint8_t>& data) {
  data_ = data;
  PostOnRenderThread([=] {
    egl_ = std::make_unique<QomoEgl>();
    egl_->Initialize();
    egl_surface_ = egl_->CreateWindowSurface(window_);
    egl_->MakeCurrent(egl_surface_);

    rgb_texture_ = CreateRgbTexture();
    LOGI("CreateRgbTexture rgb_texture=%d", rgb_texture_);

    shader_ = std::make_unique<Shader>(kVertexShader, kFragmentShader);
    shader_->Use();
  });

  return 0;
}

int EglRenderer::SetWindow(ANativeWindow *window) {
  LOGI("%p", window);
  if (!window) {
    LOGE("window is null");
    return -1;
  }

  window_ = window;
  return 0;
}

int EglRenderer::Start() {
  render_thread_ = std::make_unique<std::thread>(&EglRenderer::RenderLoop, this);
  return 0;
}

int EglRenderer::Stop() {
  LOGI("Stop");
  {
    std::unique_lock<std::mutex> lock;
    render_stop_ = true;
  }

  cv_.notify_all();
  render_thread_->join();
  return 0;
}

int EglRenderer::SetWindowSize(int width, int height) {
  LOGI("width=%d, height=%d", width, height);
  width_ = width;
  height_ = height;

  rgb_generator_ = std::make_unique<RgbGenerator>(200, 200);
  DrawRgb(rgb_generator_->data(), 200, 200);

  return 0;
}

void EglRenderer::DrawRgb(const std::vector<uint8_t> &rgb, int frame_width, int frame_height) {
  LOGI("frame_width=%d, frame_height=%d", frame_width, frame_height);
  PostOnRenderThread([=] {
    LOGI("start");

    // load rgb data to texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rgb_texture_);
    if (CheckGlError("glBindTexture")) {
      return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb.data());
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
    LOGI("end");
  });
}

template<typename F, typename... Args>
void EglRenderer::PostOnRenderThread(F &&f, Args &&... args) {
  {
    std::unique_lock<std::mutex> lock;
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

