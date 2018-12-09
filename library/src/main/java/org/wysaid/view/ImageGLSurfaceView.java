package org.wysaid.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.AttributeSet;
import android.util.Log;

import org.wysaid.common.Common;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.texUtils.TextureRenderer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wysaid on 15/12/23.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */
public class ImageGLSurfaceView extends GLSurfaceView implements Renderer {

    public static final String LOG_TAG = Common.LOG_TAG;

    public enum DisplayMode {
        DISPLAY_SCALE_TO_FILL,
        DISPLAY_ASPECT_FILL,
        DISPLAY_ASPECT_FIT,
    }

    protected CGEImageHandler mImageHandler;
    protected float mFilterIntensity = 1.0f;

    public CGEImageHandler getImageHandler() {
        return mImageHandler;
    }

    protected TextureRenderer.Viewport mRenderViewport = new TextureRenderer.Viewport();

    public TextureRenderer.Viewport getRenderViewport() {
        return mRenderViewport;
    }

    protected int mImageWidth;
    protected int mImageHeight;
    protected int mViewWidth;
    protected int mViewHeight;

    public int getImageWidth() {
        return mImageWidth;
    }

    public int getImageheight() {
        return mImageHeight;
    }

    protected DisplayMode mDisplayMode = DisplayMode.DISPLAY_SCALE_TO_FILL;

    public DisplayMode getDisplayMode() {
        return mDisplayMode;
    }

    public void setDisplayMode(DisplayMode displayMode) {
        mDisplayMode = displayMode;
        calcViewport();
        requestRender();
    }

    public void setFilterWithConfig(final String config) {

        if (mImageHandler == null)
            return;

        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "set config after release!!");
                    return;
                }

                mImageHandler.setFilterWithConfig(config);
                requestRender();
            }
        });
    }

    protected final Object mSettingIntensityLock = new Object();
    protected int mSettingIntensityCount = 1;

    public void setFilterIntensityForIndex(final float intensity, final int index) {
        setFilterIntensityForIndex(intensity, index, true);
    }

    //See: CGEImageHandler.setFilterIntensityAtIndex
    public void setFilterIntensityForIndex(final float intensity, final int index, final  boolean shouldProcess) {
        if (mImageHandler == null)
            return;

        mFilterIntensity = intensity;

        synchronized (mSettingIntensityLock) {

            if (mSettingIntensityCount <= 0) {
                Log.i(LOG_TAG, "Too fast, skipping...");
                return;
            }
            --mSettingIntensityCount;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "set intensity after release!!");
                } else {
                    mImageHandler.setFilterIntensityAtIndex(mFilterIntensity, index, shouldProcess);
                    if(shouldProcess) {
                        requestRender();
                    }
                }

                synchronized (mSettingIntensityLock) {
                    ++mSettingIntensityCount;
                }
            }
        });
    }

    public void setFilterIntensity(final float intensity) {
        if (mImageHandler == null)
            return;

        mFilterIntensity = intensity;

        synchronized (mSettingIntensityLock) {

            if (mSettingIntensityCount <= 0) {
                Log.i(LOG_TAG, "Too fast, skipping...");
                return;
            }
            --mSettingIntensityCount;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "set intensity after release!!");
                } else {
                    mImageHandler.setFilterIntensity(mFilterIntensity, true);
                    requestRender();
                }

                synchronized (mSettingIntensityLock) {
                    ++mSettingIntensityCount;
                }
            }
        });
    }

    public void flush(final boolean runFilter, final Runnable runnable) {
        if (mImageHandler == null || runnable == null)
            return;

        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "flush after release!!");
                } else {
                    runnable.run();
                    if(runFilter) {
                        mImageHandler.revertImage();
                        mImageHandler.processFilters();
                    }
                    requestRender();
                }
            }
        });
    }

    // The runnable may be skipped when busy.
    public void lazyFlush(final boolean runFilter, final Runnable runnable) {
        if (mImageHandler == null || runnable == null)
            return;

        synchronized (mSettingIntensityLock) {

            if (mSettingIntensityCount <= 0) {
                Log.i(LOG_TAG, "Too fast, skipping...");
                return;
            }
            --mSettingIntensityCount;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "flush after release!!");
                } else {
                    if(runFilter) {
                        mImageHandler.revertImage();
                        mImageHandler.processFilters();
                    }
                    runnable.run();
                    requestRender();
                }

                synchronized (mSettingIntensityLock) {
                    ++mSettingIntensityCount;
                }
            }
        });
    }

    public void setImageBitmap(final Bitmap bmp) {

        if (bmp == null)
            return;

        if (mImageHandler == null) {
            Log.e(LOG_TAG, "Handler not initialized!");
            return;
        }

        mImageWidth = bmp.getWidth();
        mImageHeight = bmp.getHeight();

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mImageHandler == null) {
                    Log.e(LOG_TAG, "set image after release!!");
                    return;
                }

                if (mImageHandler.initWithBitmap(bmp)) {

                    calcViewport();
                    requestRender();

                } else {
                    Log.e(LOG_TAG, "setImageBitmap: init handler failed!");
                }
            }
        });
    }

    public interface QueryResultBitmapCallback {
        void get(Bitmap bmp);
    }

    public void getResultBitmap(final QueryResultBitmapCallback callback) {

        if (callback == null)
            return;

        queueEvent(new Runnable() {
            @Override
            public void run() {

                Bitmap bmp = mImageHandler.getResultBitmap();
                callback.get(bmp);
            }
        });
    }


    public ImageGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
//        setZOrderMediaOverlay(true);

        Log.i(LOG_TAG, "ImageGLSurfaceView Construct...");
    }

    public interface OnSurfaceCreatedCallback {
        void surfaceCreated();
    }

    protected OnSurfaceCreatedCallback mSurfaceCreatedCallback;

    public void setSurfaceCreatedCallback(OnSurfaceCreatedCallback callback) {
        mSurfaceCreatedCallback = callback;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "ImageGLSurfaceView onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);

        mImageHandler = new CGEImageHandler();

        mImageHandler.setDrawerFlipScale(1.0f, -1.0f);

        if (mSurfaceCreatedCallback != null) {
            mSurfaceCreatedCallback.surfaceCreated();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        mViewWidth = width;
        mViewHeight = height;
        calcViewport();
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        if (mImageHandler == null)
            return;

        GLES20.glViewport(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height);
        mImageHandler.drawResult();
    }

    public void release() {

        if (mImageHandler != null) {
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    Log.i(LOG_TAG, "ImageGLSurfaceView release...");

                    if (mImageHandler != null) {
                        mImageHandler.release();
                        mImageHandler = null;
                    }
                }
            });
        }
    }

    protected void calcViewport() {

        if (mDisplayMode == DisplayMode.DISPLAY_SCALE_TO_FILL) {
            mRenderViewport.x = 0;
            mRenderViewport.y = 0;
            mRenderViewport.width = mViewWidth;
            mRenderViewport.height = mViewHeight;
            return;
        }

        float scaling;

        scaling = mImageWidth / (float) mImageHeight;

        float viewRatio = mViewWidth / (float) mViewHeight;
        float s = scaling / viewRatio;

        int w, h;

        switch (mDisplayMode) {
            case DISPLAY_ASPECT_FILL: {
                //AspectFill
                if (s > 1.0) {
                    w = (int) (mViewHeight * scaling);
                    h = mViewHeight;
                } else {
                    w = mViewWidth;
                    h = (int) (mViewWidth / scaling);
                }
            }
            break;
            case DISPLAY_ASPECT_FIT: {
                //AspectFit
                if (s < 1.0) {
                    w = (int) (mViewHeight * scaling);
                    h = mViewHeight;
                } else {
                    w = mViewWidth;
                    h = (int) (mViewWidth / scaling);
                }
            }
            break;

            default:
                Log.i(LOG_TAG, "Error occured, please check the code...");
                return;
        }


        mRenderViewport.width = w;
        mRenderViewport.height = h;
        mRenderViewport.x = (mViewWidth - w) / 2;
        mRenderViewport.y = (mViewHeight - h) / 2;

        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height));
    }
}
