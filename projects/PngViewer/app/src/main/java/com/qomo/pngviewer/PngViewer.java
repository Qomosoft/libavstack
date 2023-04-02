package com.qomo.pngviewer;

import android.view.Surface;

public class PngViewer {

    public native void init(String picPath);

    public native void setSurface(Surface surface);

    public native void onWindowSizeChanged(int width, int height);

    public native void stop();

}
