package org.wysaid.view;

/**
 * Created by wangyang on 15/7/27.
 */


import android.content.Context;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.camera.CameraInstance;
import org.wysaid.myUtils.Common;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.nativePort.CGENativeLibrary;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */
public class FilterGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final String LOG_TAG = Common.LOG_TAG;

    public int viewWidth;
    public int viewHeight;

    private SurfaceTexture mSurfaceTexture;
    private int mTextureID;

    private CGEFrameRecorder mFrameRecorder;

    public class ClearColor {
        public float r, g, b, a;
    }

    public ClearColor clearColor;

    class DrawViewport {
        int x, y, width, height;
    }

    private DrawViewport mDrawViewport;

    private boolean mIsUsingMask = false;
    private float mMaskAspectRatio = 1.0f;
    private float[] mTransformMatrix = new float[16];

    private long mTimeCount = 0;
    private long mFramesCount = 0;
    private long mLastTimestamp = 0;

    private int mPreviewTextureWidth = 640;
    private int mPreviewTextureHeight = 640;

    private boolean mIsCameraBack = true;

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

    public synchronized void switchCamera() {
        mIsCameraBack = !mIsCameraBack;

        if(mFrameRecorder != null) {
            queueEvent(new Runnable() {
                @Override
                public void run() {

                    cameraInstance().stopCamera();

                    int facing = mIsCameraBack ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;
//                    if (mIsCameraBack) {
//                        mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
//                        mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
//                    } else {
                        mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
                        mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
//                    }

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
                }
            });
        }
    }

    public synchronized void setFilterWithConfig(final String config) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                mFrameRecorder.setFilterWidthConfig(config);
            }
        });
    }

    public synchronized void setFilterIntensity(final float intensity) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                mFrameRecorder.setFilterIntensity(intensity);
            }
        });
    }

    public synchronized void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                if(bmp == null) {
                    mFrameRecorder.setMaskTexture(0, 1.0f);
                    mIsUsingMask = false;
                    calcViewport();
                    return ;
                }

                int texID[] = {0};
                GLES20.glGenTextures(1, texID, 0);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texID[0]);
                GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bmp, 0);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

                mFrameRecorder.setMaskTexture(texID[0], bmp.getWidth() / (float) bmp.getHeight());
                mIsUsingMask = true;
                mMaskAspectRatio = bmp.getWidth() / (float)bmp.getHeight();

                if(shouldRecycle)
                    bmp.recycle();
                calcViewport();
            }
        });
    }

    public FilterGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.i(LOG_TAG, "MyGLSurfaceView Construct...");

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
//        setRenderMode(RENDERMODE_CONTINUOUSLY);
//        setZOrderOnTop(true);
//        setZOrderMediaOverlay(true);

        clearColor = new ClearColor();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);
//        GLES20.glBlendFunc(GLES20.GL_ONE, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        mTextureID = genSurfaceTextureID();
        mSurfaceTexture = new SurfaceTexture(mTextureID);
        mSurfaceTexture.setOnFrameAvailableListener(this);

        requestRender();

        //打开相机并不是异步操作
        if(!cameraInstance().isCameraOpened()) {

            cameraInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                @Override
                public void cameraReady() {
                    Log.i(LOG_TAG, "tryOpenCamera OK...");
                }
            });
        }

        mFrameRecorder = new CGEFrameRecorder();
        mPreviewTextureWidth = cameraInstance().previewHeight();
        mPreviewTextureHeight = cameraInstance().previewWidth();
        if(!mFrameRecorder.init(mPreviewTextureWidth, mPreviewTextureHeight, mPreviewTextureWidth, mPreviewTextureHeight)) {
            Log.e(LOG_TAG, "Frame Recorder init failed!");
        }

        mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
        mFrameRecorder.setSrcFlipScale(1.0f, -1.0f);
        mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
    }

    private void calcViewport() {
        float scaling;

        if(mIsUsingMask) {
            scaling = mMaskAspectRatio;
        } else {
            scaling = mPreviewTextureWidth / (float)mPreviewTextureHeight;
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

        mDrawViewport = new DrawViewport();

        mDrawViewport.width = w;
        mDrawViewport.height = h;
        mDrawViewport.x = (viewWidth - mDrawViewport.width) / 2;
        mDrawViewport.y = (viewHeight - mDrawViewport.height) / 2;
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
            mPreviewTextureWidth = cameraInstance().previewHeight();
            mPreviewTextureHeight = cameraInstance().previewWidth();
            mFrameRecorder.srcResize(mPreviewTextureWidth, mPreviewTextureHeight);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        cameraInstance().stopCamera();
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if(mSurfaceTexture == null || !cameraInstance().isPreviewing())
            return;

        mSurfaceTexture.getTransformMatrix(mTransformMatrix);

        mFrameRecorder.update(mTextureID, mTransformMatrix);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);

        mSurfaceTexture.updateTexImage();

//        Log.i(LOG_TAG, "onDrawFrame...");
        if(mLastTimestamp == 0)
            mLastTimestamp = System.currentTimeMillis();

        long currentTimestamp = System.currentTimeMillis();

        ++mFramesCount;
        mTimeCount += currentTimestamp - mLastTimestamp;
        mLastTimestamp = currentTimestamp;
        if(mTimeCount >= 1e3)
        {
            Log.i(LOG_TAG, String.format("TimeCount: %d, Fps: %d", mTimeCount, mFramesCount));
            mTimeCount -= 1e3;
            mFramesCount = 0;
        }
    }

    public interface TakePictureCallback {
        void takePictureOK(Bitmap bmp);
    }

    public synchronized void takePicture(final TakePictureCallback photoCallback, Camera.ShutterCallback shutterCallback, final String config, final float intensity) {

        assert photoCallback != null : "photoCallback must not be null!!";

        //TODO 需要对超过最大纹理单元尺寸的图片进行优化（缩小）
        final int width = cameraInstance().pictureWidth();
        final int height = cameraInstance().pictureHeight();

        cameraInstance().getCameraDevice().takePicture(shutterCallback, null,
                new Camera.PictureCallback() {

                    @Override
                    public void onPictureTaken(final byte[] data, Camera camera) {

                        //默认数据格式已经设置为 JPEG
                        Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                        Bitmap bmp2 = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);

                        Canvas canvas = new Canvas(bmp2);

                        if(cameraInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_BACK) {
                            Matrix mat = new Matrix();
                            int halfLen = Math.min(width, height) / 2;
                            mat.setRotate(90, halfLen, halfLen);
                            canvas.drawBitmap(bmp, mat, null);
                        } else {
                            Matrix mat = new Matrix();
                            int halfLen = Math.max(width, height) / 2;
                            mat.setRotate(-90, halfLen, halfLen);
                            canvas.drawBitmap(bmp, mat, null);
                        }

                        bmp.recycle();

                        if(config != null) {
                            CGENativeLibrary.filterImage_MultipleEffectsWriteBack(bmp2, config, intensity);
                        }

                        photoCallback.takePictureOK(bmp2);

                        cameraInstance().getCameraDevice().startPreview();
                    }
                });
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(LOG_TAG, "onResume...");
        cameraInstance().tryOpenCamera(null);
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "surfaceview onPause in...");
        cameraInstance().stopCamera();
        super.onPause();
        Log.i(LOG_TAG, "surfaceview onPause out...");
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
//        Log.i(LOG_TAG, "onFrameAvailable...");

        requestRender();
    }

    private int genSurfaceTextureID() {
        int[] texID = new int[1];
        GLES20.glGenTextures(1, texID, 0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texID[0]);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        return texID[0];
    }
}
