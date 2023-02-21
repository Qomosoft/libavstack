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
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Choreographer;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class EGL14SurfaceView extends SurfaceView implements SurfaceHolder.Callback2 {
    private static final String TAG = "EGL14SurfaceView";
    private SurfaceHolder holder;
    private EGLConfigChooser configChooser;
    private EGLContextFactory contextFactory;
    private EGLWindowSurfaceFactory windowSurfaceFactory;
    private Renderer renderer;

    public EGL14SurfaceView(Context context) {
        super(context);

        holder = getHolder();
        //Default window surface factory
        windowSurfaceFactory = new EGLWindowSurfaceFactory() {
            @Override
            public EGLSurface createWindowSurface(EGLDisplay eglDisplay, EGLConfig eglConfig, Object nativeWindow) {
                boolean notSurface = !(nativeWindow instanceof  Surface);
                boolean notSurfaceTexture = !(nativeWindow instanceof SurfaceTexture);
                if (notSurface && notSurfaceTexture) {
                    Log.e(TAG, "Invalid surface: " + nativeWindow);
                    return null;
                }
                int[] surfaceAttribs = new int[] {EGL14.EGL_NONE};
                return EGL14.eglCreateWindowSurface(eglDisplay, eglConfig, nativeWindow, surfaceAttribs, 0);
            }

            @Override
            public void destroySurface(EGLDisplay eglDisplay, EGLSurface eglSurface) {
                if (!EGL14.eglDestroySurface(eglDisplay, eglSurface)) {
                    Log.e(TAG, "eglDestroySurface failed");
                }
            }
        };
        renderThread.windowSurfaceFactory = windowSurfaceFactory;

        setWillNotDraw(false);
        holder.setFormat(PixelFormat.RGB_888);
        holder.addCallback(this);
    }

    public void setRenderer(Renderer renderer) {
        this.renderer = renderer;
        renderThread.renderer = renderer;
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

    public interface Renderer {
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
            this.contextFactory = contextFactory;
            this.windowSurfaceFactory = windowSurfaceFactory;
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

    class MyFrameCallback implements Choreographer.FrameCallback {
        @Override
        public void doFrame(long frameTimeNanos) {
            if (null != renderThread.handler) {
                requestNextFrame();
                renderThread.handler.sendSurfaceRedraw(frameTimeNanos);
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

        renderThread.start();
        renderThread.waitUntilReady();
        renderThread.handler.sendSurfaceCreated();
        requestRender();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, ">> surfaceChanged");

        renderThread.handler.sendSurfaceChanged(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.d(TAG, ">> surfaceDestroyed");

        renderThread.handler.sendShutdown();
        try {
            renderThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, "RenderThread join was interrupted: " + e.getMessage());
        }
        choreographerCallback.stop();
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

    /** Pause the rendering thread. */
    public void onPause() {
        choreographerCallback.stop();
    }

    /** Resume the rendering thread. */
    void onResume() {
        requestRender();
    }

    /** Specify a simple EGL14 Config chooser given its buffer channels bits size. */
    public void setEGLConfigChooser(int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize) {
        configChooser = new EGLConfigChooser() {
            @Override
            public EGLConfig chooseConfig(EGLDisplay eglDisplay) {
                int renderableType = EGL14.EGL_OPENGL_ES2_BIT | EGLExt.EGL_OPENGL_ES3_BIT_KHR;
                int[] configAttribs = new int[] {
                    EGL14.EGL_RENDERABLE_TYPE, renderableType,
                    EGL14.EGL_RED_SIZE, redSize,
                    EGL14.EGL_GREEN_SIZE, greenSize,
                    EGL14.EGL_BLUE_SIZE, blueSize,
                    EGL14.EGL_ALPHA_SIZE, alphaSize,
                    EGL14.EGL_DEPTH_SIZE, depthSize,
                    EGL14.EGL_STENCIL_SIZE, stencilSize,
                    EGL14.EGL_NONE
                };
                EGLConfig[] configs = new EGLConfig[] {null};
                int[] numConfigs = new int[] {1};

                EGL14.eglChooseConfig(eglDisplay, configAttribs, 0, configs, 0, configs.length, numConfigs, 0);
                return configs[0];
            }
        };
        renderThread.configChooser = configChooser;
    }

    /** Specify a simple EGL14 Context Factory given a glES version number. */
    public void setEGLContextClientVersion(int version) {
        contextFactory = new EGLContextFactory() {
            @Override
            public EGLContext createContext(EGLDisplay eglDisplay, EGLConfig eglConfig) {
                int[] contextAttribs = new int[] {
                    EGL14.EGL_CONTEXT_CLIENT_VERSION, version,
                    EGL14.EGL_NONE
                };
                EGLConfig config = configChooser.chooseConfig(eglDisplay);
                EGLContext ctx = EGL14.eglCreateContext(eglDisplay, config, EGL14.EGL_NO_CONTEXT, contextAttribs, 0);
                return ctx;
            }

            @Override
            public void destroyContext(EGLDisplay eglDisplay, EGLContext eglContext) {
                EGL14.eglDestroyContext(eglDisplay, eglContext);
            }
        };
        renderThread.contextFactory = contextFactory;
    }

    /** Post an event to the render thread. */
    public void queueEvent(Runnable r) {
        // [ don't even know if it works ]
        renderThread.handler.post(r);
    }

    // -------------------------------------------------------

    /** Thread managing surface rendering. */

    class RenderThread extends Thread {
        private static final String TAG = "RenderThread";
        public static final int SURFACE_CREATED = 1;
        public static final int SURFACE_CHANGED = 2;
        public static final int SURFACE_REDRAW = 3;
        public static final int SHUTDOWN = 4;

        class RenderHandler extends Handler {
            private RenderThread rt;
            RenderHandler(RenderThread rt) {
                this.rt = rt;
            }

            @Override
            public void handleMessage(@NonNull Message msg) {
                switch (msg.what) {
                    case SURFACE_CREATED:
                        rt.surfaceCreated();
                        break;
                    case SURFACE_CHANGED:
                        rt.surfaceChanged(msg.arg1, msg.arg2);
                        break;
                    case SURFACE_REDRAW:
                        rt.drawFrame();
                        break;
                    case SHUTDOWN:
                        rt.shutdown();
                        break;
                    default:
                        break;

                }
                super.handleMessage(msg);
            }

            void sendSurfaceCreated() {
                sendMessage(obtainMessage(SURFACE_CREATED));
            }

            void sendSurfaceChanged(int width, int height) {
                sendMessage(obtainMessage(SURFACE_CHANGED, width, height));
            }

            void sendSurfaceRedraw(long frameTimeNs) {
                sendMessage(obtainMessage(SURFACE_REDRAW, frameTimeNs));
            }

            void sendShutdown() {
                sendMessage(obtainMessage(SHUTDOWN));
            }
        }

        private RenderHandler handler = null;
        private EGLConfigChooser configChooser = null;
        private EGLContextFactory contextFactory = null;
        private EGLWindowSurfaceFactory windowSurfaceFactory = null;
        private Renderer renderer = null;
        private ReentrantLock lock = new ReentrantLock();
        private Condition condition = lock.newCondition();
        private boolean ready = false;
        private RenderContext egl = null;
        //private EGLSurfaceBase windowSurface = null;
        private EGLSurface windowSurface;
        private SurfaceHolder surfaceHolder;

        public RenderThread(SurfaceHolder holder) {
            this.surfaceHolder = holder;
        }

        @Override
        public void run() {
            super.run();
            Looper.prepare();
            setup();
            lock.lock();
            try {
                ready = true;
                condition.signal();
            } finally {
                lock.unlock();
            }

            Looper.loop();
            release();

            lock.lock();
            try {
                ready = false;
            } finally {
                lock.unlock();
            }
        }

        private void setup() {
            if (null == renderer) {
                Log.e(TAG, "Render was not set.");
                return;
            }

            // ? might be init outside the thread ?
            egl = new RenderContext(configChooser, contextFactory, windowSurfaceFactory);

            // *must* be initialized inside the thread.
            handler = new RenderHandler(this);
        }

        /** Thread data release. */
        private void release() {
            egl.release();
        }

        /** Wait until the render thread is ready to receive messages.
         *  Called from the UI thread. */
        void waitUntilReady() {
            lock.lock();
            try {
                while (!ready) {
                    condition.await();
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                lock.unlock();
            }
        }

        /* --- messages handling --- */

        private void surfaceCreated() {
            //windowSurface = new EGLWindowSurface(egl, surfaceHolder.getSurface());
            //windowSurface.makeCurrent();
            windowSurface = egl.createWindowSurface(surfaceHolder.getSurface());
            egl.makeUnCurrent();
            renderer.onSurfaceCreated();

            checkGLError("surfaceCreated");
        }

        private void surfaceChanged(int width, int height) {
            //windowSurface.makeCurrent();
            egl.makeCurrent(windowSurface);
            renderer.onSurfaceChanged(width, height);

            checkGLError("surfaceChanged");
        }

        private void drawFrame() {
            renderer.onDrawFrame();
            //windowSurface.makeCurrent();
            //windowSurface.swapBuffers();
            egl.makeCurrent(windowSurface);
            egl.swapBuffers(windowSurface);

            checkGLError("drawFrame");
        }

        private void shutdown() {
            Looper.myLooper().quit();
        }
    }
}
