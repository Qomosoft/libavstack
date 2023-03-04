package com.qomo.miscopengles.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.qomo.miscopengles.R;
import com.qomo.miscopengles.view.QomoSurfaceView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initViews();
    }

    private void initViews() {
        findViewById(R.id.ll_glsurfaceview_example).setOnClickListener(this);
        findViewById(R.id.ll_surfaceview_example).setOnClickListener(this);
        findViewById(R.id.ll_my_glsurfaceview_example).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        Intent intent = new Intent();
        switch (v.getId()) {
            case R.id.ll_glsurfaceview_example:
                intent.setClass(this, QomoGLSurfaceViewActivity.class);
                break;
            case R.id.ll_surfaceview_example:
                intent.setClass(this, QomoSurfaceViewActivity.class);
                break;
            case R.id.ll_my_glsurfaceview_example:
                intent.setClass(this, MyGLSurfaceViewActivity.class);
                break;
            default:
                break;
        }
        startActivity(intent);
    }
}