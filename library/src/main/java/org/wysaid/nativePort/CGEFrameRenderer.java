package org.wysaid.nativePort;

import java.nio.ByteBuffer;

/**
 * Created by wangyang on 15/11/26.
 */

//A wrapper for native class FrameRecorder
public class CGEFrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    protected ByteBuffer mNativeAddress;

    public CGEFrameRenderer() {
        mNativeAddress = nativeCreateRenderer();
    }

    //Avoid 'nativeCreateRenderer' being called.
    protected CGEFrameRenderer(int dummy) {

    }

    //srcWidth&srcheight stands for the external_texture's width&height
    //dstWidth&dstHeight stands for the recording resolution (default 640*480)
    //dstWidth/dstHeight should not be changed after "init()" is called.
    //srcWidth/srcHeight could be changed by calling "srcResize" function.
    public boolean init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        if(mNativeAddress != null)
            return nativeInit(mNativeAddress, srcWidth, srcHeight, dstWidth, dstHeight);
        return false;
    }

    //Will effect the framebuffer
    public void update(int externalTexture, float[] transformMatrix) {
        if(mNativeAddress != null)
            nativeUpdate(mNativeAddress, externalTexture, transformMatrix);
    }

    //Won't effect the framebuffer
    //the arguments means the viewport.
    public void render(int x, int y, int width, int height) {
        if(mNativeAddress != null)
            nativeRender(mNativeAddress, x, y, width, height);
    }

    public void drawCache() {
        if(mNativeAddress != null)
            nativeDrawCache(mNativeAddress);
    }

    //set the rotation of the camera texture
    public void setSrcRotation(float rad) {
        if(mNativeAddress != null)
            nativeSetSrcRotation(mNativeAddress, rad);
    }

    //set the flip/scaling for the camera texture
    public void setSrcFlipScale(float x, float y) {
        if(mNativeAddress != null)
            nativeSetSrcFlipScale(mNativeAddress, x, y);
    }

    //set the render result's rotation
    public void setRenderRotation(float rad) {
        if(mNativeAddress != null)
            nativeSetRenderRotation(mNativeAddress, rad);
    }

    //set the render result's flip/scaling
    public void setRenderFlipScale(float x, float y) {
        if(mNativeAddress != null)
            nativeSetRenderFlipScale(mNativeAddress, x, y);
    }

    //initialize the filters width config string
    public void setFilterWidthConfig(final String config) {
        if(mNativeAddress != null)
            nativeSetFilterWidthConfig(mNativeAddress, config);
    }

    //set the mask rotation (radian)
    public void setMaskRotation(float rot) {
        if(mNativeAddress != null)
            nativeSetMaskRotation(mNativeAddress, rot);
    }

    //set the mask flipscale
    public void setMaskFlipScale(float x, float y) {
        if(mNativeAddress != null)
            nativeSetMaskFlipScale(mNativeAddress, x, y);

    }


    //set the intensity of the filter
    public void setFilterIntensity(float value) {
        if(mNativeAddress != null)
            nativeSetFilterIntensity(mNativeAddress, value);
    }

    public void srcResize(int width, int height) {
        if(mNativeAddress != null)
            nativeSrcResize(mNativeAddress, width, height);
    }

    public  void release() {
        if(mNativeAddress != null) {
            nativeRelease(mNativeAddress);
            mNativeAddress = null;
        }
    }


    public void setMaskTexture(int texID, float aspectRatio) {
        if(mNativeAddress != null)
            nativeSetMaskTexture(mNativeAddress, texID, aspectRatio);
    }

    public void setMaskTextureRatio(float aspectRatio) {
        if(mNativeAddress != null)
            nativeSetMaskTextureRatio(mNativeAddress, aspectRatio);
    }

    public int queryBufferTexture() {
        if(mNativeAddress != null)
            return nativeQueryBufferTexture(mNativeAddress);
        return 0;
    }

    /////////////////      protected         ///////////////////////

    protected native ByteBuffer nativeCreateRenderer();
    protected native boolean nativeInit(ByteBuffer holder, int srcWidth, int srcHeight, int dstWidth, int dstHeight);
    protected native void nativeUpdate(ByteBuffer holder, int externalTexture, float[] transformMatrix);

    protected native void nativeRender(ByteBuffer holder, int x, int y, int width, int height);
    protected native void nativeDrawCache(ByteBuffer holder);

    protected native void nativeSetSrcRotation(ByteBuffer holder, float rad);
    protected native void nativeSetSrcFlipScale(ByteBuffer holder, float x, float y);
    protected native void nativeSetRenderRotation(ByteBuffer holder, float rad);
    protected native void nativeSetRenderFlipScale(ByteBuffer holder, float x, float y);
    protected native void nativeSetFilterWidthConfig(ByteBuffer holder, String config);
    protected native void nativeSetFilterIntensity(ByteBuffer holder, float value);
    protected native void nativeSetMaskRotation(ByteBuffer holder, float value);
    protected native void nativeSetMaskFlipScale(ByteBuffer holder, float x, float y);

    protected native void nativeSrcResize(ByteBuffer holder, int width, int height);

    protected native void nativeSetMaskTexture(ByteBuffer holder, int texID, float aspectRatio);
    protected native void nativeSetMaskTextureRatio(ByteBuffer holder, float aspectRatio);

    protected native void nativeRelease(ByteBuffer holder);

    protected native int nativeQueryBufferTexture(ByteBuffer holder);

}
