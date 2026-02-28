package org.wysaid.view;

/**
 * Camera GLSurfaceView with SurfaceTexture-based rendering.
 * Uses OES external texture + CGEFrameRecorder for GPU filter processing.
 * Supports both Camera1 and CameraX via {@link org.wysaid.camera.ICameraProvider}.
 */


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.ExifInterface;
import android.opengl.GLES20;
import android.util.AttributeSet;
import android.util.Log;

import org.wysaid.camera.Camera1Provider;
import org.wysaid.camera.ICameraProvider;
import org.wysaid.common.Common;
import org.wysaid.common.FrameBufferObject;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.nativePort.CGEFrameRenderer;
import org.wysaid.nativePort.CGENativeLibrary;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */
public class CameraGLSurfaceViewWithTexture extends CameraGLSurfaceView implements SurfaceTexture.OnFrameAvailableListener {

    protected SurfaceTexture mSurfaceTexture;
    protected int mTextureID;
    protected boolean mIsTransformMatrixSet = false;
    protected CGEFrameRecorder mFrameRecorder;

    public CGEFrameRecorder getRecorder() {
        return mFrameRecorder;
    }


    public synchronized void setFilterWithConfig(final String config) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (mFrameRecorder != null) {
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

    // Customize some initialization operations
    public void setOnCreateCallback(final OnCreateCallback callback) {
        if (mFrameRecorder == null || callback == null) {
            mOnCreateCallback = callback;
        } else {
            // Already created, just run.
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    callback.createOver();
                }
            });
        }
    }

    public CameraGLSurfaceViewWithTexture(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mFrameRecorder = new CGEFrameRecorder();
        mIsTransformMatrixSet = false;
        if (!mFrameRecorder.init(mRecordWidth, mRecordHeight, mRecordWidth, mRecordHeight)) {
            Log.e(LOG_TAG, "Frame Recorder init failed!");
        }

        if (getCameraProvider().needsManualRotation()) {
            mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
            mFrameRecorder.setSrcFlipScale(1.0f, -1.0f);
            mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
        } else {
            // CameraX already encodes rotation in the SurfaceTexture transform matrix.
            // No manual rotation needed; only keep the vertical flip that matches the
            // OpenGL / screen coordinate system.
            mFrameRecorder.setSrcRotation(0.0f);
            mFrameRecorder.setSrcFlipScale(1.0f, -1.0f);
            mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
        }

        mTextureID = Common.genSurfaceTextureID();
        mSurfaceTexture = new SurfaceTexture(mTextureID);
        mSurfaceTexture.setOnFrameAvailableListener(this);

        super.onSurfaceCreated(gl, config);
    }

    protected void onRelease() {
        super.onRelease();
        if(mSurfaceTexture != null) {
            mSurfaceTexture.release();
            mSurfaceTexture = null;
        }

        if(mTextureID != 0) {
            Common.deleteTextureID(mTextureID);
            mTextureID = 0;
        }

        if(mFrameRecorder != null) {
            mFrameRecorder.release();
            mFrameRecorder = null;
        }
    }


    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        super.onSurfaceChanged(gl, width, height);

        if (!getCameraProvider().isPreviewing()) {
            resumePreview();
        }
    }

    public void resumePreview() {

        if (mFrameRecorder == null) {
            Log.e(LOG_TAG, "resumePreview after release!!");
            return;
        }

        ICameraProvider provider = getCameraProvider();
        ICameraProvider.CameraFacing facing = mIsCameraBackForward
                ? ICameraProvider.CameraFacing.BACK
                : ICameraProvider.CameraFacing.FRONT;

        // Helper: start preview and handle async size notification.
        // Extracted as a Runnable so it can be called from the camera-ready callback.
        Runnable doStartPreview = () -> {
            if (provider.isPreviewing()) return;
            provider.startPreview(mSurfaceTexture, (pw, ph) -> {
                // Fired once the real preview resolution is known.
                // CameraX fires this asynchronously from the main thread;
                // Camera1 fires it synchronously — queue to GL thread either way.
                queueEvent(() -> {
                    if (mFrameRecorder != null && pw > 0 && ph > 0) {
                        mFrameRecorder.srcResize(ph, pw);
                        requestRender();
                    }
                });
            });
            // For Camera1 (synchronous) the sizes are already set; grab them immediately.
            int pw = provider.getPreviewWidth();
            int ph = provider.getPreviewHeight();
            if (pw > 0 && ph > 0) {
                mFrameRecorder.srcResize(ph, pw);
            }
        };

        if (!provider.isCameraOpened()) {
            provider.openCamera(facing, () -> {
                // Called once the camera backend is ready (async for CameraX).
                // Now it is safe to start the preview.
                Log.i(LOG_TAG, "openCamera OK, starting preview...");
                doStartPreview.run();
            });
        } else {
            // Camera already open (e.g. resume after background) — start preview directly.
            doStartPreview.run();
        }

        requestRender();
    }

    protected float[] mTransformMatrix = new float[16];

    @Override
    public void onDrawFrame(GL10 gl) {

        if (mSurfaceTexture == null || !getCameraProvider().isPreviewing()) {
            return;
        }

        mSurfaceTexture.updateTexImage();

        mSurfaceTexture.getTransformMatrix(mTransformMatrix);
        mFrameRecorder.update(mTextureID, mTransformMatrix);

        mFrameRecorder.runProc();

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClearColor(0,0,0,0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
    }

//    protected long mTimeCount2 = 0;
//    protected long mFramesCount2 = 0;
//    protected long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

        requestRender();

//        if (mLastTimestamp2 == 0)
//            mLastTimestamp2 = System.currentTimeMillis();
//
//        long currentTimestamp = System.currentTimeMillis();
//
//        ++mFramesCount2;
//        mTimeCount2 += currentTimestamp - mLastTimestamp2;
//        mLastTimestamp2 = currentTimestamp;
//        if (mTimeCount2 >= 1000) {
//            Log.i(LOG_TAG, String.format("camera sample rate: %d", mFramesCount2));
//            mTimeCount2 %= 1000;
//            mFramesCount2 = 0;
//        }
    }

    @Override
    protected void onSwitchCamera() {
        super.onSwitchCamera();
        if (mFrameRecorder != null) {
            // Only apply Camera1-style manual rotation when the provider requires it.
            // CameraX handles its own rotation internally; applying PI/2 there breaks orientation.
            if (getCameraProvider().needsManualRotation()) {
                mFrameRecorder.setSrcRotation((float) (Math.PI / 2.0));
            } else {
                mFrameRecorder.setSrcRotation(0.0f);
            }
            mFrameRecorder.setRenderFlipScale(1.0f, -1.0f);
        }
    }

    @Override
    public void takeShot(final TakePictureCallback callback) {
        assert callback != null : "callback must not be null!";

        if (mFrameRecorder == null) {
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

                bufferTexID = Common.genBlankTextureID(mRecordWidth, mRecordHeight);
                frameBufferObject.bindTexture(bufferTexID);
                GLES20.glViewport(0, 0, mRecordWidth, mRecordHeight);
                mFrameRecorder.drawCache();
                buffer = IntBuffer.allocate(mRecordWidth * mRecordHeight);
                GLES20.glReadPixels(0, 0, mRecordWidth, mRecordHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
                bmp = Bitmap.createBitmap(mRecordWidth, mRecordHeight, Bitmap.Config.ARGB_8888);
                bmp.copyPixelsFromBuffer(buffer);
                Log.i(LOG_TAG, String.format("w: %d, h: %d", mRecordWidth, mRecordHeight));

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
        getCameraProvider().setPictureSize(height, width, isBigger);
    }

    /**
     * Take a full-resolution picture using the provider-based API.
     * Works with both Camera1 and CameraX backends.
     */
    public synchronized void takePicture(final TakePictureCallback photoCallback,
                                         final String config, final float intensity,
                                         final boolean isFrontMirror) {
        ICameraProvider provider = getCameraProvider();

        if (photoCallback == null || !provider.isCameraOpened()) {
            Log.e(LOG_TAG, "takePicture after release!");
            if (photoCallback != null) {
                photoCallback.takePictureOK(null);
            }
            return;
        }

        provider.takePicture(null, (jpegData, facing, rotation) -> {
            if (jpegData == null) {
                photoCallback.takePictureOK(null);
                return;
            }

            Bitmap bmp = BitmapFactory.decodeByteArray(jpegData, 0, jpegData.length);
            if (bmp == null) {
                photoCallback.takePictureOK(null);
                return;
            }

            int width = bmp.getWidth();
            int height = bmp.getHeight();

            // Scale down if exceeds maximum texture size
            if (width > mMaxTextureSize || height > mMaxTextureSize) {
                float scaling = Math.max(width / (float) mMaxTextureSize, height / (float) mMaxTextureSize);
                Log.i(LOG_TAG, String.format("目标尺寸(%d x %d)超过当前设备OpenGL 能够处理的最大范围(%d x %d)， 现在将图片压缩至合理大小!",
                        width, height, mMaxTextureSize, mMaxTextureSize));
                Bitmap scaled = Bitmap.createScaledBitmap(bmp, (int) (width / scaling), (int) (height / scaling), false);
                if (scaled != bmp) {
                    bmp.recycle();
                }
                bmp = scaled;
                width = bmp.getWidth();
                height = bmp.getHeight();
            }

            boolean shouldRotate = shouldRotateForCapture(provider, bmp, jpegData, rotation);
            boolean isBackCamera = (facing == ICameraProvider.CameraFacing.BACK);
            Bitmap bmp2;

            if (shouldRotate) {
                bmp2 = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(bmp2);

                if (isBackCamera) {
                    Matrix mat = new Matrix();
                    int halfLen = Math.min(width, height) / 2;
                    mat.setRotate(rotation, halfLen, halfLen);
                    canvas.drawBitmap(bmp, mat, null);
                } else {
                    Matrix mat = new Matrix();
                    if (isFrontMirror) {
                        mat.postTranslate(-width / 2.0f, -height / 2.0f);
                        mat.postScale(-1.0f, 1.0f);
                        mat.postTranslate(width / 2.0f, height / 2.0f);
                        int halfLen = Math.min(width, height) / 2;
                        mat.postRotate(rotation, halfLen, halfLen);
                    } else {
                        int halfLen = Math.max(width, height) / 2;
                        mat.postRotate(-rotation, halfLen, halfLen);
                    }
                    canvas.drawBitmap(bmp, mat, null);
                }
                bmp.recycle();
            } else {
                if (isBackCamera) {
                    bmp2 = bmp;
                } else {
                    bmp2 = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                    Canvas canvas = new Canvas(bmp2);
                    Matrix mat = new Matrix();
                    if (isFrontMirror) {
                        mat.postTranslate(-width / 2.0f, -height / 2.0f);
                        mat.postScale(1.0f, -1.0f);
                        mat.postTranslate(width / 2.0f, height / 2.0f);
                    } else {
                        mat.postTranslate(-width / 2.0f, -height / 2.0f);
                        mat.postScale(-1.0f, -1.0f);
                        mat.postTranslate(width / 2.0f, height / 2.0f);
                    }
                    canvas.drawBitmap(bmp, mat, null);
                    bmp.recycle();
                }
            }

            if (config != null) {
                CGENativeLibrary.filterImage_MultipleEffectsWriteBack(bmp2, config, intensity);
            }

            photoCallback.takePictureOK(bmp2);
            provider.resumePreviewAfterCapture();
        });
    }

    private boolean shouldRotateForCapture(ICameraProvider provider, Bitmap bitmap, byte[] jpegData, int fallbackRotation) {
        if (provider instanceof Camera1Provider) {
            return shouldRotateForCamera1Legacy((Camera1Provider) provider, bitmap, jpegData, fallbackRotation);
        }
        return fallbackRotation == 90 || fallbackRotation == 270;
    }

    private boolean shouldRotateForCamera1Legacy(Camera1Provider provider, Bitmap bitmap, byte[] jpegData, int fallbackRotation) {
        Camera.Parameters params = provider.getCameraInstance().getParams();
        Camera.Size pictureSize = params != null ? params.getPictureSize() : null;

        if (pictureSize != null && pictureSize.width != pictureSize.height) {
            int width = bitmap.getWidth();
            int height = bitmap.getHeight();
            return (pictureSize.width > pictureSize.height && width > height)
                    || (pictureSize.width < pictureSize.height && width < height);
        }

        if (jpegData != null) {
            Boolean shouldRotate = shouldRotateByExif(jpegData);
            if (shouldRotate != null) {
                return shouldRotate;
            }
        }

        return fallbackRotation == 90 || fallbackRotation == 270;
    }

    private Boolean shouldRotateByExif(byte[] jpegData) {
        String cacheDir = getContext().getExternalCacheDir() != null ? getContext().getExternalCacheDir().getAbsolutePath() : null;
        if (cacheDir == null) {
            return null;
        }

        String tmpFilename = cacheDir + "/picture_cache000.jpg";
        try (FileOutputStream fileout = new FileOutputStream(tmpFilename);
             BufferedOutputStream bufferOutStream = new BufferedOutputStream(fileout)) {
            bufferOutStream.write(jpegData);
            bufferOutStream.flush();

            ExifInterface exifInterface = new ExifInterface(tmpFilename);
            int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
            return orientation == ExifInterface.ORIENTATION_ROTATE_90
                    || orientation == ExifInterface.ORIENTATION_ROTATE_270;
        } catch (IOException e) {
            Log.e(LOG_TAG, "Err when reading exif from cache image: " + e.toString());
            return null;
        }
    }

    /**
     * @deprecated Use {@link #takePicture(TakePictureCallback, String, float, boolean)} instead.
     *             This overload accepts a Camera1 ShutterCallback for backward compatibility.
     */
    @Deprecated
    public synchronized void takePicture(final TakePictureCallback photoCallback,
                                         Camera.ShutterCallback shutterCallback,
                                         final String config, final float intensity,
                                         final boolean isFrontMirror) {
        // Ignore the Camera1-specific shutterCallback, delegate to the new method
        takePicture(photoCallback, config, intensity, isFrontMirror);
    }
}
