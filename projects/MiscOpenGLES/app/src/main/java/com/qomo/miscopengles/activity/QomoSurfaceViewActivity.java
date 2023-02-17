package com.qomo.miscopengles.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.qomo.miscopengles.view.QomoSurfaceView;

public class QomoSurfaceViewActivity extends AppCompatActivity {
    private QomoSurfaceView qomoSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        qomoSurfaceView = new QomoSurfaceView(this);
        setContentView(qomoSurfaceView);
    }
}