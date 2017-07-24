package org.wysaid.texUtils;

import android.opengl.GLES20;


/**
 * Created by wangyang on 15/7/18.
 */
public class TextureRendererWave extends TextureRendererDrawOrigin {

    private static final String fshWave = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform %s inputImageTexture;\n" +
            "uniform float motion;\n" +
            "const float angle = 20.0;" +
            "void main()\n" +
            "{\n" +
            "   vec2 coord;\n" +
            "   coord.x = texCoord.x + 0.01 * sin(motion + texCoord.x * angle);\n" +
            "   coord.y = texCoord.y + 0.01 * sin(motion + texCoord.y * angle);\n" +
            "   gl_FragColor = texture2D(inputImageTexture, coord);\n" +
            "}";

    private int mMotionLoc = 0;

    private boolean mAutoMotion = false;
    private float mMotion = 0.0f;
    private float mMotionSpeed = 0.0f;

    public TextureRendererWave() {
    }

    public static TextureRendererWave create(boolean isExternalOES) {
        TextureRendererWave renderer = new TextureRendererWave();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    @Override
    public boolean init(boolean isExternalOES) {
        if(setProgramDefault(vshDrawDefault, fshWave, isExternalOES)) {
            mProgram.bind();
            mMotionLoc = mProgram.getUniformLoc("motion");
            return true;
        }
        return false;
    }

    public void setWaveMotion(float motion) {
        mProgram.bind();
        GLES20.glUniform1f(mMotionLoc, motion);
    }

    public void setAutoMotion(float speed) {
        mMotionSpeed = speed;
        mAutoMotion = (speed != 0.0f);
    }

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
        if(mAutoMotion) {
            mMotion += mMotionSpeed;
            GLES20.glUniform1f(mMotionLoc, mMotion);
            if(mMotion > Math.PI * 20.0f) {
                mMotion -= Math.PI * 20.0f;
            }
        }
        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }
}
