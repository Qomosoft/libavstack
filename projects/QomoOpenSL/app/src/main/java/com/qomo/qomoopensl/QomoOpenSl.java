package com.qomo.qomoopensl;

public class QomoOpenSl {
    long nativeHandle;

    QomoOpenSl() {
        nativeHandle = nativeCreate();
    }

    public int init(String sourcePath, int sampleRate, int bufferSize, int channels) {
        return nativeInit(nativeHandle, sourcePath, sampleRate, bufferSize, channels);
    }

    public void start() {
        nativeStart(nativeHandle);
    }

    public void stop() {
        nativeStop(nativeHandle);
    }

    public void release() {
        if (nativeHandle != 0) {
            nativeRelease(nativeHandle);
            nativeHandle = 0;
        }
    }

    @Override
    protected void finalize() {
        release();
    }

    private native long nativeCreate();
    private native int nativeInit(long nativeHandle, String sourcePath, int sampleRate,
                                  int bufferSize, int channels);
    private native void nativeStart(long nativeHandle);
    private native void nativeStop(long nativeHandle);
    private native void nativeRelease(long nativeHandle);
}
