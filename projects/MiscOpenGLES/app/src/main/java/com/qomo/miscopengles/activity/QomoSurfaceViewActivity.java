package com.qomo.miscopengles.activity;

import android.opengl.GLES20;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;

import com.qomo.miscopengles.view.EGL14SurfaceView;

public class QomoSurfaceViewActivity extends AppCompatActivity {
    private static final String TAG = "QomoSurfaceViewActivity";
    private EGL14SurfaceView qomoSurfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        qomoSurfaceView = new EGL14SurfaceView(this);
        setContentView(qomoSurfaceView);
        setupGLSurface(qomoSurfaceView);
    }

    private void setupGLSurface(EGL14SurfaceView view) {
        view.setEGLConfigChooser(8, 8, 8, 8, 24, 8);
        view.setEGLContextClientVersion(2);

        view.setRenderer(new EGL14SurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated() {
                GLES20.glClearColor(1.0f, 0.5f, 0.25f, 1.0f);
                GLES20.glDisable(GLES20.GL_DEPTH_TEST);
                GLES20.glDisable(GLES20.GL_CULL_FACE);
            }

            @Override
            public void onSurfaceChanged(int width, int height) {
                GLES20.glViewport(0, 0, width, height);
            }

            @Override
            public void onDrawFrame() {
                GLES20.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
            }
        });
    }
}
