package org.wysaid.common;

import android.opengl.GLES20;
import android.util.Log;

import java.nio.FloatBuffer;

/**
 * Created by wangyang on 15/8/8.
 * A simple direct drawer with flip, scale, rotate
 */
public class TextureDrawer {

    private static final String vsh = "" +
            "attribute vec2 vPosition;\n"+
            "varying vec2 texCoord;\n"+
            "uniform mat2 rotation;\n"+
            "uniform vec2 flipScale;\n"+
            "void main()\n"+
            "{\n"+
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n"+
            "   texCoord = flipScale * (vPosition / 2.0 * rotation) + 0.5;\n"+
            "}";

    private static final String fsh = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform sampler2D inputImageTexture;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = texture2D(inputImageTexture, texCoord);\n" +
            "}";

    public static final float[] vertices = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    public static final int DRAW_FUNCTION = GLES20.GL_TRIANGLE_FAN;

    private ProgramObject mProgram;
    private int mVertBuffer;
    private int mRotLoc, mFlipScaleLoc;

    private TextureDrawer() {
    }

    protected boolean init() {
        mProgram = new ProgramObject();
        if(!mProgram.init(vsh, fsh)) {
            mProgram.release();
            mProgram = null;
            return false;
        }

        mProgram.bind();

        mRotLoc = mProgram.getUniformLoc("rotation");
        mFlipScaleLoc = mProgram.getUniformLoc("flipScale");

        int[] vertBuffer = new int[1];
        GLES20.glGenBuffers(1, vertBuffer, 0);
        mVertBuffer = vertBuffer[0];

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertBuffer);
        FloatBuffer buffer = FloatBuffer.allocate(vertices.length);
        buffer.put(vertices).position(0);

        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, 32, buffer, GLES20.GL_STATIC_DRAW);
        setRotation(0.0f);
        setFlipScale(1.0f, 1.0f);
        return true;
    }


    public static TextureDrawer create() {
        TextureDrawer drawer = new TextureDrawer();
        if(!drawer.init())
        {
            Log.e(Common.LOG_TAG, "TextureDrawer create failed!");
            drawer.release();
            drawer = null;
        }
        return drawer;
    }

    public void release() {
        mProgram.release();
        GLES20.glDeleteBuffers(1, new int[]{mVertBuffer}, 0);
        mProgram = null;
        mVertBuffer = 0;
    }

    public void drawTexture(int texID) {
        drawTexture(texID, GLES20.GL_TEXTURE_2D);
    }

    public void drawTexture(int texID, int type) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(type, texID);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);
        mProgram.bind();
        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }

    //特殊外部辅助用法
    public void bindVertexBuffer() {
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertBuffer);
    }

    public void setRotation(float rad) {
        _rotate(mRotLoc, rad);
    }

    public void setFlipScale(float x, float y) {
        mProgram.bind();
        GLES20.glUniform2f(mFlipScaleLoc, x, y);
    }

    private void _rotate(int location, float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);

        final float[] rotation = new float[] {
                cosRad, sinRad,
                -sinRad, cosRad
        };

        mProgram.bind();
        GLES20.glUniformMatrix2fv(location, 1, false, rotation, 0);
    }

}
