package com.qomo.miscopengles.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.qomo.miscopengles.view.QomoGLSurfaceView;

public class QomoGLSurfaceViewActivity extends AppCompatActivity {
    private QomoGLSurfaceView qomoGLSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        qomoGLSurfaceView = new QomoGLSurfaceView(this);
        setContentView(qomoGLSurfaceView);
    }
}