package com.qomo.miscopengles.view;

import android.content.Context;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLES20;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QomoSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private static final String TAG = "QomoSurfaceView";

    public QomoSurfaceView(Context context) {
        super(context);
        getHolder().addCallback(this);
    }

    private boolean initWindow(SurfaceView glSurfaceView) {
        // 1. 获取 EAL 显示
        EGLDisplay eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if(eglDisplay == EGL14.EGL_NO_DISPLAY) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        // 2. 初始化 EGL
        int[] version = new int[2];
        if(!EGL14.eglInitialize(eglDisplay, version, 0, version, 1)) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        // 3. 确定配置
        int[] configAttribs = new int[] {
            EGL14.EGL_RENDERABLE_TYPE, EGL14.EGL_WINDOW_BIT,
            EGL14.EGL_RED_SIZE, 8,
            EGL14.EGL_GREEN_SIZE, 8,
            EGL14.EGL_BLUE_SIZE, 8,
            EGL14.EGL_DEPTH_SIZE, 24,
            EGL14.EGL_NONE
        };
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        if(!EGL14.eglChooseConfig(eglDisplay, configAttribs, 0, configs, 0,
            configs.length, numConfigs, 0)) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        if(configs[0] == null) {
            return false;
        }
        // 4. 创建渲染表面，此处是创建窗口
        EGLSurface window = EGL14.eglCreateWindowSurface(eglDisplay,
            configs[0], glSurfaceView.getHolder(), null, 0);
        if(window == EGL14.EGL_NO_SURFACE) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        // 5. 创建上下文
        int[] contextAttribs = new int[] {
            EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL14.EGL_NONE
        };
        EGLContext eglContext = EGL14.eglCreateContext(eglDisplay, configs[0],
            EGL14.EGL_NO_CONTEXT, contextAttribs, 0);
        if(eglContext == EGL14.EGL_NO_CONTEXT) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        // 6. 绑定上下文与表面
        if(!EGL14.eglMakeCurrent(eglDisplay, window, window, eglContext)) {
            Log.e("initWindow", EGL14.eglGetError() + "");
            return false;
        }
        Log.d("initWindow", "初始化成功");
        return true;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        if (!initWindow(this)) {
            Log.e(TAG, "initWindow error");
            return;
        }
        GLES20.glClearColor(1.0f, 0.5f, 0.25f, 1.0f);
        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_CULL_FACE);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        GLES20.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }
}
