package com.qomo.miscopengles.view;

import android.content.Context;
import android.opengl.EGL14;
import android.opengl.EGLExt;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends SurfaceView implements SurfaceHolder.Callback2 {
    private static final String TAG = "MyGLSurfaceView";
    private Renderer mRenderer;
    private GLThread mGLThread;
    private int mEGLContextClientVersion;

    public MyGLSurfaceView(Context context) {
        super(context);
        init();
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public void setRenderer(Renderer renderer) {
        mRenderer = renderer;
        mGLThread = new GLThread();
        mGLThread.setRenderer(mRenderer);
    }

    public void setEGLContextClientVersion(int version) {
        checkRenderThreadState();
        mEGLContextClientVersion = version;
    }

    private void init() {
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
    }

    private void checkRenderThreadState() {
        if (mGLThread != null) {
            throw new IllegalStateException(
                "setRenderer has already been called for this instance.");
        }
    }

    @Override
    public void surfaceRedrawNeeded(@NonNull SurfaceHolder holder) {
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        mGLThread.surfaceCreated(getHolder());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        mGLThread.surfaceChanged(getHolder(), format, width, height);
        mGLThread.start();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mGLThread.surfaceDestroyed(holder);
    }

    public interface Renderer {
        void onSurfaceCreated(GL10 gl, EGLConfig config);
        void onSurfaceChanged(GL10 gl, int width, int height);
        void onDrawFrame(GL10 gl);
    }

    static class GLThread extends Thread implements SurfaceHolder.Callback {
        private boolean mExited = false;
        private boolean mEglPrepared = false;

        private EGL10 mEgl;
        private GL10 gl;
        private EGLDisplay mEglDisplay;
        private EGLSurface mEglSurface;
        private EGLConfig mEglConfig;
        private EGLContext mEglContext;
        private SurfaceHolder mSurfaceHolder;
        private Renderer mRenderer;
        private int mWidth;
        private int mHeight;
        private int[] mConfigSpec;
        private int mEGLContextClientVersion = 2;
        private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        @Override
        public void run() {
            setName("GLThread " + getId());
            while (!mExited) {
                if (!mEglPrepared) {
                    prepareEglContext();
                    if (mRenderer != null) {
                        mRenderer.onSurfaceCreated(gl, mEglConfig);
                        mRenderer.onSurfaceChanged(gl, mWidth, mHeight);
                        mRenderer.onDrawFrame(gl);
                        if (!mEgl.eglSwapBuffers(mEglDisplay, mEglSurface)) {
                            throw new RuntimeException("eglSwapBuffers failed: 0x" + Integer.toHexString(mEgl.eglGetError()));
                        }
                    }
                    mEglPrepared = true;
                }
            }

            //Destroy context
            if (mEgl.eglDestroySurface(mEglDisplay, mEglSurface)) {
                Log.e(TAG, "eglDestroySurface display:" + mEglDisplay + " surface: " + mEglSurface);
            }

            if (mEgl.eglDestroyContext(mEglDisplay, mEglContext)) {
                Log.e(TAG, "eglDestroyContext display:" + mEglDisplay + " context: " + mEglContext);
            }
        }

        private void prepareEglContext() {
            mEgl = (EGL10) EGLContext.getEGL();

            mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
            if (mEglDisplay == EGL10.EGL_NO_DISPLAY) {
                throw new RuntimeException("eglGetDisplay failed");
            }

            int[] version = new int[2];
            if (!mEgl.eglInitialize(mEglDisplay, version)) {
                throw new RuntimeException("eglInitialize failed");
            }

            int[] configSpec = new int[] {
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_ALPHA_SIZE, 0,
                EGL10.EGL_DEPTH_SIZE, 16,
                EGL10.EGL_STENCIL_SIZE, 0,
                EGL10.EGL_NONE};
            mConfigSpec = filterConfigSpec(configSpec);

            int[] num_config = new int[1];
            if (!mEgl.eglChooseConfig(mEglDisplay, mConfigSpec, null, 0,
                num_config)) {
                throw new IllegalArgumentException("eglChooseConfig failed");
            }

            int numConfigs = num_config[0];
            EGLConfig[] configs = new EGLConfig[numConfigs];
            if (!mEgl.eglChooseConfig(mEglDisplay, mConfigSpec, configs, numConfigs,
                num_config)) {
                throw new IllegalArgumentException("eglChooseConfig#2 failed");
            }

            mEglConfig = configs[0];
            if (mEglConfig == null) {
                throw new IllegalArgumentException("eglChooseConfig#3 failed");
            }

            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, mEGLContextClientVersion,
                EGL10.EGL_NONE };

            mEglContext = mEgl.eglCreateContext(mEglDisplay, mEglConfig, EGL10.EGL_NO_CONTEXT,
                mEGLContextClientVersion != 0 ? attrib_list : null);

            if (mEglContext == null || mEglContext == EGL10.EGL_NO_CONTEXT) {
                mEglContext = null;
                throw new RuntimeException("createEglContext");
            }

            mEglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, mEglConfig, mSurfaceHolder, null);
            if (mEglSurface == null) {
                throw new RuntimeException("createWindowSurface");
            }

            if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
                /*
                 * Could not make the context current, probably because the underlying
                 * SurfaceView surface has been destroyed.
                 */
                Log.e(TAG, "eglMakeCurrent failed: 0x" + Integer.toHexString(mEgl.eglGetError()));
            }

            gl = (GL10) mEglContext.getGL();
        }

        private int[] filterConfigSpec(int[] configSpec) {
            if (mEGLContextClientVersion != 2 && mEGLContextClientVersion != 3) {
                return configSpec;
            }
            /* We know none of the subclasses define EGL_RENDERABLE_TYPE.
             * And we know the configSpec is well formed.
             */
            int len = configSpec.length;
            int[] newConfigSpec = new int[len + 2];
            System.arraycopy(configSpec, 0, newConfigSpec, 0, len-1);
            newConfigSpec[len-1] = EGL10.EGL_RENDERABLE_TYPE;
            if (mEGLContextClientVersion == 2) {
                newConfigSpec[len] = EGL14.EGL_OPENGL_ES2_BIT;  /* EGL_OPENGL_ES2_BIT */
            } else {
                newConfigSpec[len] = EGLExt.EGL_OPENGL_ES3_BIT_KHR; /* EGL_OPENGL_ES3_BIT_KHR */
            }
            newConfigSpec[len+1] = EGL10.EGL_NONE;
            return newConfigSpec;
        }

        public void setRenderer(Renderer renderer) {
            mRenderer = renderer;
        }

        @Override
        public void surfaceCreated(@NonNull SurfaceHolder holder) {
            mSurfaceHolder = holder;
        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            mWidth = width;
            mHeight = height;
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
            mExited = true;
        }
    }
}
