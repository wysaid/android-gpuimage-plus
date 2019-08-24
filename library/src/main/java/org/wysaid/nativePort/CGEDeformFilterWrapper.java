package org.wysaid.nativePort;

import android.util.Log;

import org.wysaid.common.Common;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Date: 2018/12/09
 * Description:
 */

public class CGEDeformFilterWrapper {

    static {
        NativeLibraryLoader.load();
    }

    private CGEDeformFilterWrapper(int w, int h, float stride) {
        mNativeAddress = nativeCreate(w, h, stride);
    }

    //'width', 'height': The real size of the picture in pixels.
    //'stride': Set the mesh in real pixels, the mesh size would be (width / stride, height / stride)
    public static CGEDeformFilterWrapper create(int w, int h, float stride) {
        CGEDeformFilterWrapper wrapper = new CGEDeformFilterWrapper(w, h, stride);
        if(wrapper.mNativeAddress == 0) {
            wrapper.release(true);
            wrapper = null;
            Log.e(Common.LOG_TAG, "CGEDeformFilterWrapper.create failed!");
        }
        return wrapper;
    }

    // You should not delete the native filter if the wrapper is binding to the image handler.
    // The handler will release the native filter when necessary.
    public void release(boolean shouldDeleteNativeFilter) {
        if(mNativeAddress != 0) {
            if(shouldDeleteNativeFilter) {
                nativeRelease(mNativeAddress);
            }
            mNativeAddress = 0;
        }
    }

    public void restore() {
        nativeRestore(mNativeAddress);
    }

    public void restoreWithIntensity(float intensity) {
        nativeRestoreWithIntensity(mNativeAddress, intensity);
    }

    //'start', 'end': the real position of the cursor between two operations.
    //'x/y': the real position of the cursor.
    //'w', 'h': the canvas size. (the max 'x' and 'y' of the cursor)
    //radius: the deform radius in real pixels.
    //intensity: range (0, 1], 0 for origin. Better not more than 0.5
    public void forwardDeform(float startX, float startY, float endX, float endY, float w, float h, float radius, float intensity) {
        nativeForwardDeform(mNativeAddress, startX, startY, endX, endY, w, h, radius, intensity);
    }

    public void restoreWithPoint(float x, float y, float w, float h, float radius, float intensity) {
        nativeRestoreWithPoint(mNativeAddress, x, y, w, h, radius, intensity);
    }

    public void bloatDeform(float x, float y, float w, float h, float radius, float intensity) {
        nativeBloatDeform(mNativeAddress, x, y, w, h, radius, intensity);
    }

    public void wrinkleDeform(float x, float y, float w, float h, float radius, float intensity) {
        nativeWrinkleDeform(mNativeAddress, x, y, w, h, radius, intensity);
    }

    public void setUndoSteps(int undoSteps) {
        nativeSetUndoSteps(mNativeAddress, undoSteps);
    }

    // check if `undo` is available.
    public boolean canUndo() {
        return nativeCanUndo(mNativeAddress);
    }

    // check if `redo` is available.
    public boolean canRedo() {
        return nativeCanRedo(mNativeAddress);
    }

    // The return value marks if `undo` is done correctly.
    public boolean undo() {
        return nativeUndo(mNativeAddress);
    }

    // The return value marks if `redo` is done correctly.
    public boolean redo() {
        return nativeRedo(mNativeAddress);
    }

    // The return value marks if `pushDeformStep` is done correctly.
    // `false` will be returned when the steps count meet the "max step" set by "setUndoSteps"
    public boolean pushDeformStep() {
        return nativePushDeformStep(mNativeAddress);
    }

    public void showMesh(boolean show) {
        nativeShowMesh(mNativeAddress, show);
    }

    protected long mNativeAddress;

    // You should not release the native filter if you pass the native address to the handler.
    public long getNativeAddress() {
        return mNativeAddress;
    }

    protected native long nativeCreate(int width, int height, float stride);
    protected native void nativeRelease(long holder);

    // reset to default.
    protected native void nativeRestore(long holder);
    protected native void nativeRestoreWithIntensity(long holder, float intensity);
    protected native void nativeForwardDeform(long holder, float startX, float startY, float endX, float endY, float w, float h, float radius, float intensity);
    protected native void nativeRestoreWithPoint(long holder, float x, float y, float w, float h, float radius, float intensity);
    protected native void nativeBloatDeform(long holder, float x, float y, float w, float h, float radius, float intensity);
    protected native void nativeWrinkleDeform(long holder, float x, float y, float w, float h, float radius, float intensity);

    protected native void nativeSetUndoSteps(long holder, int undoSteps);
    protected native boolean nativeCanUndo(long holder);
    protected native boolean nativeCanRedo(long holder);
    protected native boolean nativeUndo(long holder);
    protected native boolean nativeRedo(long holder);
    protected native boolean nativePushDeformStep(long holder);

    protected native void nativeShowMesh(long holder, boolean show);
}
