package com.qomo.qomoplayer.media;

import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.qomo.qomoplayer.Constants;

public class QomoPlayer {
    public static final String TAG = "QomoPlayer";
    private long mNativeHandle = 0;
    public QomoPlayer() {
        mNativeHandle = native_init(Constants.CodecType.SWDecoder);
        if (mNativeHandle == 0) {
            Log.e(TAG, "failed to create native player engine");
        }
    }
    public void setDisplay(SurfaceHolder sh) {
        native_setDisplay(mNativeHandle, sh);
    }

    public void setSurface(Surface surface) {
        native_setSurface(mNativeHandle, surface);
    }

    public void setWindowSize(int width, int height) {
        native_setWindowSize(mNativeHandle, width, height);
    }

    public void setVideoScalingMode(int mode) {
        native_setVideoScalingMode(mNativeHandle, mode);
    }

    public void setDataSource(String path) {
        native_setDataSource(mNativeHandle, path);
    }

    public void prepare() {
        native_prepare(mNativeHandle);
    }

    public void prepareAsync() {
        native_prepareAsync(mNativeHandle);
    }

    public void start() {
        native_start(mNativeHandle);
    }

    public void stop() {
        native_stop(mNativeHandle);
    }

    public void pause() {
        native_pause(mNativeHandle);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        native_finalize(mNativeHandle);
        mNativeHandle = 0;
    }

    private static native long native_init(int decoderType);
    private native void native_setup();
    private native void native_finalize(long nativeHandle);

    private native void native_setVideoScalingMode(long nativeHandle, int mode);
    private native void native_setDisplay(long nativeHandle, SurfaceHolder sh);
    private native void native_setSurface(long nativeHandle, Surface surface);

    private native void native_setWindowSize(long nativeHandle, int width, int height);
    private native void native_setDataSource(long nativeHandle, String path);
    private native void native_prepare(long nativeHandle);
    private native void native_prepareAsync(long nativeHandle);

    private native void native_start(long nativeHandle);
    private native void native_pause(long nativeHandle);
    private native void native_stop(long nativeHandle);
}
