package com.qomo.miscopengles.activity;

import android.opengl.GLES31;
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
        view.setEGLContextClientVersion(3);

        view.setRenderer(new EGL14SurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated() {
                GLES31.glClearColor(1.0f, 0.5f, 0.25f, 1.0f);
                GLES31.glDisable(GLES31.GL_DEPTH_TEST);
                GLES31.glDisable(GLES31.GL_CULL_FACE);
            }

            @Override
            public void onSurfaceChanged(int width, int height) {
                GLES31.glViewport(0, 0, width, height);
            }

            @Override
            public void onDrawFrame() {
                GLES31.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
                GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT);
            }
        });
    }
}
