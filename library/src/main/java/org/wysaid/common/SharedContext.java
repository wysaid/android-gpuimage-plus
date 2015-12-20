package org.wysaid.common;

import android.annotation.SuppressLint;
import android.opengl.EGL14;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/12/13.
 */

@SuppressLint("InlinedApi")
public class SharedContext {
    public static final String LOG_TAG = Common.LOG_TAG;
    public static final int EGL_RECORDABLE_ANDROID = 0x3142;

    private EGLContext mContext;
    private EGLConfig mConfig;
    private EGLDisplay mDisplay;
    private EGLSurface mSurface;
    private EGL10 mEgl;
    private GL10 mGl;

    private static int mBitsR = 8, mBitsG = 8, mBitsB = 8, mBitsA = 8;

    //注意， 设置之后将影响之后的所有操作
    static public void setContextColorBits(int r, int g, int b, int a) {
        mBitsR = r;
        mBitsG = g;
        mBitsB = b;
        mBitsA = a;
    }

    public static SharedContext create() {
        return create(EGL10.EGL_NO_CONTEXT, 64, 64, EGL10.EGL_PBUFFER_BIT, null);
    }

    public static SharedContext create(int width, int height) {
        return create(EGL10.EGL_NO_CONTEXT, width, height, EGL10.EGL_PBUFFER_BIT, null);
    }

    public static SharedContext create(EGLContext context, int width, int height) {
        return create(context, width, height, EGL10.EGL_PBUFFER_BIT, null);
    }

    //contextType: EGL10.EGL_PBUFFER_BIT
    //             EGL10.EGL_WINDOW_BIT
    //             EGL10.EGL_PIXMAP_BIT
    //             EGL_RECORDABLE_ANDROID ( = 0x3142 )
    //             etc.
    public static SharedContext create(EGLContext context, int width, int height, int contextType, Object obj) {

        SharedContext sharedContext = new SharedContext();
        if(!sharedContext.initEGL(context, width, height, contextType, obj)) {
            sharedContext.release();
            sharedContext = null;
        }
        return sharedContext;
    }

    public EGLContext getContext() {
        return mContext;
    }

    public EGLDisplay getDisplay() {
        return mDisplay;
    }

    public EGLSurface getSurface() {
        return mSurface;
    }

    public EGL10 getEGL() {
        return mEgl;
    }

    public GL10 getGL() {
        return mGl;
    }

    SharedContext() {}

    public void release() {
        Log.i(LOG_TAG, "#### CGESharedGLContext Destroying context... ####");
        if(mDisplay != EGL10.EGL_NO_DISPLAY) {
            mEgl.eglMakeCurrent(mDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
            mEgl.eglDestroyContext(mDisplay, mContext);
            mEgl.eglDestroySurface(mDisplay, mSurface);
            mEgl.eglTerminate(mDisplay);
        }

        mDisplay = EGL10.EGL_NO_DISPLAY;
        mSurface = EGL10.EGL_NO_SURFACE;
        mContext = EGL10.EGL_NO_CONTEXT;
    }

    public void makeCurrent() {
        if(!mEgl.eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
            Log.e(LOG_TAG, "eglMakeCurrent failed:" + mEgl.eglGetError());
        }
    }

    public boolean swapBuffers() {
        return mEgl.eglSwapBuffers(mDisplay, mSurface);
    }

    private boolean initEGL(EGLContext context, int width, int height, int contextType, Object obj) {

        int[] contextAttribList = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL10.EGL_NONE
        };

        int[] configSpec = {
                EGL10.EGL_SURFACE_TYPE, contextType,
                EGL10.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT,
                EGL10.EGL_RED_SIZE, 8, EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8, EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_NONE
        };

        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfig = new int[1];
        int[] version = new int[2];

        int surfaceAttribList[] = {
                EGL10.EGL_WIDTH, width,
                EGL10.EGL_HEIGHT, height,
                EGL10.EGL_NONE
        };

        mEgl = (EGL10) EGLContext.getEGL();

        if((mDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY)) == EGL10.EGL_NO_DISPLAY) {
            Log.e(LOG_TAG, String.format("eglGetDisplay() returned error 0x%x", mEgl.eglGetError()));
            return false;
        }

        if(!mEgl.eglInitialize(mDisplay, version)) {
            Log.e(LOG_TAG, String.format("eglInitialize() returned error 0x%x", mEgl.eglGetError()));
            return false;
        }

        Log.i(LOG_TAG, String.format("eglInitialize - major: %d, minor: %d", version[0], version[1]));

        if(!mEgl.eglChooseConfig(mDisplay, configSpec, configs, 1, numConfig)) {
            Log.e(LOG_TAG, String.format("eglChooseConfig() returned error 0x%x", mEgl.eglGetError()));
            return false;
        }

        Log.i(LOG_TAG, String.format("Config num: %d, has sharedContext: %s", numConfig[0], context == EGL10.EGL_NO_CONTEXT ? "NO" : "YES"));

        mConfig = configs[0];

        mContext = mEgl.eglCreateContext(mDisplay, mConfig,
                context, contextAttribList);
        if (mContext == EGL10.EGL_NO_CONTEXT) {
            Log.e(LOG_TAG, "eglCreateContext Failed!");
            return false;
        }

        switch (contextType) {
            case EGL10.EGL_PIXMAP_BIT:
                mSurface = mEgl.eglCreatePixmapSurface(mDisplay, mConfig, obj, surfaceAttribList);
                break;
            case EGL10.EGL_WINDOW_BIT:
                mSurface = mEgl.eglCreateWindowSurface(mDisplay, mConfig, obj, surfaceAttribList);
                break;
            case EGL10.EGL_PBUFFER_BIT:
            case EGL_RECORDABLE_ANDROID:
                mSurface = mEgl.eglCreatePbufferSurface(mDisplay, mConfig,
                        surfaceAttribList);
        }

        if (mSurface == EGL10.EGL_NO_SURFACE) {
            Log.e(LOG_TAG, "eglCreatePbufferSurface Failed!");
            return false;
        }

        if (!mEgl.eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)) {
            Log.e(LOG_TAG, "eglMakeCurrent failed:" + mEgl.eglGetError());
            return false;
        }

        int[] clientVersion = new int[1];
        mEgl.eglQueryContext(mDisplay, mContext, EGL14.EGL_CONTEXT_CLIENT_VERSION, clientVersion);
        Log.i(LOG_TAG, "EGLContext created, client version " + clientVersion[0]);

        mGl = (GL10) mContext.getGL();

        return true;
    }
}
