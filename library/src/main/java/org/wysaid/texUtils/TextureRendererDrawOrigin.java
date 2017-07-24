package org.wysaid.texUtils;

import android.opengl.GLES20;

/**
 * Created by wangyang on 15/7/23.
 */
public class TextureRendererDrawOrigin extends TextureRenderer {

    private static final String fshDrawOrigin = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform %s inputImageTexture;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = texture2D(inputImageTexture, texCoord);\n" +
            "}";

    //初始化默认的顶点序列等。
    protected TextureRendererDrawOrigin() {
        defaultInitialize();
    }

    protected TextureRendererDrawOrigin(boolean noDefaultInitialize) {
        if(!noDefaultInitialize)
            defaultInitialize();
    }

    public static TextureRendererDrawOrigin create(boolean isExternalOES) {
        TextureRendererDrawOrigin renderer = new TextureRendererDrawOrigin();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    @Override
    public boolean init(boolean isExternalOES) {
        return setProgramDefault(getVertexShaderString(), getFragmentShaderString(), isExternalOES);
    }

//    @Override
//    public void release() {
//        GLES20.glDeleteBuffers(1, new int[]{mVertexBuffer}, 0);
//        mVertexBuffer = 0;
//        mProgram.release();
//        mProgram = null;
//    }

    @Override
    public void renderTexture(int texID, Viewport viewport) {

        if(viewport != null) {
            GLES20.glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
        }

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(TEXTURE_2D_BINDABLE, texID);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);

        mProgram.bind();
        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }

    @Override
    public void setTextureSize(int w, int h) {
        mTextureWidth = w;
        mTextureHeight = h;
    }

    @Override
    public String getVertexShaderString() {
        return vshDrawDefault;
    }

    @Override
    public String getFragmentShaderString() {
        return fshDrawOrigin;
    }
}
