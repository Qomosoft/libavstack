#define LOG_TAG "EglRenderer"

extern "C" {
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
}

#include "egl_renderer.h"
#include "logging.h"
#include "block_profiler.h"
#include "frame.h"

#define GLSL(version, shader) "#version " #version "\n" #shader
#define GLSL300(shader) GLSL(300 es, shader)

static const char* kVertexShader = GLSL300(
                                       layout(location = 0) in vec4 position;
                                       layout(location = 1) in vec2 texcoord;
                                       out vec2 v_texcoord;
                                       void main()
                                       {
                                         gl_Position = position;
                                         v_texcoord = texcoord;
                                       }
                                   );

static const char* kRgbFragmentShader = GLSL300(
                                            in vec2 v_texcoord;
                                            out vec4 fragColor;
                                            uniform sampler2D rgbTexSampler;
                                            void main()
                                            {
                                              fragColor = texture(rgbTexSampler, v_texcoord);
                                            }
                                        );

static const char* kYuvFragmentShader = GLSL300(
                                            precision mediump float;
                                            in vec2 v_texcoord;
                                            out vec4 gl_FragColor;

                                            uniform sampler2D yTexture;
                                            uniform sampler2D uTexture;
                                            uniform sampler2D vTexture;

                                            void main() {
                                              vec3 yuv;
                                              vec3 rgb;
                                              yuv.x = texture(yTexture, v_texcoord).g;
                                              yuv.y = texture(uTexture, v_texcoord).g - 0.5;
                                              yuv.z = texture(vTexture, v_texcoord).g - 0.5;
                                              rgb = mat3(
                                                  1.0, 1.0, 1.0,
                                                  0.0, -0.39465, 2.03211,
                                                  1.13983, -0.5806, 0.0
                                              ) * yuv;
                                              gl_FragColor = vec4(rgb, 1.0);
                                            }
                                        );

EglRenderer::EglRenderer() :
    render_stop_(false),
    rgb_texture_(-1) {}

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
  return 0;
}

int EglRenderer::SetWindowSize(int width, int height) {
//  LOGI("width=%d, height=%d", width, height);
  width_ = width;
  height_ = height;

  return 0;
}

void EglRenderer::DrawFrame(Frame *frame) {
  if (frame->GetPixFmt() == AV_PIX_FMT_RGB32) {
    DrawRgb(frame, frame->GetWidth(), frame->GetHeight());
  } else if (frame->GetPixFmt() == AV_PIX_FMT_YUV420P) {
    DrawYuv(frame, frame->GetWidth(), frame->GetHeight());
  }
}

void EglRenderer::DrawRgb(Frame *frame, int frame_width, int frame_height) {
//  LOGI("frame_width=%d, frame_height=%d", frame_width, frame_height);
  PostOnRenderThread([=] {
//    BlockProfiler profiler("DrawRgb");
//    LOGI("start");
    if (rgb_texture_ == -1) {
      rgb_texture_ = CreateRgbTexture();
      LOGI("CreateRgbTexture rgb_texture=%d", rgb_texture_);
      shader_ = std::make_unique<Shader>(kVertexShader, kRgbFragmentShader);
    }
    SetWindowSize(frame_width, frame_height);

    // load rgb data to texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rgb_texture_);
    if (CheckGlError("glBindTexture")) {
      return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->GetData());
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
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertices);
    glEnableVertexAttribArray(0);
    static const GLfloat tex_coords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, tex_coords);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (egl_->SwapBuffers(egl_surface_) != 0) LOGE("SwapBuffers failed");
    TIME_EVENT(Stats::first_video_frame_rendered_time_pt);
//    LOGI("end");
  });
}

void EglRenderer::DrawYuv(Frame *frame, int width, int height) {
//  LOGI("width=%d, height=%d", width, height);
  PostOnRenderThread([=] {
//    LOGI("start");

//    BlockProfiler profiler("DrawYuv");
    if (yuv_textures_.empty()) {
      yuv_textures_.resize(3);
      glGenTextures(3, yuv_textures_.data());
      shader_ = std::make_unique<Shader>(kVertexShader, kYuvFragmentShader);
    }
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_->Use();

    static const GLfloat vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, vertices);
    glEnableVertexAttribArray(0);
    static const GLfloat tex_coords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, tex_coords);
    glEnableVertexAttribArray(1);

    // yuv textures sampler
    shader_->SetInt("yTexture", 0);
    shader_->SetInt("uTexture", 1);
    shader_->SetInt("vTexture", 2);

    const uint8_t* data_y = frame->GetData();
    const uint8_t* data_u = frame->GetData() + width * height;
    const uint8_t* data_v = frame->GetData() + width * height * 5 / 4;

    //textures[0]
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图（这里就是只取一个亮度的颜色通道的意思）
                 width,//加载的纹理宽度。最好为2的次幂(这里对y分量数据当做指定尺寸算，但显示尺寸会拉伸到全屏？)
                 height,//加载的纹理高度。最好为2的次幂
                 0,//纹理边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[1]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图（这里就是只取一个颜色通道的意思）
                 width / 2,//u数据数量为屏幕的4分之1
                 height / 2,
                 0,//边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[2]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图（这里就是只取一个颜色通道的意思）
                 width / 2,
                 height / 2,//v数据数量为屏幕的4分之1
                 0,//边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //激活第一层纹理，绑定到创建的纹理
    //下面的width,height主要是显示尺寸？
    glActiveTexture(GL_TEXTURE0);
    //绑定y对应的纹理
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[0]);
    //替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,//相对原来的纹理的offset
                    width, height,//加载的纹理宽度、高度。最好为2的次幂
                    GL_LUMINANCE, GL_UNSIGNED_BYTE,
                    data_y);

    //激活第二层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE1);
    //绑定u对应的纹理
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[1]);
    //替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    data_u);

    //激活第三层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE2);
    //绑定v对应的纹理
    glBindTexture(GL_TEXTURE_2D, yuv_textures_[2]);
    //替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    data_v);

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

