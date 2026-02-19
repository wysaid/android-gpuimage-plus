package org.wysaid.view;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.camera.Camera1Provider;
import org.wysaid.camera.CameraInstance;
import org.wysaid.camera.ICameraProvider;
import org.wysaid.common.Common;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Base class for camera GL views. Handles viewport calculation, flash control,
 * camera switching, and provides the {@link ICameraProvider} abstraction.
 *
 * <p>By default uses Camera1 for backward compatibility.
 * Call {@link #setCameraProvider(ICameraProvider)} before the surface is created
 * to use a different backend (e.g., CameraX).
 */
public class CameraGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer {

    public CameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 0, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
//        setZOrderOnTop(true);
//        setZOrderMediaOverlay(true);
    }

    public static final String LOG_TAG = Common.LOG_TAG;

    public int mMaxTextureSize = 0;

    protected int mViewWidth;
    protected int mViewHeight;

    protected int mRecordWidth = 480;
    protected int mRecordHeight = 640;

    /** The camera provider instance. Defaults to Camera1 for backward compatibility. */
    protected ICameraProvider mCameraProvider;

    /**
     * Set a custom camera provider (e.g., CameraX). Should be called before the surface is created.
     *
     * @param provider The camera provider to use.
     */
    public void setCameraProvider(ICameraProvider provider) {
        mCameraProvider = provider;
    }

    /**
     * Get the current camera provider. Creates a default Camera1Provider if none was set.
     */
    public ICameraProvider getCameraProvider() {
        if (mCameraProvider == null) {
            mCameraProvider = new Camera1Provider();
        }
        return mCameraProvider;
    }

    //isBigger 为true 表示当宽高不满足时，取最近的较大值.
    // 若为 false 则取较小的
    public void setPictureSize(int width, int height, boolean isBigger) {
        //默认会旋转90度.
        getCameraProvider().setPictureSize(height, width, isBigger);
    }

    /**
     * Set flash mode using {@link ICameraProvider.FlashMode} enum.
     * Preferred over {@link #setFlashLightMode(String)}.
     */
    public boolean setFlashMode(ICameraProvider.FlashMode mode) {
        if (!getContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH)) {
            Log.e(LOG_TAG, "No flash light is supported by current device!");
            return false;
        }
        if (!mIsCameraBackForward) {
            return false;
        }
        return getCameraProvider().setFlashMode(mode);
    }

    // mode value should be:
    //    Camera.Parameters.FLASH_MODE_AUTO;
    //    Camera.Parameters.FLASH_MODE_OFF;
    //    Camera.Parameters.FLASH_MODE_ON;
    //    Camera.Parameters.FLASH_MODE_RED_EYE
    //    Camera.Parameters.FLASH_MODE_TORCH 等
    /**
     * @deprecated Use {@link #setFlashMode(ICameraProvider.FlashMode)} instead.
     */
    @Deprecated
    public synchronized boolean setFlashLightMode(String mode) {

        if (!getContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH)) {
            Log.e(LOG_TAG, "No flash light is supported by current device!");
            return false;
        }

        if (!mIsCameraBackForward) {
            return false;
        }

        // Delegate to the new enum-based API
        ICameraProvider.FlashMode flashMode = ICameraProvider.camera1ToFlashMode(mode);
        return getCameraProvider().setFlashMode(flashMode);
    }

    protected int mMaxPreviewWidth = 1920;
    protected int mMaxPreviewHeight = 1920;

    public static class Viewport {
         public int x, y, width, height;
    }

    protected Viewport mDrawViewport = new Viewport();

    public Viewport getDrawViewport() {
        return mDrawViewport;
    }

    //The max preview size. Make it greater if you want to preview with 2K/4K or more!
    void setMaxPreviewSize(int w, int h) {
        mMaxPreviewWidth = w;
        mMaxPreviewHeight = h;
    }

    protected boolean mFitFullView = false;

    public void setFitFullView(boolean fit) {
        mFitFullView = fit;
        calcViewport();
    }

    // Whether to use back camera
    protected boolean mIsCameraBackForward = true;

    public boolean isCameraBackForward() {
        return mIsCameraBackForward;
    }

    /**
     * @deprecated Use {@link #getCameraProvider()} instead.
     */
    @Deprecated
    public CameraInstance cameraInstance() {
        return CameraInstance.getInstance();
    }

    //should be called before 'onSurfaceCreated'.
    public void presetCameraForward(boolean isBackForward) {
        mIsCameraBackForward = isBackForward;
    }

    // Note: The size of recording will affect the size of preview
    // Here width and height represent portrait size
    // Set before onSurfaceCreated to take effect
    public void presetRecordingSize(int width, int height) {
        if (width > mMaxPreviewWidth || height > mMaxPreviewHeight) {
            float scaling = Math.min(mMaxPreviewWidth / (float) width, mMaxPreviewHeight / (float) height);
            width = (int) (width * scaling);
            height = (int) (height * scaling);
        }

        Log.i(Common.LOG_TAG, "presetRecordingSize: " + width + "x" + height);

        mRecordWidth = width;
        mRecordHeight = height;
        getCameraProvider().setPreferredPreviewSize(width, height);
    }

    public void resumePreview() {

    }

    public void stopPreview() {
        getCameraProvider().stopPreview();
    }

    protected void onSwitchCamera() {

    }

    /**
     * Switch between front and back camera.
     *
     * <p><b>Must be called from the main thread</b> — camera lifecycle operations
     * ({@link ICameraProvider#closeCamera()}, {@link ICameraProvider#openCamera}) require it.
     */
    public final void switchCamera() {
        mIsCameraBackForward = !mIsCameraBackForward;

        final ICameraProvider.CameraFacing facing = mIsCameraBackForward
                ? ICameraProvider.CameraFacing.BACK
                : ICameraProvider.CameraFacing.FRONT;

        getCameraProvider().closeCamera();
        onSwitchCamera();
        getCameraProvider().openCamera(facing, new ICameraProvider.CameraOpenCallback() {
            @Override
            public void cameraReady() {
                resumePreview();
            }
        });
        requestRender();
    }

    //Attention， 'focusAtPoint' will change focus mode to 'FOCUS_MODE_AUTO'
    //If you want to keep the previous focus mode， please reset the focus mode after 'AutoFocusCallback'.
    //x,y should be: [0, 1]， stands for 'touchEventPosition / viewSize'.

    /**
     * Focus at a point using the new {@link ICameraProvider.AutoFocusCallback}.
     */
    public void focusAtPoint(float x, float y, ICameraProvider.AutoFocusCallback focusCallback) {
        getCameraProvider().focusAtPoint(y, 1.0f - x, 0.2f, focusCallback);
    }

    /**
     * @deprecated Use {@link #focusAtPoint(float, float, ICameraProvider.AutoFocusCallback)} instead.
     */
    @Deprecated
    public void focusAtPoint(float x, float y, Camera.AutoFocusCallback focusCallback) {
        getCameraProvider().focusAtPoint(y, 1.0f - x, 0.2f,
                focusCallback != null ? success -> focusCallback.onAutoFocus(success, null) : null);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        getCameraProvider().closeCamera();
    }

    public interface OnCreateCallback {
        void createOver();
    }

    protected OnCreateCallback mOnCreateCallback;

    // Customize some initialization operations
    public void setOnCreateCallback(final OnCreateCallback callback) {
        mOnCreateCallback = callback;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        int texSize[] = new int[1];

        GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, texSize, 0);
        mMaxTextureSize = texSize[0];

        if (mOnCreateCallback != null) {
            mOnCreateCallback.createOver();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(LOG_TAG, String.format("onSurfaceChanged: %d x %d", width, height));

        GLES20.glClearColor(0, 0, 0, 0);

        mViewWidth = width;
        mViewHeight = height;

        calcViewport();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        //Nothing . See `CameraGLSurfaceViewWithTexture` or `CameraGLSurfaceViewWithBuffer`
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(LOG_TAG, "glsurfaceview onResume...");
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "glsurfaceview onPause in...");

        getCameraProvider().closeCamera();
        super.onPause();
        Log.i(LOG_TAG, "glsurfaceview onPause out...");
    }

    public interface ReleaseOKCallback {

        void releaseOK();
    }

    protected void onRelease() {

    }

    public final void release(final ReleaseOKCallback callback) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                onRelease();

                Log.i(LOG_TAG, "GLSurfaceview release...");
                if (callback != null)
                    callback.releaseOK();
            }
        });
    }

    public interface TakePictureCallback {
        //You can recycle the bitmap.
        void takePictureOK(Bitmap bmp);
    }

    public void takeShot(final TakePictureCallback callback) {
    }

    protected void calcViewport() {

        float scaling = mRecordWidth / (float) mRecordHeight;
        float viewRatio = mViewWidth / (float) mViewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if (mFitFullView) {
            //撑满全部view(内容大于view)
            if (s > 1.0) {
                w = (int) (mViewHeight * scaling);
                h = mViewHeight;
            } else {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            }
        } else {
            //显示全部内容(内容小于view)
            if (s > 1.0) {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            } else {
                h = mViewHeight;
                w = (int) (mViewHeight * scaling);
            }
        }

        mDrawViewport.width = w;
        mDrawViewport.height = h;
        mDrawViewport.x = (mViewWidth - mDrawViewport.width) / 2;
        mDrawViewport.y = (mViewHeight - mDrawViewport.height) / 2;
        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height));
    }
}
