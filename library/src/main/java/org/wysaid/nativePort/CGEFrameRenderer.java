package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/11/26.
 */

//A wrapper for native class FrameRecorder
public class CGEFrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    protected long mNativeAddress;

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
        if(mNativeAddress != 0)
            return nativeInit(mNativeAddress, srcWidth, srcHeight, dstWidth, dstHeight);
        return false;
    }

    //Will effect the framebuffer
    public void update(int externalTexture, float[] transformMatrix) {
        if(mNativeAddress != 0)
            nativeUpdate(mNativeAddress, externalTexture, transformMatrix);
    }

    public void runProc() {
        if(mNativeAddress != 0)
            nativeRunProc(mNativeAddress);
    }

    //Won't effect the framebuffer
    //the arguments means the viewport.
    public void render(int x, int y, int width, int height) {
        if(mNativeAddress != 0)
            nativeRender(mNativeAddress, x, y, width, height);
    }

    public void drawCache() {
        if(mNativeAddress != 0)
            nativeDrawCache(mNativeAddress);
    }

    //set the rotation of the camera texture
    public void setSrcRotation(float rad) {
        if(mNativeAddress != 0)
            nativeSetSrcRotation(mNativeAddress, rad);
    }

    //set the flip/scaling for the camera texture
    public void setSrcFlipScale(float x, float y) {
        if(mNativeAddress != 0)
            nativeSetSrcFlipScale(mNativeAddress, x, y);
    }

    //set the render result's rotation
    public void setRenderRotation(float rad) {
        if(mNativeAddress != 0)
            nativeSetRenderRotation(mNativeAddress, rad);
    }

    //set the render result's flip/scaling
    public void setRenderFlipScale(float x, float y) {
        if(mNativeAddress != 0)
            nativeSetRenderFlipScale(mNativeAddress, x, y);
    }

    //initialize the filters width config string
    public void setFilterWidthConfig(final String config) {
        if(mNativeAddress != 0)
            nativeSetFilterWidthConfig(mNativeAddress, config);
    }

    //set the mask rotation (radian)
    public void setMaskRotation(float rot) {
        if(mNativeAddress != 0)
            nativeSetMaskRotation(mNativeAddress, rot);
    }

    //set the mask flipscale
    public void setMaskFlipScale(float x, float y) {
        if(mNativeAddress != 0)
            nativeSetMaskFlipScale(mNativeAddress, x, y);

    }


    //set the intensity of the filter
    public void setFilterIntensity(float value) {
        if(mNativeAddress != 0)
            nativeSetFilterIntensity(mNativeAddress, value);
    }

    public void srcResize(int width, int height) {
        if(mNativeAddress != 0)
            nativeSrcResize(mNativeAddress, width, height);
    }

    public  void release() {
        if(mNativeAddress != 0) {
            nativeRelease(mNativeAddress);
            mNativeAddress = 0;
        }
    }


    public void setMaskTexture(int texID, float aspectRatio) {
        if(mNativeAddress != 0)
            nativeSetMaskTexture(mNativeAddress, texID, aspectRatio);
    }

    public void setMaskTextureRatio(float aspectRatio) {
        if(mNativeAddress != 0)
            nativeSetMaskTextureRatio(mNativeAddress, aspectRatio);
    }

    public int queryBufferTexture() {
        if(mNativeAddress != 0)
            return nativeQueryBufferTexture(mNativeAddress);
        return 0;
    }

    public long getImageHandler() {
        return nativeGetImageHandler(mNativeAddress);
    }

    public void bindImageFBO() {
        nativeBindImageFBO(mNativeAddress);
    }

    //nativeFilter 为 JNI 下的 CGEImageFilterInterfaceAbstract 或者其子类.
    public void processWithFilter(long nativeFilter) {
        nativeProcessWithFilter(mNativeAddress, nativeFilter);
    }

    /////////////////      protected         ///////////////////////

    protected native long nativeCreateRenderer();
    protected native boolean nativeInit(long holder, int srcWidth, int srcHeight, int dstWidth, int dstHeight);
    protected native void nativeUpdate(long holder, int externalTexture, float[] transformMatrix);
    protected native void nativeRunProc(long holder);

    protected native void nativeRender(long holder, int x, int y, int width, int height);
    protected native void nativeDrawCache(long holder);

    protected native void nativeSetSrcRotation(long holder, float rad);
    protected native void nativeSetSrcFlipScale(long holder, float x, float y);
    protected native void nativeSetRenderRotation(long holder, float rad);
    protected native void nativeSetRenderFlipScale(long holder, float x, float y);
    protected native void nativeSetFilterWidthConfig(long holder, String config);
    protected native void nativeSetFilterIntensity(long holder, float value);
    protected native void nativeSetMaskRotation(long holder, float value);
    protected native void nativeSetMaskFlipScale(long holder, float x, float y);

    protected native void nativeSrcResize(long holder, int width, int height);

    protected native void nativeSetMaskTexture(long holder, int texID, float aspectRatio);
    protected native void nativeSetMaskTextureRatio(long holder, float aspectRatio);

    protected native void nativeRelease(long holder);

    protected native int nativeQueryBufferTexture(long holder);

    protected native long nativeGetImageHandler(long holder);
    protected native void nativeBindImageFBO(long holder);

    //辅助方法
    protected native void nativeProcessWithFilter(long holder, long nativeFilter);

}
