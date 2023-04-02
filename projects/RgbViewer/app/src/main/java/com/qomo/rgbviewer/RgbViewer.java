package com.qomo.rgbviewer;

import android.view.Surface;

public class RgbViewer {
  private static final String TAG = "RgbViewer";
  private long nativeHandler;

  public RgbViewer() {
    nativeHandler = nativeCreate();
  }

  public int Initialize(Surface surface) {
    return nativeInitialize(nativeHandler, surface);
  }


  public int onWindowSizeChanged(int width, int height) {
    return nativeOnWindowSizeChanged(nativeHandler, width, height);
  }

  @Override
  protected void finalize() throws Throwable {
    super.finalize();
    nativeDestroy(nativeHandler);
    nativeHandler = 0;
  }
  private native long nativeCreate();

  private native void nativeDestroy(long nativeHandler);

  private native int nativeInitialize(long nativeHandler, Surface surface);

  private native int nativeOnWindowSizeChanged(long nativeHandler, int width, int height);

  static {
    System.loadLibrary("rgbviewer");
  }
}
