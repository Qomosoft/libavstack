package com.qomo.miscopengles.view;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Choreographer;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class EGL14SurfaceView extends SurfaceView implements SurfaceHolder.Callback2 {
    private static final String TAG = "EGL14SurfaceView";
    private Renderer renderer;
    private RenderContext renderContext;
    private EGLSurface eglSurface;
    private HandlerThread glThread;
    private Handler glHandler;

    public EGL14SurfaceView(Context context) {
        super(context);

//        setWillNotDraw(false);
//        holder.setFormat(PixelFormat.RGB_888);
        getHolder().addCallback(this);
        glThread = new HandlerThread("GLThread");
    }

    public void setRenderer(Renderer renderer) {
        this.renderer = renderer;
    }

    public interface Renderer {
        void onSurfaceCreated();
        void onSurfaceChanged(int width, int height);
        void onDrawFrame();
    }

    class RenderContext {
        private EGLDisplay eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        private EGLConfig eglConfig;
        private EGLContext eglContext;

        public RenderContext() {
            int[] version = new int[2];
            EGL14.eglInitialize(eglDisplay, version, 0, version, 1);
            int renderableType = EGL14.EGL_OPENGL_ES2_BIT | EGLExt.EGL_OPENGL_ES3_BIT_KHR;
            int[] configAttribs = new int[] {
                EGL14.EGL_RENDERABLE_TYPE, renderableType,
                EGL14.EGL_RED_SIZE, 8,
                EGL14.EGL_GREEN_SIZE, 8,
                EGL14.EGL_BLUE_SIZE, 8,
                EGL14.EGL_ALPHA_SIZE, 8,
                EGL14.EGL_DEPTH_SIZE, 24,
                EGL14.EGL_STENCIL_SIZE, 8,
                EGL14.EGL_NONE
            };
            EGLConfig[] configs = new EGLConfig[] {null};
            int[] numConfigs = new int[] {1};

            EGL14.eglChooseConfig(eglDisplay, configAttribs, 0, configs, 0, configs.length, numConfigs, 0);
            eglConfig = configs[0];

            int[] contextAttribs = new int[] {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL14.EGL_NONE
            };

            eglContext = EGL14.eglCreateContext(eglDisplay, eglConfig, EGL14.EGL_NO_CONTEXT, contextAttribs, 0);
        }

        public void release() {
            EGL14.eglMakeCurrent(eglDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
            EGL14.eglDestroyContext(eglDisplay, eglContext);
            EGL14.eglReleaseThread();
            EGL14.eglTerminate(eglDisplay);
        }

        public EGLSurface createWindowSurface(Object surface) {
            if (!(surface instanceof Surface) && !(surface instanceof SurfaceTexture)) {
                throw new RuntimeException("Invalid surface: " + surface);
            }

            if (!(surface instanceof Surface || surface instanceof SurfaceTexture)) {
                Log.e(TAG, "Invalid surface: " + surface);
                return null;
            }
            int[] surfaceAttribs = new int[] {EGL14.EGL_NONE};
            EGLSurface eglSurface = EGL14.eglCreateWindowSurface(eglDisplay, eglConfig, surface, surfaceAttribs, 0);
            checkGLError("eglCreateWindowSurface");
            return eglSurface;
        }

        public void releaseSurface(EGLSurface eglSurface) {
            if (!EGL14.eglDestroySurface(eglDisplay, eglSurface)) {
                Log.e(TAG, "eglDestroySurface failed");
            }
        }

        public void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
            if (eglDisplay != EGL14.EGL_NO_DISPLAY) {
                EGL14.eglMakeCurrent(eglDisplay, drawSurface, readSurface, eglContext);
            } else {
                Log.e(TAG, "makeCurrent error: EGL_NO_DISPLAY");
            }
        }

        public void makeCurrent(EGLSurface eglSurface) {
            makeCurrent(eglSurface, eglSurface);
        }

        public void makeUnCurrent() {
            makeCurrent(EGL14.EGL_NO_SURFACE);
        }

        public boolean swapBuffers(EGLSurface eglSurface) {
            return EGL14.eglSwapBuffers(eglDisplay, eglSurface);
        }
    }

    class MyFrameCallback implements Choreographer.FrameCallback {
        @Override
        public void doFrame(long frameTimeNanos) {
            if (null != glHandler) {
                runOnGLThread(() -> {
                    renderer.onDrawFrame();
                    renderContext.makeCurrent(eglSurface);
                    renderContext.swapBuffers(eglSurface);
                    checkGLError("drawFrame");
                });
            }
        }

        public void requestNextFrame() {
            Choreographer.getInstance().postFrameCallback(this);
        }

        public void stop() {
            Choreographer.getInstance().removeFrameCallback(this);
        }
    }

    private MyFrameCallback choreographerCallback = new MyFrameCallback();

    @Override
    public void surfaceRedrawNeeded(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        // Notes:
        // Some bugs might appears because of thread member initialization.
        // We might have to set some variables inside RenderThread::run directly.
        Log.d(TAG, ">> surfaceCreated");

        glThread.start();
        glHandler = new Handler(glThread.getLooper());
        runOnGLThread(() -> {
            renderContext = new RenderContext();
            eglSurface = renderContext.createWindowSurface(holder.getSurface());
            renderContext.makeUnCurrent();
            renderer.onSurfaceCreated();
            checkGLError("surfaceCreated");
        });
        requestRender();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, ">> surfaceChanged");

        runOnGLThread(() -> {
            renderContext.makeCurrent(eglSurface);
            renderer.onSurfaceChanged(width, height);
            checkGLError("surfaceChanged");
        });
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.d(TAG, ">> surfaceDestroyed");

        choreographerCallback.stop();
        glThread.quit();
        renderContext.release();
    }

    public void checkGLError(String msg) {
        int err = EGL14.eglGetError();
        if (err != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(err));
        }
    }

    /** Request that the renderer render a frame. */
    public void requestRender() {
        choreographerCallback.requestNextFrame();
    }

    private void runOnGLThread(Runnable runnable) {
        glHandler.post(runnable);
    }
}
