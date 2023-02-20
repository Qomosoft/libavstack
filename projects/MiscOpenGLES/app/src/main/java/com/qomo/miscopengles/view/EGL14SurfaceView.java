package com.qomo.miscopengles.view;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Choreographer;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class EGL14SurfaceView extends SurfaceView implements SurfaceHolder.Callback2 {
    private static final String TAG = "EGL14SurfaceView";

    public EGL14SurfaceView(Context context) {
        super(context);
    }

    interface EGLConfigChooser {
        EGLConfig chooseConfig(EGLDisplay eglDisplay);
    }

    interface EGLContextFactory {
        EGLContext createContext(EGLDisplay eglDisplay, EGLConfig eglConfig);
        void destroyContext(EGLDisplay eglDisplay, EGLContext eglContext);
    }

    interface EGLWindowSurfaceFactory {
        EGLSurface createWindowSurface(EGLDisplay eglDisplay, EGLConfig eglConfig, Object nativeWindow);
        void destroySurface(EGLDisplay eglDisplay, EGLSurface eglSurface);
    }

    interface Renderer {
        void onSurfaceCreated();
        void onSurfaceChanged(int width, int height);
        void onDrawFrame();
    }

    class RenderContext {
        private EGLDisplay display = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        private EGLConfig config;
        private EGLContext context;
        private EGLContextFactory contextFactory;
        private EGLWindowSurfaceFactory windowSurfaceFactory;
        public RenderContext(EGLConfigChooser configChooser, EGLContextFactory contextFactory,
                      EGLWindowSurfaceFactory windowSurfaceFactory) {
            int[] version = new int[2];
            EGL14.eglInitialize(display, version, 0, version, 1);
            contextFactory = contextFactory;
            windowSurfaceFactory = windowSurfaceFactory;
            config = configChooser.chooseConfig(display);
            context = contextFactory.createContext(display, config);
        }

        public void release() {
            EGL14.eglMakeCurrent(display, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
            contextFactory.destroyContext(display, context);
            EGL14.eglReleaseThread();
            EGL14.eglTerminate(display);
        }

        public EGLSurface createWindowSurface(Object surface) {
            if (!(surface instanceof Surface) && !(surface instanceof SurfaceTexture)) {
                throw new RuntimeException("Invalid surface: " + surface);
            }

            EGLSurface eglSurface = windowSurfaceFactory.createWindowSurface(display, config, surface);
            checkGLError("eglCreateWindowSurface");
            return eglSurface;
        }

        public void releaseSurface(EGLSurface eglSurface) {
            windowSurfaceFactory.destroySurface(display, eglSurface);
        }

        public void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
            if (display != EGL14.EGL_NO_DISPLAY) {
                EGL14.eglMakeCurrent(display, drawSurface, readSurface, context);
            } else {
                Log.e(TAG, "makeCurrent error: EGL_NO_DISPLAY");
            }
        }

        public void makeCurrent(EGLSurface surface) {
            makeCurrent(surface, surface);
        }

        public void makeUnCurrent() {
            makeCurrent(EGL14.EGL_NO_SURFACE);
        }

        public boolean swapBuffers(EGLSurface eglSurface) {
            return EGL14.eglSwapBuffers(display, eglSurface);
        }
    }

    private RenderThread renderThread = new RenderThread(getHolder());

    private Choreographer.FrameCallback choreographerCallback = new Choreographer.FrameCallback() {
        @Override
        public void doFrame(long frameTimeNanos) {
            if (null != renderThread.handler) {
                requestNextFrame();
                renderThread.handler.sendSurfaceRedraw(frameTimeNanos);
            }
        }

        void requestNextFrame() {
            Choreographer.getInstance().postFrameCallback(this);
        }

        void stop() {
            Choreographer.getInstance().removeFrameCallback(this);
        }
    };

    @Override
    public void surfaceRedrawNeeded(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    void checkGLError(String msg) {
        int err = EGL14.eglGetError();
        if (err != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(err));
        }
    }

    class RenderThread extends Thread {
        private static final String TAG = "RenderThread";
        public RenderThread(SurfaceHolder holder) {

        }

        class RenderHandler extends Handler {
            enum MSG {
                SURFACE_CREATED,
                SURFACE_CHANGED,
                SURFACE_REDRAW,
                SHUTDOWN
            }
        }
    }
}
