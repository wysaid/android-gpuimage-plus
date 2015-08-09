package org.wysaid.myUtils;

import android.opengl.GLES20;
import android.util.Log;

/**
 * Created by wangyang on 15/7/27.
 */

public class Common {

    public static final boolean DEBUG = true;
    public static final String LOG_TAG = "wysaid";

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
}
