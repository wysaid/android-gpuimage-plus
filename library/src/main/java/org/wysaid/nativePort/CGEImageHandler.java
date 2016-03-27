package org.wysaid.nativePort;

import android.graphics.Bitmap;

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

    protected long mNativeAddress;

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

    //直接传 null 可以清空当前使用的滤镜。
    public void setFilterWithConfig(String config) {
        nativeSetFilterWithConfig(mNativeAddress, config, true, true);
    }

    public void setFilterWithConfig(String config, boolean shouldClearOlder, boolean shouldProcess) {
        nativeSetFilterWithConfig(mNativeAddress, config, shouldClearOlder, shouldProcess);
    }

    public void setFilterIntensity(float intensity) {
        nativeSetFilterIntensity(mNativeAddress, intensity, true);
    }

    public void setFilterIntensity(float intensity, boolean shouldProcess) {
        nativeSetFilterIntensity(mNativeAddress, intensity, shouldProcess);
    }

    public void drawResult() {
        nativeDrawResult(mNativeAddress);
    }

    //绑定handler输出的FBO
    public void bindTargetFBO() {
        nativeBindTargetFBO(mNativeAddress);
    }

    //绑定handler输出的FBO 并且设置viewport为FBO大小
    public void setAsTarget() {
        nativeSetAsTarget(mNativeAddress);
    }

    //交换缓存
    public void swapBufferFBO() {
        nativeSwapBufferFBO(mNativeAddress);
    }

    //恢复图像
    public void revertImage() {
        nativeRevertImage(mNativeAddress);
    }

    public void processingFilters() {
        nativeProcessingFilters(mNativeAddress);
    }

    public void processWithFilter(long filterAddress) {
        nativeProcessWithFilter(mNativeAddress, filterAddress);
    }

    public void release() {
        if(mNativeAddress != 0) {
            nativeRelease(mNativeAddress);
            mNativeAddress = 0;
        }
    }


    /////////////////      protected         ///////////////////////

    protected native long nativeCreateHandler();
    protected native boolean nativeInitWithBitmap(long holder, Bitmap bmp);
    protected native Bitmap nativeGetResultBitmap(long holder);

    protected native void nativeSetDrawerRotation(long holder, float rad);
    protected native void nativeSetDrawerFlipScale(long holder, float x, float y);
    protected native boolean nativeSetFilterWithConfig(long holder, String config, boolean shouldCleanOlder, boolean shouldProcess);
    protected native void nativeSetFilterIntensity(long holder, float value, boolean shouldProcess);

    protected native void nativeDrawResult(long holder);
    protected native void nativeBindTargetFBO(long holder);
    protected native void nativeSetAsTarget(long holder);
    protected native void nativeSwapBufferFBO(long holder);
    protected native void nativeRevertImage(long holder);
    protected native void nativeProcessingFilters(long holder);
    protected native void nativeProcessWithFilter(long holder, long filterAddress);

    protected native void nativeRelease(long holder);
}
