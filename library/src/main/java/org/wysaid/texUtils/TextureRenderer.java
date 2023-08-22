package org.wysaid.texUtils;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.Common;
import org.wysaid.common.ProgramObject;

import java.nio.FloatBuffer;

/**
 * Created by wangyang on 15/7/23.
 */
public abstract class TextureRenderer {

    public static final String LOG_TAG = Common.LOG_TAG;

    //初始化program 等
    public abstract boolean init(boolean isExternalOES);

    //为了保证GLContext 的对应， 不能等待finalize
    public void release() {
        if(mVertexBuffer != 0) {
            GLES20.glDeleteBuffers(1, new int[]{mVertexBuffer}, 0);
            mVertexBuffer = 0;
        }

        if(mProgram != null) {
            mProgram.release();
            mProgram = null;
        }
    }

    public abstract void renderTexture(int texID, Viewport viewport);

    public abstract void setTextureSize(int width, int height);

    public abstract String getVertexShaderString();

    public abstract String getFragmentShaderString();

    public static class Viewport {
        public int x, y;
        public int width, height;
        public Viewport() {}
        public Viewport(int _x, int _y, int _width, int _height) {
            x = _x;
            y = _y;
            width = _width;
            height = _height;
        }
    }

    ////////////////////////////////////////////////////////////////

    protected static final String REQUIRE_STRING_EXTERNAL_OES = "#extension GL_OES_EGL_image_external : require\n";
    protected static final String SAMPLER2D_VAR_EXTERNAL_OES = "samplerExternalOES";
    protected static final String SAMPLER2D_VAR = "sampler2D";

    protected static final String vshDrawDefault = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 texCoord;\n" +
            "uniform mat4 transform;\n" +
            "uniform mat2 rotation;\n" +
            "uniform vec2 flipScale;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
            "   vec2 coord = flipScale * (vPosition / 2.0 * rotation) + 0.5;\n" +
            "   texCoord = (transform * vec4(coord, 0.0, 1.0)).xy;\n" +
            "}";


    protected static final String POSITION_NAME = "vPosition";
    protected static final String ROTATION_NAME = "rotation";
    protected static final String FLIPSCALE_NAME = "flipScale";
    protected static final String TRANSFORM_NAME = "transform";

    public static final float[] vertices = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    public static final int DRAW_FUNCTION = GLES20.GL_TRIANGLE_STRIP;

    protected int TEXTURE_2D_BINDABLE;

    protected int mVertexBuffer;
    protected ProgramObject mProgram;

    protected int mTextureWidth, mTextureHeight;

    protected int mRotationLoc, mFlipScaleLoc, mTransformLoc;

    //设置界面旋转弧度 -- 录像时一般是 PI / 2 (也就是 90°) 的整数倍
    public void setRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);

        float rot[] = new float[] {
                cosRad, sinRad,
                -sinRad, cosRad
        };

        assert mProgram != null : "setRotation must not be called before init!";

        mProgram.bind();
        GLES20.glUniformMatrix2fv(mRotationLoc, 1, false, rot, 0);
    }

    public void setFlipscale(float x, float y) {
        mProgram.bind();
        GLES20.glUniform2f(mFlipScaleLoc, x, y);
    }

    public void setTransform(float[] matrix) {
        mProgram.bind();
        GLES20.glUniformMatrix4fv(mTransformLoc, 1, false, matrix, 0);
    }

    protected boolean setProgramDefault(String vsh, String fsh, boolean isExternalOES) {
        TEXTURE_2D_BINDABLE = isExternalOES ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES : GLES20.GL_TEXTURE_2D;
        mProgram = new ProgramObject();
        mProgram.bindAttribLocation(POSITION_NAME, 0);
        String fshResult = (isExternalOES ? REQUIRE_STRING_EXTERNAL_OES : "") + String.format(fsh, isExternalOES ? SAMPLER2D_VAR_EXTERNAL_OES : SAMPLER2D_VAR);
        if(mProgram.init(vsh, fshResult)) {
            mRotationLoc = mProgram.getUniformLoc(ROTATION_NAME);
            mFlipScaleLoc = mProgram.getUniformLoc(FLIPSCALE_NAME);
            mTransformLoc = mProgram.getUniformLoc(TRANSFORM_NAME);
            setRotation(0.0f);
            setFlipscale(1.0f, 1.0f);
            setTransform(new float[]{
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
            });
            return true;
        }
        return false;
    }

    protected void defaultInitialize() {
        int[] vertexBuffer = new int[1];
        GLES20.glGenBuffers(1, vertexBuffer, 0);
        mVertexBuffer = vertexBuffer[0];

        if(mVertexBuffer == 0) {
            Log.e(LOG_TAG, "Invalid VertexBuffer!");
        }

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertexBuffer);
        FloatBuffer buffer = FloatBuffer.allocate(vertices.length);
        buffer.put(vertices).position(0);
        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, 32, buffer, GLES20.GL_STATIC_DRAW);
    }
}
