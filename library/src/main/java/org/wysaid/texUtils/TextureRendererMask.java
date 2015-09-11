package org.wysaid.texUtils;

import android.opengl.GLES20;

/**
 * Created by wangyang on 15/8/20.
 */
public class TextureRendererMask extends TextureRendererDrawOrigin {

    private static final String vshMask = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 texCoord;\n" +
            "varying vec2 maskCoord;\n" +

            "uniform mat2 rotation;\n" +
            "uniform vec2 flipScale;\n" +

            "uniform mat2 maskRotation;\n" +
            "uniform vec2 maskFlipScale;\n" +

            "uniform mat4 transform;\n" +

            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n" +

            "   vec2 coord = flipScale * (vPosition / 2.0 * rotation) + 0.5;\n" +
            "   texCoord = (transform * vec4(coord, 0.0, 1.0)).xy;\n" +

            "   maskCoord = maskFlipScale * (vPosition / 2.0 * maskRotation) + 0.5;\n" +
            "}";

    private static final String fshMask = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "varying vec2 maskCoord;\n" +
            "uniform %s inputImageTexture;\n" +
            "uniform sampler2D maskTexture;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = texture2D(inputImageTexture, texCoord);\n" +
            "   vec4 maskColor = texture2D(maskTexture, maskCoord);\n" +
            //不预乘
//            "   maskColor.rgb *= maskColor.a;\n" +
            "   gl_FragColor *= maskColor;\n" +
            "}";

    protected static final String MASK_ROTATION_NAME = "maskRotation";
    protected static final String MASK_FLIPSCALE_NAME = "maskFlipScale";
    protected static final String MASK_TEXTURE_NAME = "maskTexture";

    protected int mMaskRotLoc, mMaskFlipscaleLoc;
    protected int mMaskTexture;

    public static TextureRendererMask create(boolean isExternalOES) {
        TextureRendererMask renderer = new TextureRendererMask();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    @Override
    public boolean init(boolean isExternalOES) {
        if(setProgramDefualt(getVertexShaderString(), getFragmentShaderString(), isExternalOES)) {
            mProgram.bind();
            mMaskRotLoc = mProgram.getUniformLoc(MASK_ROTATION_NAME);
            mMaskFlipscaleLoc = mProgram.getUniformLoc(MASK_FLIPSCALE_NAME);
            mProgram.sendUniformi(MASK_TEXTURE_NAME, 1);
            setMaskRotation(0.0f);
            setMaskFlipscale(1.0f, 1.0f);
            return true;
        }
        return false;
    }

    public void setMaskRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);

        float rot[] = new float[] {
                cosRad, sinRad,
                -sinRad, cosRad
        };

        assert mProgram != null : "setRotation must not be called before init!";

        mProgram.bind();
        GLES20.glUniformMatrix2fv(mMaskRotLoc, 1, false, rot, 0);
    }

    public void setMaskFlipscale(float x, float y) {
        mProgram.bind();
        GLES20.glUniform2f(mMaskFlipscaleLoc, x, y);
    }

    public void setMaskTexture(int texID) {
        if(texID == mMaskTexture)
            return;

        GLES20.glDeleteTextures(1, new int[]{mMaskTexture}, 0);
        mMaskTexture = texID;
    }

    @Override
    public void renderTexture(int texID, Viewport viewport) {

        if(viewport != null) {
            GLES20.glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
        }

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(TEXTURE_2D_BINDABLE, texID);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mMaskTexture);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);

        mProgram.bind();
        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }

    @Override
    public String getVertexShaderString() {
        return vshMask;
    }

    @Override
    public String getFragmentShaderString() {
        return fshMask;
    }

    @Override
    public void release() {
        super.release();
        GLES20.glDeleteTextures(1, new int[]{mMaskTexture}, 0);
    }

}
