package org.wysaid.common;

import android.graphics.Bitmap;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import java.nio.FloatBuffer;

/**
 * Created by wangyang on 15/7/27.
 */

public class Common {

    public static final boolean DEBUG = true;
    public static final String LOG_TAG = "libCGE_java";
    public static final float[] FULLSCREEN_VERTICES = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

    public static void checkGLError(final String tag) {
        int loopCnt = 0;
        for(int err = GLES20.glGetError(); loopCnt < 32 && err != GLES20.GL_FALSE; err = GLES20.glGetError(), ++loopCnt)
        {
            String msg;
            switch (err)
            {
                case GLES20.GL_INVALID_ENUM:
                    msg = "invalid enum"; break;
                case GLES20.GL_INVALID_FRAMEBUFFER_OPERATION:
                    msg = "invalid framebuffer operation"; break;
                case GLES20.GL_INVALID_OPERATION:
                    msg = "invalid operation";break;
                case GLES20.GL_INVALID_VALUE:
                    msg = "invalid value";break;
                case GLES20.GL_OUT_OF_MEMORY:
                    msg = "out of memory"; break;
                default: msg = "unknown error";
            }
            Log.e(LOG_TAG, String.format("After tag \"%s\" glGetError %s(0x%x) ", tag, msg, err));
        }
    }

    public static void texParamHelper(int type, int filter, int wrap) {
        GLES20.glTexParameterf(type, GLES20.GL_TEXTURE_MIN_FILTER, filter);
        GLES20.glTexParameterf(type, GLES20.GL_TEXTURE_MAG_FILTER, filter);
        GLES20.glTexParameteri(type, GLES20.GL_TEXTURE_WRAP_S, wrap);
        GLES20.glTexParameteri(type, GLES20.GL_TEXTURE_WRAP_T, wrap);
    }

    public static int genBlankTextureID(int width, int height) {
        return genBlankTextureID(width, height, GLES20.GL_LINEAR, GLES20.GL_CLAMP_TO_EDGE);
    }

    public static int genBlankTextureID(int width, int height, int filter, int wrap) {
        int[] texID = new int[1];
        GLES20.glGenTextures(1, texID, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texID[0]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        texParamHelper(GLES20.GL_TEXTURE_2D, filter, wrap);
        return texID[0];
    }

    public static int genNormalTextureID(Bitmap bmp) {
        return genNormalTextureID(bmp, GLES20.GL_LINEAR, GLES20.GL_CLAMP_TO_EDGE);
    }

    public static int genNormalTextureID(Bitmap bmp, int filter, int wrap) {
        int[] texID = new int[1];
        GLES20.glGenTextures(1, texID, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texID[0]);
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bmp, 0);
        texParamHelper(GLES20.GL_TEXTURE_2D, filter, wrap);
        return texID[0];
    }

    public static int genSurfaceTextureID() {
        int[] texID = new int[1];
        GLES20.glGenTextures(1, texID, 0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texID[0]);
        texParamHelper(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_LINEAR, GLES20.GL_CLAMP_TO_EDGE);
        return texID[0];
    }

    public static void deleteTextureID(int texID) {
        GLES20.glDeleteTextures(1, new int[]{texID}, 0);
    }

    public static int genFullscreenVertexArrayBuffer() {
        int[] vertexBuffer = new int[1];
        GLES20.glGenBuffers(1, vertexBuffer, 0);

        if(vertexBuffer[0] == 0) {
            Log.e(LOG_TAG, "Invalid VertexBuffer! You must call this within an OpenGL thread!");
            return 0;
        }

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vertexBuffer[0]);
        FloatBuffer buffer = FloatBuffer.allocate(FULLSCREEN_VERTICES.length);
        buffer.put(FULLSCREEN_VERTICES).position(0);
        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, 32, buffer, GLES20.GL_STATIC_DRAW);
        return vertexBuffer[0];
    }
}
