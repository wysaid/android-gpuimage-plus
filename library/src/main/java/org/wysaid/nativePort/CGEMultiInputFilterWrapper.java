package org.wysaid.nativePort;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Data: 2019/04/17
 * Description:
 */

// A demo for multi input.
public class CGEMultiInputFilterWrapper {

    static {
        NativeLibraryLoader.load();
    }

    private long mNativeAddress = 0;

    private CGEMultiInputFilterWrapper() {}

    static public CGEMultiInputFilterWrapper create(String vsh, String fsh) {
        CGEMultiInputFilterWrapper wrapper = new CGEMultiInputFilterWrapper();
        wrapper.mNativeAddress = wrapper.nativeCreate(vsh, fsh);
        if(wrapper.mNativeAddress == 0) {
            wrapper = null;
        }
        return wrapper;
    }

    // The native filter should not be deleted if passed to an image handler!
    public void release(boolean deleteFilter) {
        if(mNativeAddress != 0) {
            if(deleteFilter) {
                nativeRelease(mNativeAddress);
            }
            mNativeAddress = 0;
        }
    }

    IntBuffer mInputTextureBuffer;

    public long getNativeAddress() {
        return mNativeAddress;
    }

    public void updateInputTextures(IntBuffer inputTextureBuffer, int count) {
        nativeUpdateInputTextures(mNativeAddress, inputTextureBuffer, count);
    }

    public void updateInputTextures(int[] inputTextures) {
        if(mInputTextureBuffer == null || mInputTextureBuffer.capacity() < inputTextures.length) {
            mInputTextureBuffer = ByteBuffer.allocateDirect(inputTextures.length * Integer.BYTES).order(ByteOrder.nativeOrder()).asIntBuffer();
        }
        mInputTextureBuffer.put(inputTextures);
        mInputTextureBuffer.position(0);
        nativeUpdateInputTextures(mNativeAddress, mInputTextureBuffer, inputTextures.length);
    }


    /////////////////////////////////

    protected static native long nativeCreate(String vsh, String fsh);
    protected static native void nativeRelease(long holder);

    protected native void nativeUpdateInputTextures(long holder, IntBuffer inputTextureBuffer, int count);
}
