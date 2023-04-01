#ifndef ANDROID_EGL_CORE_H_
#define ANDROID_EGL_CORE_H_

#include "logging.h"

//#include "./../libcommon/CommonTools.h"
#include <pthread.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <KHR/khrplatform.h>

typedef EGLBoolean (EGLAPIENTRYP PFNEGLPRESENTATIONTIMEANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

class EGLCore {
public:
	EGLCore();
    virtual ~EGLCore();

    bool init();

    bool init(EGLContext sharedContext);

  	EGLSurface createWindowSurface(ANativeWindow* _window);

	bool makeCurrent(EGLSurface eglSurface);

	bool swapBuffers(EGLSurface eglSurface);

	void releaseSurface(EGLSurface eglSurface);
    void release();

private:
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;

	PFNEGLPRESENTATIONTIMEANDROIDPROC pfneglPresentationTimeANDROID;
};
#endif // ANDROID_EGL_CORE_H_
