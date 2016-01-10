package org.wysaid.nativePort;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;

/**
 * Created by wysaid on 15/12/25.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

//对 c++ native class 'CGEImageHandlerAndroid' 做映射

public class CGEImageHandler {

    static {
        NativeLibraryLoader.load();
    }

    protected ByteBuffer mNativeAddress;

    public CGEImageHandler() {
        mNativeAddress = nativeCreateHandler();
    }

    public boolean initWidthBitmap(Bitmap bmp) {
        if(bmp == null)
            return false;

        if(bmp.getConfig() != Bitmap.Config.ARGB_8888) {
            bmp = bmp.copy(Bitmap.Config.ARGB_8888, false);
        }

        return nativeInitWithBitmap(mNativeAddress, bmp);
    }

    public Bitmap getResultBitmap() {
        return nativeGetResultBitmap(mNativeAddress);
    }

    public void setDrawerRotation(float rad) {
        nativeSetDrawerRotation(mNativeAddress, rad);
    }

    public void setDrawerFlipScale(float x, float y) {
        nativeSetDrawerFlipScale(mNativeAddress, x, y);
    }

    public void setFilterWithConfig(String config) {
        nativeSetFilterWithConfig(mNativeAddress, config);
    }

    public void setFilterIntensity(float intensity) {
        nativeSetFilterIntensity(mNativeAddress, intensity);
    }

    public void drawResult() {
        nativeDrawResult(mNativeAddress);
    }

    public void bindTargetFBO() {
        nativeBindTargetFBO(mNativeAddress);
    }

    public void setAsTarget() {
        nativeSetAsTarget(mNativeAddress);
    }

    public void swapBufferFBO() {
        nativeSwapBufferFBO(mNativeAddress);
    }

    public void revertImage() {
        nativeRevertImage(mNativeAddress);
    }

    public void processingFilters() {
        nativeProcessingFilters(mNativeAddress);
    }

    public void release() {
        if(mNativeAddress != null) {
            nativeRelease(mNativeAddress);
            mNativeAddress = null;
        }
    }

    /////////////////      protected         ///////////////////////

    protected native ByteBuffer nativeCreateHandler();
    protected native boolean nativeInitWithBitmap(ByteBuffer holder, Bitmap bmp);
    protected native Bitmap nativeGetResultBitmap(ByteBuffer holder);

    protected native void nativeSetDrawerRotation(ByteBuffer holder, float rad);
    protected native void nativeSetDrawerFlipScale(ByteBuffer holder, float x, float y);
    protected native boolean nativeSetFilterWithConfig(ByteBuffer holder, String config);
    protected native void nativeSetFilterIntensity(ByteBuffer holder, float value);

    protected native void nativeDrawResult(ByteBuffer holder);
    protected native void nativeBindTargetFBO(ByteBuffer holder);
    protected native void nativeSetAsTarget(ByteBuffer holder);
    protected native void nativeSwapBufferFBO(ByteBuffer holder);
    protected native void nativeRevertImage(ByteBuffer holder);
    protected native void nativeProcessingFilters(ByteBuffer holder);

    protected native void nativeRelease(ByteBuffer holder);
}
