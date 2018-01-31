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

    public boolean initWithBitmap(Bitmap bmp) {
        if(bmp == null)
            return false;

        if(bmp.getConfig() != Bitmap.Config.ARGB_8888) {
            bmp = bmp.copy(Bitmap.Config.ARGB_8888, false);
        }

        return nativeInitWithBitmap(mNativeAddress, bmp);
    }

    public boolean initWithSize(int width, int height) {
        return nativeInitWithSize(mNativeAddress, width, height);
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
        nativeSetFilterWithConfig(mNativeAddress, config, true, true);
    }

    //config: The filter rule string. Pass null for config to clear all filters.
    //shouldClearOlder: The last filter will be cleared if it's true.
    //    There may be memory leaks if you pass false for 'shouldClearOlder' and you have not cleared it by yourself.
    //shouldProcess: This marks if the filter should be run right now.
    //    The result will not change if you pass false for 'shouldProcess' until you call 'processFilters'.
    public void setFilterWithConfig(String config, boolean shouldClearOlder, boolean shouldProcess) {
        nativeSetFilterWithConfig(mNativeAddress, config, shouldClearOlder, shouldProcess);
    }

    public void setFilterIntensity(float intensity) {
        nativeSetFilterIntensity(mNativeAddress, intensity, true);
    }

    //intensity: filter intensity.
    //shouldProcess: This marks if the filter should be run right now.
    //    The result will not change if you pass false for 'shouldProcess' until you call 'processFilters'.
    public void setFilterIntensity(float intensity, boolean shouldProcess) {
        nativeSetFilterIntensity(mNativeAddress, intensity, shouldProcess);
    }

    //intensity: filter intensity.
    //index: Only the intensity of the filter on the given index will be changed.
    //shouldProcess: This marks if the filter should be run right now.
    //    The result will not change if you pass false for 'shouldProcess' until you call 'processFilters'.
    //return value: marks if this function worked. (It will fail when the index is out of range.)
    //    e.g. If you're running "@adjust contrast 0.5 @adjust brightness 1",
    //       Pass 0 for index to set the intensity of "@adjust contrast 0.5", the return value is true.
    //       Pass 1 for index to set the intensity of "@adjust brightness 1", the return value is true.
    //       Otherwise the return value is false.
    public boolean setFilterIntensityAtIndex(float intensity, int index, boolean shouldProcess) {
        return nativeSetFilterIntensityAtIndex(mNativeAddress, intensity, index, shouldProcess);
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

    public void processFilters() {
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

    public void setFilterWithAddres(long filter) {
        nativeSetFilterWithAddress(mNativeAddress, filter);
    }

    /////////////////      protected         ///////////////////////

    protected native long nativeCreateHandler();
    protected native boolean nativeInitWithBitmap(long holder, Bitmap bmp);
    protected native boolean nativeInitWithSize(long holder, int width, int height);
    protected native Bitmap nativeGetResultBitmap(long holder);

    protected native void nativeSetDrawerRotation(long holder, float rad);
    protected native void nativeSetDrawerFlipScale(long holder, float x, float y);
    protected native boolean nativeSetFilterWithConfig(long holder, String config, boolean shouldCleanOlder, boolean shouldProcess);
    protected native void nativeSetFilterWithAddress(long holder, long filter);
    protected native void nativeSetFilterIntensity(long holder, float value, boolean shouldProcess);
    protected native boolean nativeSetFilterIntensityAtIndex(long holder, float value, int index, boolean shouldProcess);

    protected native void nativeDrawResult(long holder);
    protected native void nativeBindTargetFBO(long holder);
    protected native void nativeSetAsTarget(long holder);
    protected native void nativeSwapBufferFBO(long holder);
    protected native void nativeRevertImage(long holder);
    protected native void nativeProcessingFilters(long holder);
    protected native void nativeProcessWithFilter(long holder, long filterAddress);

    protected native void nativeRelease(long holder);
}
