package com.qomo.rgbviewer;

import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
  private static final String TAG = "MainActivity";
  private SurfaceView surfaceView;
  private RgbViewer rgbViewer;
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    initView();
  }
  private void initView() {
    surfaceView = findViewById(R.id.surface_content);
    SurfaceHolder holder = surfaceView.getHolder();
    holder.addCallback(this);
  }
  @Override
  public void surfaceCreated(@NonNull SurfaceHolder holder) {
    rgbViewer = new RgbViewer();
    rgbViewer.Initialize(holder.getSurface(), getAssets());
  }
  @Override
  public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
    rgbViewer.onWindowSizeChanged(width, height);
  }
  @Override
  public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

  }
}