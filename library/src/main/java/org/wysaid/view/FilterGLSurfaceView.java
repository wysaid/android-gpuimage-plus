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
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.camera.CameraInstance;
import org.wysaid.myUtils.Common;
import org.wysaid.myUtils.FrameBufferObject;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.texUtils.TextureRendererDrawOrigin;

import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */
public class FilterGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final String LOG_TAG = Common.LOG_TAG;

    public int maxTextureSize = 0;

    public int viewWidth;
    public int viewHeight;

    private int mRecordWidth = 480;
    private int mRecordHeight = 640;

    private SurfaceTexture mSurfaceTexture;
    private int mTextureID;

    private CGEFrameRecorder mFrameRecorder;

    public CGEFrameRecorder getRecorder() {
        return mFrameRecorder;
    }
    private Context mContext;

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

    private TextureRenderer.Viewport mDrawViewport;

    private boolean mIsUsingMask = false;

    public boolean isUsingMask() {
        return mIsUsingMask;
    }

    private float mMaskAspectRatio = 1.0f;
    private float[] mTransformMatrix = new float[16];

    //是否使用后置摄像头
    private boolean mIsCameraBackForward = true;

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

    private CameraInstance cameraInstance() {
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

    // 参数为
    //    Camera.Parameters.FLASH_MODE_AUTO;
    //    Camera.Parameters.FLASH_MODE_OFF;
    //    Camera.Parameters.FLASH_MODE_ON;
    //    Camera.Parameters.FLASH_MODE_RED_EYE
    //    Camera.Parameters.FLASH_MODE_TORCH 等
    public synchronized boolean setFlashLightMode(String mode) {

        if(!mContext.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH)) {
            Log.e(LOG_TAG, "当前设备不支持闪光灯!");
            return false;
        }

        if(!mIsCameraBackForward) {
            return false;
        }

        Camera.Parameters parameters = cameraInstance().getParams();

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
                assert mFrameRecorder != null && config != null : "Recorder & Config must not be null!";
                mFrameRecorder.setFilterWidthConfig(config);
            }
        });
    }

    public void setFilterIntensity(final float intensity) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                assert mFrameRecorder != null : "Recorder must not be null!";
                mFrameRecorder.setFilterIntensity(intensity);
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
                if(bmp == null) {
                    if(mBackgroundRenderer != null)
                        mBackgroundRenderer.release();
                    mBackgroundRenderer = null;
                    if(mBackgroundTexture != 0)
                        GLES20.glDeleteTextures(1, new int[] {mBackgroundTexture}, 0);
                    mBackgroundTexture = 0;
                    if(callback != null)
                        callback.setBackgroundImageOK();
                    return;
                }

                if(mBackgroundTexture != 0) {
                    GLES20.glDeleteTextures(1, new int[]{mBackgroundTexture}, 0);
                }

                mBackgroundTexture = Common.genNormalTextureID(bmp, GLES20.GL_NEAREST, GLES20.GL_CLAMP_TO_EDGE);

                if(mBackgroundRenderer == null) {
                    mBackgroundRenderer = TextureRendererDrawOrigin.create(false);
                    mBackgroundRenderer.setFlipscale(1.0f, -1.0f);
                }

                if(shouldRecycle)
                    bmp.recycle();

                if(callback != null)
                    callback.setBackgroundImageOK();
            }
        });
    }

    public interface OnCreateCallback {
        void createOK();
    }

    private OnCreateCallback mOnCreateCallback;

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
                    callback.createOK();
                }
            });
        }
    }

    public FilterGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.i(LOG_TAG, "MyGLSurfaceView Construct...");

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
//        setRenderMode(RENDERMODE_WHEN_DIRTY);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
        setZOrderOnTop(true);
//        setZOrderMediaOverlay(true);

        clearColor = new ClearColor();
        mContext = context;
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
        if(!mFrameRecorder.init(mRecordWidth, mRecordHeight, mRecordWidth, mRecordHeight)) {
            Log.e(LOG_TAG, "Frame Recorder init failed!");
        }

        mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
        mFrameRecorder.setSrcFlipScale(1.0f, -1.0f);
        mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);

        requestRender();

        if(!cameraInstance().isCameraOpened()) {

            int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

            cameraInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                @Override
                public void cameraReady() {
                    Log.i(LOG_TAG, "tryOpenCamera OK...");
                }
            }, facing);
        }

        if(mOnCreateCallback != null) {
            mOnCreateCallback.createOK();
        }
    }

    private void calcViewport() {
        float scaling;

        if(mIsUsingMask) {
            scaling = mMaskAspectRatio;
        } else {
            scaling = mRecordWidth / (float)mRecordHeight;
        }

        float viewRatio = viewWidth / (float)viewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if(s > 1.0) {
            w = viewWidth;
            h = (int)(viewWidth / scaling);
        } else {
            h = viewHeight;
            w = (int)(viewHeight * scaling);
        }

        mDrawViewport = new TextureRenderer.Viewport();

        mDrawViewport.width = w;
        mDrawViewport.height = h;
        mDrawViewport.x = (viewWidth - mDrawViewport.width) / 2;
        mDrawViewport.y = (viewHeight - mDrawViewport.height) / 2;
        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height));
    }

    public interface ReleaseOKCallback {

        void releaseOK();
    }

    public void release(final ReleaseOKCallback callback) {
        synchronized (this) {
            if(mFrameRecorder != null)
            {
                queueEvent(new Runnable() {
                    @Override
                    public void run() {

                        mFrameRecorder.release();
                        mFrameRecorder = null;
                        GLES20.glDeleteTextures(1, new int[]{mTextureID}, 0);
                        mTextureID = 0;
                        mSurfaceTexture.release();
                        mSurfaceTexture = null;
                        if(mBackgroundRenderer != null) {
                            mBackgroundRenderer.release();
                            mBackgroundRenderer = null;
                        }
                        if(mBackgroundTexture != 0) {
                            GLES20.glDeleteTextures(1, new int[]{mBackgroundTexture}, 0);
                            mBackgroundTexture = 0;
                        }

                        Log.i(LOG_TAG, "glsurfaceview release...");
                        if(callback != null)
                            callback.releaseOK();
                    }
                });
            }
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

    private long mTimeCount = 0;
    private long mFramesCount = 0;
    private long mLastTimestamp = 0;

    public void stopPreview() {

        queueEvent(new Runnable() {
            @Override
            public void run() {
                cameraInstance().stopPreview();
            }
        });
    }

    public synchronized void resumePreview() {

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

        mSurfaceTexture.getTransformMatrix(mTransformMatrix);
        mFrameRecorder.update(mTextureID, mTransformMatrix);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        synchronized (mThunbnailLock) {
            if(mTakeThunbnailCallback != null) {

                GLES20.glViewport(0, 0, mThunbnailWidth, mThunbnailHeight);
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

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        if(mBackgroundRenderer != null) {
            GLES20.glViewport(0, 0, viewWidth, viewHeight);
            mBackgroundRenderer.renderTexture(mBackgroundTexture, null);
        }
        GLES20.glEnable(GLES20.GL_BLEND);
        mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
        GLES20.glDisable(GLES20.GL_BLEND);

        if(mLastTimestamp == 0)
            mLastTimestamp = System.currentTimeMillis();

        long currentTimestamp = System.currentTimeMillis();

        ++mFramesCount;
        mTimeCount += currentTimestamp - mLastTimestamp;
        mLastTimestamp = currentTimestamp;
        if(mTimeCount >= 1000) {
            Log.i(LOG_TAG, String.format("每秒重绘帧率: %d", mFramesCount));
            mTimeCount %= 1000;
            mFramesCount = 0;
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(LOG_TAG, "onResume...");
//        int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;
//        cameraInstance().tryOpenCamera(null, facing);
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "surfaceview onPause in...");

        cameraInstance().stopCamera();
        super.onPause();
        Log.i(LOG_TAG, "surfaceview onPause out...");
    }

    private long mTimeCount2 = 0;
    private long mFramesCount2 = 0;
    private long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
//        Log.i(LOG_TAG, "onFrameAvailable...");

//        requestRender();

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
        void takeThunbnailOK(Bitmap bmp);
    }

    private Bitmap mThunbnailBmp;
    private TakeThunbnailCallback mTakeThunbnailCallback;
    private final int[] mThunbnailLock = new int[0];
    private int mThunbnailWidth, mThunbnailHeight;
    private IntBuffer mThunbnailBuffer;

    public boolean isTakingThunbnail() {
        boolean status;
        synchronized (mThunbnailLock) {
            status = mTakeThunbnailCallback != null;
        }
        return status;
    }

    public void startThunbnailCliping(Bitmap cache, TakeThunbnailCallback callback) {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = callback;
            mThunbnailBmp = cache;
            mThunbnailWidth = cache.getWidth();
            mThunbnailHeight = cache.getHeight();
            mThunbnailBuffer = IntBuffer.allocate(mThunbnailWidth * mThunbnailHeight);
        }
    }

    public void startThunbnailCliping(int width, int height, TakeThunbnailCallback callback) {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = callback;
            mThunbnailBmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            mThunbnailWidth = width;
            mThunbnailHeight = height;
            mThunbnailBuffer = IntBuffer.allocate(width * height);
        }
    }

    public void stopThunbnailCliping() {
        synchronized (mThunbnailLock) {
            mTakeThunbnailCallback = null;
            mThunbnailBmp.recycle();
            mThunbnailBmp = null;
            mThunbnailBuffer = null;
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

                if(noMask || !mIsUsingMask) {

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

        assert photoCallback != null : "photoCallback must not be null!!";

        cameraInstance().getCameraDevice().takePicture(shutterCallback, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(final byte[] data, Camera camera) {

                //默认数据格式已经设置为 JPEG
                Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);

                int width = bmp.getWidth(), height = bmp.getHeight();

                if(width > maxTextureSize || height > maxTextureSize) {
                    float scaling = Math.max(width / (float) maxTextureSize, height / (float) maxTextureSize);
                    Log.i(LOG_TAG, String.format("目标尺寸(%d x %d)超过当前设备OpenGL 能够处理的最大范围(%d x %d)， 现在将图片压缩至合理大小!", width, height, maxTextureSize, maxTextureSize));

                    bmp = Bitmap.createScaledBitmap(bmp, (int)(width / scaling), (int)(height / scaling), false);

                    width = bmp.getWidth();
                    height = bmp.getHeight();
                }

                Bitmap bmp2 = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);

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

                if (config != null) {
                    CGENativeLibrary.filterImage_MultipleEffectsWriteBack(bmp2, config, intensity);
                }

                photoCallback.takePictureOK(bmp2);

                cameraInstance().getCameraDevice().startPreview();
            }
        });
    }
}
