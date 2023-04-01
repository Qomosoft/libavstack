#include "./pic_preview_controller.h"

#define LOG_TAG "PicPreviewController"

static const std::vector<uint8_t> kRed = {0xff, 0, 0, 0xff};
static const std::vector<uint8_t> kGreen = {0, 0xff, 0, 0xff};
static const std::vector<uint8_t> kBlue = {0, 0, 0xff, 0xff};

PicPreviewController::PicPreviewController() :
		_msg(MSG_NONE), previewSurface(0), eglCore(0) {
	LOGI("VideoDutePlayerController instance created");
	pthread_mutex_init(&mLock, NULL);
	pthread_cond_init(&mCondition, NULL);
	renderer = new PicPreviewRender();
	this->screenWidth = 720;
	this->screenHeight = 720;
}

PicPreviewController::~PicPreviewController() {
	LOGI("VideoDutePlayerController instance destroyed");
	pthread_mutex_destroy(&mLock);
	pthread_cond_destroy(&mCondition);
}

bool PicPreviewController::start(char* spriteFilePath) {
	LOGI("Creating VideoDutePlayerController thread");
	pthread_create(&_threadId, 0, threadStartCallback, this);
	return true;
}

void PicPreviewController::stop() {
	LOGI("Stopping VideoDutePlayerController Render thread");
	/*send message to render thread to stop rendering*/
	pthread_mutex_lock(&mLock);
	_msg = MSG_RENDER_LOOP_EXIT;
	pthread_cond_signal(&mCondition);
	pthread_mutex_unlock(&mLock);

	LOGI("we will join render thread stop");
	pthread_join(_threadId, 0);
	LOGI("VideoDutePlayerController Render thread stopped");
}

void PicPreviewController::setWindow(ANativeWindow *window) {
	/*notify render thread that window has changed*/
	pthread_mutex_lock(&mLock);
	_msg = MSG_WINDOW_SET;
	_window = window;
	pthread_cond_signal(&mCondition);
	pthread_mutex_unlock(&mLock);
}

void PicPreviewController::resetSize(int width, int height) {
	LOGI("VideoDutePlayerController::resetSize width:%d; height:%d", width, height);
	pthread_mutex_lock(&mLock);
	this->screenWidth = width;
	this->screenHeight = height;
	renderer->resetRenderSize(0, 0, width, height);
	pthread_cond_signal(&mCondition);
	pthread_mutex_unlock(&mLock);
}

void* PicPreviewController::threadStartCallback(void *myself) {
	PicPreviewController *controller = (PicPreviewController*) myself;
	controller->renderLoop();
	pthread_exit(0);
	return 0;
}

void PicPreviewController::renderLoop() {
	bool renderingEnabled = true;
	LOGI("renderLoop()");
	while (renderingEnabled) {
		pthread_mutex_lock(&mLock);
		/*process incoming messages*/
        LOGI("looping msg: %d", _msg);
		switch (_msg) {
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
		_msg = MSG_NONE;

		if (eglCore) {
			eglCore->makeCurrent(previewSurface);
			this->drawFrame();
			pthread_cond_wait(&mCondition, &mLock);
			usleep(100 * 1000);
		}

		pthread_mutex_unlock(&mLock);
	}
	LOGI("Render loop exits");

	return;
}

bool PicPreviewController::initialize() {
	eglCore = new EGLCore();
	eglCore->init();
	previewSurface = eglCore->createWindowSurface(_window);
	eglCore->makeCurrent(previewSurface);

	picPreviewTexture = new PicPreviewTexture();
	bool createTexFlag = picPreviewTexture->createTexture();
	if(!createTexFlag){
		LOGE("createTexFlag is failed...");
		destroy();
		return false;
	}

	this->updateTexImage();

	bool isRendererInitialized = renderer->init(screenWidth, screenHeight, picPreviewTexture);
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
    picPreviewTexture->updateTexImage((byte*) data.data(), width, height);
}

void PicPreviewController::drawFrame() {
    renderer->render();
	if (!eglCore->swapBuffers(previewSurface)) {
		LOGE("eglSwapBuffers() returned error %d", eglGetError());
	}
}
