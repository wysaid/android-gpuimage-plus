package org.wysaid.common;

import android.opengl.GLES20;
import android.util.Log;

/**
 * Created by wangyang on 15/7/27.
 */

public class FrameBufferObject {
    private int mFramebufferID;

    public FrameBufferObject() {
        int[] buf = new int[1];
        GLES20.glGenFramebuffers(1, buf, 0);
        mFramebufferID = buf[0];
    }

    public void release() {
        GLES20.glDeleteFramebuffers(1, new int[]{mFramebufferID}, 0);
    }

    public void bind() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebufferID);
    }

    //将texture 绑定到该framebuffer的 GL_COLOR_ATTACHMENT0
    public void bindTexture(int texID) {
        bind();
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texID, 0);
        if(GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE)
        {
            Log.e(Common.LOG_TAG, "CGE::FrameBuffer::bindTexture2D - Frame buffer is not valid!");
        }
    }

}
