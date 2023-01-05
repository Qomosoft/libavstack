package com.qomo.pngviewer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static final String TAG = "MainActivity";
    private SurfaceView pngSurfaceView;
    private PngViewer pngViewer;
    private String pngPath = "/sdcard/1.png";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initViews();
    }

    @Override
    protected void onStop() {
        super.onStop();
        pngViewer.stop();
        pngViewer = null;
    }

    private void initViews() {
        pngSurfaceView = findViewById(R.id.sv_png);
        SurfaceHolder holder = pngSurfaceView.getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        pngSurfaceView.getLayoutParams().width = getWindowManager().getDefaultDisplay().getWidth();
        pngSurfaceView.getLayoutParams().height = getWindowManager().getDefaultDisplay().getWidth();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        pngViewer = new PngViewer();
        pngViewer.init(pngPath);
        pngViewer.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        pngViewer.resetSize(width, height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    static {
        System.loadLibrary("pngviewer");
    }
}