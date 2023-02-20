package com.qomo.miscopengles.activity;

import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.GLES20;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;

import com.qomo.miscopengles.egl.EglBase;
import com.qomo.miscopengles.view.QomoSurfaceView;

public class QomoSurfaceViewActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static final String TAG = "QomoSurfaceViewActivity";
    private EglBase eglBase;
    private Handler renderThreadHandler;
    private QomoSurfaceView qomoSurfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        qomoSurfaceView = new QomoSurfaceView(this);
        qomoSurfaceView.getHolder().addCallback(this);
        setContentView(qomoSurfaceView);
        final HandlerThread renderThread = new HandlerThread("EglRenderer");
        renderThread.start();
        renderThreadHandler = new Handler(renderThread.getLooper());
        postOnRenderThread(() -> eglBase = EglBase.create());
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
        postOnRenderThread(() -> {
            eglBase.createSurface(holder.getSurface());
            eglBase.makeCurrent();
            GLES20.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
            EGLDisplay eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        });
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG, "surfaceChanged");
        postOnRenderThread(() -> GLES20.glViewport(0, 0, width, height));
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    private void postOnRenderThread(Runnable runnable) {
        renderThreadHandler.post(runnable);
    }
}
