#ifndef PIC_PREVIEW_CONTROLLER_H
#define PIC_PREVIEW_CONTROLLER_H

#include "pic_preview_render.h"
#include "qomo_egl.h"

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

class PicPreviewController {
public:
	PicPreviewController();
	virtual ~PicPreviewController();

	bool start(char* spriteFilePath);
	void stop();
	void setWindow(ANativeWindow* window);
	void resetSize(int width, int height);
private:
	PicPreviewRender* renderer;

	int screenWidth;
	int screenHeight;
	enum RenderThreadMessage {
		MSG_NONE = 0, MSG_WINDOW_SET, MSG_RENDER_LOOP_EXIT
	};

    std::unique_ptr<std::thread> render_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<int> msg_queue_;

	// android window, supported by NDK r5 and newer
	ANativeWindow* _window;

	std::unique_ptr<QomoEgl> egl_;
	EGLSurface previewSurface;
    GLint texture;

    GLuint createTexture();
	bool checkGlError(const char *op);

	// Helper method for starting the thread
	// RenderLoop is called in a rendering thread started in start() method
	// It creates rendering context and renders scene until stop() is called
	void renderLoop();
	bool initialize();
	void updateTexImage();
	void drawFrame();
	void destroy();
};

#endif // PIC_PREVIEW_CONTROLLER_H
