package com.qomo.miscopengles.view;

import android.view.Surface;

public class EGLWindowSurface extends EGLSurfaceBase {
    private Surface surface;
    public EGLWindowSurface(EGL14SurfaceView.RenderContext egl, Surface surface) {
        super(egl, surface);
        this.surface = surface;
    }

    @Override
    void release() {
        super.release();
        surface.release();
    }
}
