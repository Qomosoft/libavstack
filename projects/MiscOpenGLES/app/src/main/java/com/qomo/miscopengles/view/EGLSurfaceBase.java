package com.qomo.miscopengles.view;

import android.opengl.EGL14;
import android.opengl.EGLSurface;

public class EGLSurfaceBase {
    private EGLSurface eglSurface;
    private EGL14SurfaceView.RenderContext egl;

    public EGLSurfaceBase(EGL14SurfaceView.RenderContext egl, Object surface) {
        this.egl = egl;
        this.eglSurface = egl.createWindowSurface(surface);
    }

    void release() {
        egl.release();
        eglSurface = EGL14.EGL_NO_SURFACE;
    }

    void makeCurrent() {
        egl.makeCurrent(eglSurface);
    }

    boolean swapBuffers() {
        return egl.swapBuffers(eglSurface);
    }
}
