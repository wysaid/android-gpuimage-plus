package org.wysaid.view;

/**
 * Created by wangyang on 15/7/27.
 */


import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.ExifInterface;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.camera.CameraInstance;
import org.wysaid.common.Common;
import org.wysaid.common.FrameBufferObject;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.texUtils.TextureRendererDrawOrigin;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */
public class CameraGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final String LOG_TAG = Common.LOG_TAG;

    public int maxTextureSize = 0;

    public int viewWidth;
    public int viewHeight;

    protected int mRecordWidth = 480;
    protected int mRecordHeight = 640;

    protected SurfaceTexture mSurfaceTexture;
    protected int mTextureID;

    protected CGEFrameRecorder mFrameRecorder;

    public CGEFrameRecorder getRecorder() {
        return mFrameRecorder;
    }

    public int maxPreviewWidth = 1280;
    public int maxPreviewHeight = 1280;

    //将对preview进行缩放。 使用默认值即可， 最好不要修改
    void setMaxPreviewSize(int w, int h) {
        maxPreviewWidth = w;
        maxPreviewHeight = h;
    }

    public class ClearColor {
        public float r, g, b, a;
    }

    public ClearColor clearColor;

    protected TextureRenderer.Viewport mDrawViewport = new TextureRenderer.Viewport();

    protected boolean mIsUsingMask = false;

    protected boolean mFitFullView = false;

    protected boolean mIsTransformMatrixSet = false;

    public void setFitFullView(boolean fit) {
        mFitFullView = fit;
        if(mFrameRecorder != null)
            calcViewport();
    }

    public boolean isUsingMask() {
        return mIsUsingMask;
    }

    protected float mMaskAspectRatio = 1.0f;

    //是否使用后置摄像头
    protected boolean mIsCameraBackForward = true;

    public boolean isCameraBackForward() {
        return mIsCameraBackForward;
    }

    public void setClearColor(float r, float g, float b, float a) {
        clearColor.r = r;
        clearColor.g = g;
        clearColor.b = b;
        clearColor.a = a;
        queueEvent(new Runnable() {
            @Override
            public void run() {
                GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
                GLES20.glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            }
        });
    }

    public CameraInstance cameraInstance() {
        return CameraInstance.getInstance();
    }

    //在onSurfaceCreated之前设置有效
    public void presetCameraForward(boolean isBackForward) {
        mIsCameraBackForward = isBackForward;
    }

    //注意， 录制的尺寸将影响preview的尺寸
    //这里的width和height表示竖屏尺寸
    //在onSurfaceCreated之前设置有效
    public void presetRecordingSize(int width, int height) {
        if(width > maxPreviewWidth || height > maxPreviewHeight) {
            float scaling = Math.min(maxPreviewWidth / (float)width, maxPreviewHeight / (float)height);
            width = (int)(width * scaling);
            height = (int)(height * scaling);
        }

        mRecordWidth = width;
        mRecordHeight = height;
        cameraInstance().setPreferPreviewSize(width, height);
    }

    public synchronized void switchCamera() {
        mIsCameraBackForward = !mIsCameraBackForward;

        if(mFrameRecorder != null) {
            queueEvent(new Runnable() {
                @Override
                public void run() {

                    if(mFrameRecorder == null) {
                        Log.e(LOG_TAG, "Error: switchCamera after release!!");
                        return;
                    }

                    cameraInstance().stopCamera();

                    int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

                    mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
                    mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);

                    if (mIsUsingMask) {
                        mFrameRecorder.setMaskTextureRatio(mMaskAspectRatio);
                    }

                    cameraInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                        @Override
                        public void cameraReady() {
                            if (!cameraInstance().isPreviewing()) {
                                Log.i(LOG_TAG, "## switch camera -- start preview...");
                                cameraInstance().startPreview(mSurfaceTexture);
                                mFrameRecorder.srcResize(cameraInstance().previewHeight(), cameraInstance().previewWidth());
                            }
                        }
                    }, facing);

                    requestRender();
                }
            });
        }
    }

    //注意， focusAtPoint 会强制 focus mode 为 FOCUS_MODE_AUTO
    //如果有自定义的focus mode， 请在 AutoFocusCallback 里面重设成所需的focus mode。
    //x,y 取值范围: [0, 1]， 一般为 touchEventPosition / viewSize.
    public void focusAtPoint(float x, float y, Camera.AutoFocusCallback focusCallback) {
        cameraInstance().focusAtPoint(y, 1.0f - x, focusCallback);
    }

    // 参数为
    //    Camera.Parameters.FLASH_MODE_AUTO;
    //    Camera.Parameters.FLASH_MODE_OFF;
    //    Camera.Parameters.FLASH_MODE_ON;
    //    Camera.Parameters.FLASH_MODE_RED_EYE
    //    Camera.Parameters.FLASH_MODE_TORCH 等
    public synchronized boolean setFlashLightMode(String mode) {

        if(!getContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH)) {
            Log.e(LOG_TAG, "当前设备不支持闪光灯!");
            return false;
        }

        if(!mIsCameraBackForward) {
            return false;
        }

        Camera.Parameters parameters = cameraInstance().getParams();

        if(parameters == null)
            return false;

        try {

            if(!parameters.getSupportedFlashModes().contains(mode)) {
                Log.e(LOG_TAG, "Invalid Flash Light Mode!!!");
                return false;
            }

            parameters.setFlashMode(mode);
            cameraInstance().setParams(parameters);
        } catch (Exception e) {
            Log.e(LOG_TAG, "修改闪光灯状态失败, 请检查是否正在使用前置摄像头?");
            return false;
        }

        return true;
    }

    public synchronized void setFilterWithConfig(final String config) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                if(mFrameRecorder != null) {
                    mFrameRecorder.setFilterWidthConfig(config);
                } else {
                    Log.e(LOG_TAG, "setFilterWithConfig after release!!");
                }
            }
        });
    }

    public void setFilterIntensity(final float intensity) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mFrameRecorder != null) {
                    mFrameRecorder.setFilterIntensity(intensity);
                } else {
                    Log.e(LOG_TAG, "setFilterIntensity after release!!");
                }
            }
        });
    }

    public interface SetMaskBitmapCallback {
        void setMaskOK(CGEFrameRecorder recorder);
    }

    public void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle) {
        setMaskBitmap(bmp, shouldRecycle, null);
    }

    //注意， 当传入的bmp为null时， SetMaskBitmapCallback 不会执行.
    public void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle, final SetMaskBitmapCallback callback) {

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if(mFrameRecorder == null) {
                    Log.e(LOG_TAG, "setMaskBitmap after release!!");
                    return;
                }

                if (bmp == null) {
                    mFrameRecorder.setMaskTexture(0, 1.0f);
                    mIsUsingMask = false;
                    calcViewport();
                    return;
                }

                int texID = Common.genNormalTextureID(bmp, GLES20.GL_NEAREST, GLES20.GL_CLAMP_TO_EDGE);

                mFrameRecorder.setMaskTexture(texID, bmp.getWidth() / (float) bmp.getHeight());
                mIsUsingMask = true;
                mMaskAspectRatio = bmp.getWidth() / (float) bmp.getHeight();

                if (callback != null) {
                    callback.setMaskOK(mFrameRecorder);
                }

                if (shouldRecycle)
                    bmp.recycle();

                calcViewport();
            }
        });
    }

    public interface SetBackgroundImageCallback {
        void setBackgroundImageOK();
    }

    TextureRendererDrawOrigin mBackgroundRenderer;
    int mBackgroundTexture;

    public void setBackgroundImage(final Bitmap bmp, final boolean shouldRecycle, final SetBackgroundImageCallback callback) {

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mFrameRecorder == null) {
                    Log.e(LOG_TAG, "setBackgroundImage after reelase!!");
                    return;
                }

                if (bmp == null) {
                    if (mBackgroundRenderer != null)
                        mBackgroundRenderer.release();
                    mBackgroundRenderer = null;
                    if (mBackgroundTexture != 0)
                        GLES20.glDeleteTextures(1, new int[]{mBackgroundTexture}, 0);
                    mBackgroundTexture = 0;
                    if (callback != null)
                        callback.setBackgroundImageOK();
                    return;
                }

                if (mBackgroundTexture != 0) {
                    GLES20.glDeleteTextures(1, new int[]{mBackgroundTexture}, 0);
                }

                mBackgroundTexture = Common.genNormalTextureID(bmp, GLES20.GL_NEAREST, GLES20.GL_CLAMP_TO_EDGE);

                if (mBackgroundRenderer == null) {
                    mBackgroundRenderer = TextureRendererDrawOrigin.create(false);
                    if(mBackgroundRenderer != null)
                        mBackgroundRenderer.setFlipscale(1.0f, -1.0f);
                }

                if (shouldRecycle)
                    bmp.recycle();

                if (callback != null)
                    callback.setBackgroundImageOK();
            }
        });
    }

    public interface OnCreateCallback {
        void createOver(boolean success);
    }

    protected OnCreateCallback mOnCreateCallback;

    //定制一些初始化操作
    public void setOnCreateCallback(final OnCreateCallback callback) {

        assert callback != null : "无意义操作!";

        if(mFrameRecorder == null) {
            mOnCreateCallback = callback;
        }
        else {
            // 已经创建完毕， 直接执行
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    callback.createOver(cameraInstance().getCameraDevice() != null);
                }
            });
        }
    }

    public CameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.i(LOG_TAG, "MyGLSurfaceView Construct...");

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        setZOrderOnTop(true);
//        setZOrderMediaOverlay(true);

        clearColor = new ClearColor();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        int texSize[] = new int[1];

        GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, texSize, 0);
        maxTextureSize = texSize[0];

        mTextureID = Common.genSurfaceTextureID();
        mSurfaceTexture = new SurfaceTexture(mTextureID);
        mSurfaceTexture.setOnFrameAvailableListener(this);

        mFrameRecorder = new CGEFrameRecorder();
        mIsTransformMatrixSet = false;
        if(!mFrameRecorder.init(mRecordWidth, mRecordHeight, mRecordWidth, mRecordHeight)) {
            Log.e(LOG_TAG, "Frame Recorder init failed!");
        }

        mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
        mFrameRecorder.setSrcFlipScale(1.0f, -1.0f);
        mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);

        requestRender();

        if(!cameraInstance().isCameraOpened()) {

            int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

            if(!cameraInstance().tryOpenCamera(null, facing)) {
                Log.e(LOG_TAG, "相机启动失败!!");
            }
        }

        if(mOnCreateCallback != null) {
            mOnCreateCallback.createOver(cameraInstance().getCameraDevice() != null);
        }
    }

    protected void calcViewport() {

        float scaling;

        if(mIsUsingMask) {
            scaling = mMaskAspectRatio;
        } else {
            scaling = mRecordWidth / (float)mRecordHeight;
        }

        float viewRatio = viewWidth / (float)viewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if(mFitFullView) {
            //撑满全部view(内容大于view)
            if(s > 1.0) {
                w = (int)(viewHeight * scaling);
                h = viewHeight;
            } else {
                w = viewWidth;
                h = (int)(viewWidth / scaling);
            }
        } else {
            //显示全部内容(内容小于view)
            if(s > 1.0) {
                w = viewWidth;
                h = (int)(viewWidth / scaling);
            } else {
                h = viewHeight;
                w = (int)(viewHeight * scaling);
            }
        }

        mDrawViewport.width = w;
        mDrawViewport.height = h;
        mDrawViewport.x = (viewWidth - mDrawViewport.width) / 2;
        mDrawViewport.y = (viewHeight - mDrawViewport.height) / 2;
        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height));
    }

    public interface ReleaseOKCallback {

        void releaseOK();
    }

    public synchronized void release(final ReleaseOKCallback callback) {

        if(mFrameRecorder != null) {

            queueEvent(new Runnable() {
                @Override
                public void run() {

                    if (mFrameRecorder != null) {
                        mFrameRecorder.release();
                        mFrameRecorder = null;
                        GLES20.glDeleteTextures(1, new int[]{mTextureID}, 0);
                        mTextureID = 0;
                        mSurfaceTexture.release();
                        mSurfaceTexture = null;
                        if (mBackgroundRenderer != null) {
                            mBackgroundRenderer.release();
                            mBackgroundRenderer = null;
                        }
                        if (mBackgroundTexture != 0) {
                            GLES20.glDeleteTextures(1, new int[]{mBackgroundTexture}, 0);
                            mBackgroundTexture = 0;
                        }

                        Log.i(LOG_TAG, "GLSurfaceview release...");
                        if (callback != null)
                            callback.releaseOK();
                    }

                }
            });
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(LOG_TAG, String.format("onSurfaceChanged: %d x %d", width, height));

        GLES20.glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        viewWidth = width;
        viewHeight = height;

        calcViewport();

        if(!cameraInstance().isPreviewing()) {
            cameraInstance().startPreview(mSurfaceTexture);
            mFrameRecorder.srcResize(cameraInstance().previewHeight(), cameraInstance().previewWidth());
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        cameraInstance().stopCamera();
    }

    public void stopPreview() {

        queueEvent(new Runnable() {
            @Override
            public void run() {
                cameraInstance().stopPreview();
            }
        });
    }

    public synchronized void resumePreview() {

        if(mFrameRecorder == null) {
            Log.e(LOG_TAG, "resumePreview after release!!");
            return;
        }

        if(!cameraInstance().isCameraOpened()) {

            int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

            cameraInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                @Override
                public void cameraReady() {
                    Log.i(LOG_TAG, "tryOpenCamera OK...");
                }
            }, facing);
        }

        if(!cameraInstance().isPreviewing()) {
            cameraInstance().startPreview(mSurfaceTexture);
            mFrameRecorder.srcResize(cameraInstance().previewHeight(), cameraInstance().previewWidth());
        }

        requestRender();
    }

    private float[] _transformMatrix = new float[16];

    @Override
    public void onDrawFrame(GL10 gl) {

        if(mSurfaceTexture == null || !cameraInstance().isPreviewing()) {
            //防止双缓冲情况下最后几帧抖动
            if(mFrameRecorder != null) {
                GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
                mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
            }

            return;
        }

        mSurfaceTexture.updateTexImage();

        //TransformMatrix 只需要设置一次
        mSurfaceTexture.getTransformMatrix(_transformMatrix);
        mFrameRecorder.update(mTextureID, _transformMatrix);

        mFrameRecorder.runProc();

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        if(mTakeThunbnailCallback != null && !mTakeThunbnailCallback.isUsingBitmap()) {

            synchronized (mThunbnailLock) {

                // double judgement for mTakeThunbnailCallback ensure both performance and safety
                if(mTakeThunbnailCallback != null) {

                    if(mThumnailClipingArea != null) {
                        int clipW = (int)(mThunbnailWidth / mThumnailClipingArea.width());
                        int clipH = (int)(mThunbnailHeight / mThumnailClipingArea.height());
                        int x = -(int)(clipW * mThumnailClipingArea.left);
                        int y = -(int)(clipH * mThumnailClipingArea.top);

                        GLES20.glViewport(x, y, clipW, clipH);

                    } else {
                        GLES20.glViewport(0, 0, mThunbnailWidth, mThunbnailHeight);
                    }

                    mFrameRecorder.drawCache();

                    mThunbnailBuffer.position(0);
                    GLES20.glReadPixels(0, 0, mThunbnailWidth, mThunbnailHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mThunbnailBuffer);

                    mThunbnailBmp.copyPixelsFromBuffer(mThunbnailBuffer);

                    post(new Runnable() {
                        @Override
                        public void run() {
                            synchronized (mThunbnailLock) {
                                if(mTakeThunbnailCallback != null)
                                    mTakeThunbnailCallback.takeThunbnailOK(mThunbnailBmp);
                            }
                        }
                    });
                }
            }
        }

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        if(mBackgroundRenderer != null) {
            GLES20.glViewport(0, 0, viewWidth, viewHeight);
            mBackgroundRenderer.renderTexture(mBackgroundTexture, null);
        }
        GLES20.glEnable(GLES20.GL_BLEND);
        mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
        GLES20.glDisable(GLES20.GL_BLEND);
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(LOG_TAG, "glsurfaceview onResume...");
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "glsurfaceview onPause in...");

        cameraInstance().stopCamera();
        super.onPause();
        Log.i(LOG_TAG, "glsurfaceview onPause out...");
    }

    protected long mTimeCount2 = 0;
    protected long mFramesCount2 = 0;
    protected long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

        requestRender();

        if(mLastTimestamp2 == 0)
            mLastTimestamp2 = System.currentTimeMillis();

        long currentTimestamp = System.currentTimeMillis();

        ++mFramesCount2;
        mTimeCount2 += currentTimestamp - mLastTimestamp2;
        mLastTimestamp2 = currentTimestamp;
        if(mTimeCount2 >= 1000) {
            Log.i(LOG_TAG, String.format("相机每秒采样率: %d", mFramesCount2));
            mTimeCount2 %= 1000;
            mFramesCount2 = 0;
        }
    }

    public interface TakeThunbnailCallback {
        // 当 TakeThunbnailCallback 被设置之后
        // 每一帧都会获取 isUsingBitmap() 返回值
        // 当 isUsingBitmap() 返回 true 的时候 takeThunbnailOK 将不被调用
        // 当 isUsingBitmap() 返回 false 的时候 takeThunbnailOK 正常执行
        boolean isUsingBitmap();
        void takeThunbnailOK(Bitmap bmp);
    }

    protected Bitmap mThunbnailBmp;
    protected TakeThunbnailCallback mTakeThunbnailCallback;
    protected final Object mThunbnailLock = new Object();
    protected int mThunbnailWidth, mThunbnailHeight;
    RectF mThumnailClipingArea;
    protected IntBuffer mThunbnailBuffer;

    public boolean isTakingThunbnail() {
        boolean status;
        synchronized (mThunbnailLock) {
            status = mTakeThunbnailCallback != null;
        }
        return status;
    }

    public void startThunbnailCliping(Bitmap cache, RectF clipingArea, TakeThunbnailCallback callback) {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = callback;
            mThunbnailBmp = cache;
            mThunbnailWidth = cache.getWidth();
            mThunbnailHeight = cache.getHeight();
            mThunbnailBuffer = IntBuffer.allocate(mThunbnailWidth * mThunbnailHeight);
            mThumnailClipingArea = clipingArea;
        }
    }

    public void startThunbnailCliping(int width, int height, RectF clipingArea, TakeThunbnailCallback callback) {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = callback;
            mThunbnailBmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            mThunbnailWidth = width;
            mThunbnailHeight = height;
            mThunbnailBuffer = IntBuffer.allocate(width * height);
            mThumnailClipingArea = clipingArea;
        }
    }

    public void stopThunbnailCliping() {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = null;
            if(mThunbnailBmp != null && !mThunbnailBmp.isRecycled())
            	mThunbnailBmp.recycle();
            mThunbnailBmp = null;
            mThunbnailBuffer = null;
            mThumnailClipingArea = null;
        }
    }

    public interface TakePictureCallback {
        //传入的bmp可以由接收者recycle
        void takePictureOK(Bitmap bmp);
    }

    public void takeShot(final TakePictureCallback callback) {
        takeShot(callback, true);
    }

    public synchronized void takeShot(final TakePictureCallback callback, final boolean noMask) {
        assert callback != null : "callback must not be null!";

        if(mFrameRecorder == null) {
            Log.e(LOG_TAG, "Recorder not initialized!");
            callback.takePictureOK(null);
            return;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                FrameBufferObject frameBufferObject = new FrameBufferObject();
                int bufferTexID;
                IntBuffer buffer;
                Bitmap bmp;

                if (noMask || !mIsUsingMask) {

                    bufferTexID = Common.genBlankTextureID(mRecordWidth, mRecordHeight);
                    frameBufferObject.bindTexture(bufferTexID);
                    GLES20.glViewport(0, 0, mRecordWidth, mRecordHeight);
                    mFrameRecorder.drawCache();
                    buffer = IntBuffer.allocate(mRecordWidth * mRecordHeight);
                    GLES20.glReadPixels(0, 0, mRecordWidth, mRecordHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
                    bmp = Bitmap.createBitmap(mRecordWidth, mRecordHeight, Bitmap.Config.ARGB_8888);
                    bmp.copyPixelsFromBuffer(buffer);
                    Log.i(LOG_TAG, String.format("w: %d, h: %d", mRecordWidth, mRecordHeight));

                } else {

                    bufferTexID = Common.genBlankTextureID(mDrawViewport.width, mDrawViewport.height);
                    frameBufferObject.bindTexture(bufferTexID);

                    int w = Math.min(mDrawViewport.width, viewWidth);
                    int h = Math.min(mDrawViewport.height, viewHeight);

                    mFrameRecorder.setRenderFlipScale(1.0f, 1.0f);
                    mFrameRecorder.setMaskTextureRatio(mMaskAspectRatio);
                    mFrameRecorder.render(0, 0, w, h);
                    mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
                    mFrameRecorder.setMaskTextureRatio(mMaskAspectRatio);

                    Log.i(LOG_TAG, String.format("w: %d, h: %d", w, h));
                    buffer = IntBuffer.allocate(w * h);
                    GLES20.glReadPixels(0, 0, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
                    bmp = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
                    bmp.copyPixelsFromBuffer(buffer);
                }

                frameBufferObject.release();
                GLES20.glDeleteTextures(1, new int[]{bufferTexID}, 0);

                callback.takePictureOK(bmp);
            }
        });

    }

    //isBigger 为true 表示当宽高不满足时，取最近的较大值.
    // 若为 false 则取较小的
    public void setPictureSize(int width, int height, boolean isBigger) {
        //默认会旋转90度.
        cameraInstance().setPictureSize(height, width, isBigger);
    }

    public synchronized void takePicture(final TakePictureCallback photoCallback, Camera.ShutterCallback shutterCallback, final String config, final float intensity, final boolean isFrontMirror) {

        Camera.Parameters params = cameraInstance().getParams();

        if(photoCallback == null || params == null) {
            Log.e(LOG_TAG, "takePicture after release!");
            if(photoCallback != null) {
                photoCallback.takePictureOK(null);
            }
            return;
        }

        try {
            params.setRotation(90);
            cameraInstance().setParams(params);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Error when takePicture: " + e.toString());
            if(photoCallback != null) {
                photoCallback.takePictureOK(null);
            }
            return;
        }

        cameraInstance().getCameraDevice().takePicture(shutterCallback, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(final byte[] data, Camera camera) {

                Camera.Parameters params = camera.getParameters();
                Camera.Size sz = params.getPictureSize();

                boolean shouldRotate;

                Bitmap bmp;
                int width, height;

                //当拍出相片不为正方形时， 可以判断图片是否旋转
                if(sz.width != sz.height) {
                    //默认数据格式已经设置为 JPEG
                    bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                    width = bmp.getWidth();
                    height = bmp.getHeight();
                    shouldRotate = (sz.width > sz.height && width > height) || (sz.width < sz.height && width < height);
                } else {
                    Log.i(LOG_TAG, "Cache image to get exif.");

                    try {
                        String tmpFilename = getContext().getExternalCacheDir() + "/picture_cache000.jpg";
                        FileOutputStream fileout = new FileOutputStream(tmpFilename);
                        BufferedOutputStream bufferOutStream = new BufferedOutputStream(fileout);
                        bufferOutStream.write(data);
                        bufferOutStream.flush();
                        bufferOutStream.close();

                        ExifInterface exifInterface = new ExifInterface(tmpFilename);
                        int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);

                        switch (orientation) {
                            //被保存图片exif记录只有旋转90度， 和不旋转两种情况
                            case ExifInterface.ORIENTATION_ROTATE_90:
                                shouldRotate = true;
                                break;
                            default:
                                shouldRotate = false;
                                break;
                        }

                        bmp = BitmapFactory.decodeFile(tmpFilename);
                        width = bmp.getWidth();
                        height = bmp.getHeight();

                    } catch (IOException e) {
                        Log.e(LOG_TAG, "Err when saving bitmap...");
                        e.printStackTrace();
                        return;
                    }
                }


                if(width > maxTextureSize || height > maxTextureSize) {
                    float scaling = Math.max(width / (float) maxTextureSize, height / (float) maxTextureSize);
                    Log.i(LOG_TAG, String.format("目标尺寸(%d x %d)超过当前设备OpenGL 能够处理的最大范围(%d x %d)， 现在将图片压缩至合理大小!", width, height, maxTextureSize, maxTextureSize));

                    bmp = Bitmap.createScaledBitmap(bmp, (int)(width / scaling), (int)(height / scaling), false);

                    width = bmp.getWidth();
                    height = bmp.getHeight();
                }

                Bitmap bmp2;

                if(shouldRotate)
                {
                    bmp2 = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);

                    Canvas canvas = new Canvas(bmp2);

                    if (cameraInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_BACK) {
                        Matrix mat = new Matrix();
                        int halfLen = Math.min(width, height) / 2;
                        mat.setRotate(90, halfLen, halfLen);
                        canvas.drawBitmap(bmp, mat, null);
                    } else {
                        Matrix mat = new Matrix();

                        if (isFrontMirror) {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(-1.0f, 1.0f);
                            mat.postTranslate(width / 2, height / 2);
                            int halfLen = Math.min(width, height) / 2;
                            mat.postRotate(90, halfLen, halfLen);
                        } else {
                            int halfLen = Math.max(width, height) / 2;
                            mat.postRotate(-90, halfLen, halfLen);
                        }

                        canvas.drawBitmap(bmp, mat, null);
                    }

                    bmp.recycle();
                } else {
                    if(cameraInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_BACK) {
                        bmp2 = bmp;
                    } else {

                        bmp2 = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                        Canvas canvas = new Canvas(bmp2);
                        Matrix mat = new Matrix();
                        if (isFrontMirror) {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(1.0f, -1.0f);
                            mat.postTranslate(width / 2, height / 2);
                        } else {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(-1.0f, -1.0f);
                            mat.postTranslate(width / 2, height / 2);
                        }

                        canvas.drawBitmap(bmp, mat, null);
                    }

                }

                if (config != null) {
                    CGENativeLibrary.filterImage_MultipleEffectsWriteBack(bmp2, config, intensity);
                }

                photoCallback.takePictureOK(bmp2);

                cameraInstance().getCameraDevice().startPreview();
            }
        });
    }
}
