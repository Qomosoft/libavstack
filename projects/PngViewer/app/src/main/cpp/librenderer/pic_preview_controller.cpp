#define LOG_TAG "PicPreviewController"

#include "pic_preview_controller.h"

static const std::vector<uint8_t> kRed = {0xff, 0, 0, 0xff};
static const std::vector<uint8_t> kGreen = {0, 0xff, 0, 0xff};
static const std::vector<uint8_t> kBlue = {0, 0, 0xff, 0xff};

PicPreviewController::PicPreviewController() :
        previewSurface(0), eglCore(0) {
	LOGI("VideoDutePlayerController instance created");
	renderer = new PicPreviewRender();
	this->screenWidth = 720;
	this->screenHeight = 720;
}

PicPreviewController::~PicPreviewController() {
	LOGI("VideoDutePlayerController instance destroyed");
}

bool PicPreviewController::start(char* spriteFilePath) {
	LOGI("Creating VideoDutePlayerController thread");
    render_thread_ = std::make_unique<std::thread>(&PicPreviewController::renderLoop, this);
	return true;
}

void PicPreviewController::stop() {
	LOGI("Stopping VideoDutePlayerController Render thread");
	/*send message to render thread to stop rendering*/
    {
        std::unique_lock<std::mutex> lock(mutex_);
		msg_queue_.push(MSG_RENDER_LOOP_EXIT);
    }
    cv_.notify_all();

	LOGI("we will join render thread stop");
    render_thread_->join();
	LOGI("VideoDutePlayerController Render thread stopped");
}

void PicPreviewController::setWindow(ANativeWindow *window) {
	/*notify render thread that window has changed*/
    {
        std::unique_lock<std::mutex> lock;
		msg_queue_.push(MSG_WINDOW_SET);
        _window = window;
    }
    cv_.notify_one();
}

void PicPreviewController::resetSize(int width, int height) {
	LOGI("VideoDutePlayerController::resetSize width:%d; height:%d", width, height);
    {
        std::unique_lock<std::mutex> lock;
        this->screenWidth = width;
        this->screenHeight = height;
        renderer->resetRenderSize(0, 0, width, height);
    }
    cv_.notify_one();
}

void PicPreviewController::renderLoop() {
	bool renderingEnabled = true;
	LOGI("renderLoop()");
	while (renderingEnabled) {
        std::unique_lock<std::mutex> lock;
		/*process incoming messages*/
		if (!msg_queue_.empty()) {
			int msg = msg_queue_.front();
			msg_queue_.pop();
			LOGI("looping msg: %d", msg);
			switch (msg) {
				case MSG_WINDOW_SET:
					initialize();
					break;
				case MSG_RENDER_LOOP_EXIT:
					renderingEnabled = false;
					destroy();
					break;
				default:
					break;
			}
		}

		if (eglCore) {
			eglCore->makeCurrent(previewSurface);
			this->drawFrame();
            cv_.wait(lock, [=] { return !renderingEnabled || msg_queue_.empty(); });
			if (!renderingEnabled && msg_queue_.empty()) break;
		}
	}
	LOGI("Render loop exits");

	return;
}

bool PicPreviewController::initialize() {
	eglCore = new EGLCore();
	eglCore->init();
	previewSurface = eglCore->createWindowSurface(_window);
	eglCore->makeCurrent(previewSurface);

    texture = createTexture();
	this->updateTexImage();

	bool isRendererInitialized = renderer->init(screenWidth, screenHeight, texture);
	if (!isRendererInitialized) {
		LOGI("Renderer failed on initialized...");
		return false;
	}
	LOGI("Initializing context Success");
	return true;
}

void PicPreviewController::destroy() {
	LOGI("dealloc renderer ...");
	if (NULL != renderer) {
		renderer->dealloc();
		delete renderer;
		renderer = NULL;
	}
	if(eglCore){
		eglCore->releaseSurface(previewSurface);
		eglCore->release();
		eglCore = NULL;
	}
	return;
}

void PicPreviewController::updateTexImage() {
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

	// load rgba data to texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (checkGlError("glBindTexture")) {
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

void PicPreviewController::drawFrame() {
    renderer->render();
	if (!eglCore->swapBuffers(previewSurface)) {
		LOGE("eglSwapBuffers() returned error %d", eglGetError());
	}
}

GLuint PicPreviewController::createTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    if (checkGlError("createTexture")) LOGE("createTexture failed");

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

bool PicPreviewController::checkGlError(const char* op) {
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
        return true;
    }
    return false;
}
