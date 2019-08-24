package org.wysaid.view;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.util.AttributeSet;
import android.util.Log;

import org.wysaid.camera.CameraInstance;
import org.wysaid.common.Common;
import org.wysaid.gpuCodec.TextureDrawerNV12ToRGB;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Date: 10/04/2018
 * Description:
 */

// Preview with buffer.
public class CameraGLSurfaceViewWithBuffer extends CameraGLSurfaceView implements Camera.PreviewCallback {

    protected byte[] mPreviewBuffer0;
    protected byte[] mPreviewBuffer1;
    protected TextureDrawerNV12ToRGB mYUVDrawer;
    protected int mTextureY, mTextureUV;
    protected int mTextureWidth, mTextureHeight;
    protected ByteBuffer mBufferY, mBufferUV;
    protected int mYSize, mUVSize;
    protected int mBufferSize;
    protected SurfaceTexture mSurfaceTexture;
    protected boolean mBufferUpdated = false;
    protected final int[] mBufferUpdateLock = new int[0];

    public CameraGLSurfaceViewWithBuffer(Context context, AttributeSet attrs) {
        super(context, attrs);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    @Override
    protected void onRelease() {
        super.onRelease();
        if(mYUVDrawer != null) {
            mYUVDrawer.release();
            mYUVDrawer = null;
        }

        if(mSurfaceTexture != null) {
            mSurfaceTexture.release();
            mSurfaceTexture = null;
        }

//        if(mTextureID != 0) {
//            Common.deleteTextureID(mTextureID);
//            mTextureID = 0;
//        }

        if(mTextureY != 0 || mTextureUV != 0) {
            GLES20.glDeleteTextures(2, new int[]{mTextureY, mTextureUV}, 0);
            mTextureY = mTextureUV = 0;
            mTextureWidth = 0;
            mTextureHeight = 0;
        }
    }

    protected void resizeTextures() {

        if (mTextureY == 0 || mTextureUV == 0) {
            int[] textures = new int[2];
            GLES20.glGenTextures(2, textures, 0);
            mTextureY = textures[0];
            mTextureUV = textures[1];

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureY);
            Common.texParamHelper(GLES20.GL_TEXTURE_2D, GLES20.GL_LINEAR, GLES20.GL_CLAMP_TO_EDGE);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureUV);
            Common.texParamHelper(GLES20.GL_TEXTURE_2D, GLES20.GL_LINEAR, GLES20.GL_CLAMP_TO_EDGE);
        }

        int width = cameraInstance().previewWidth();
        int height = cameraInstance().previewHeight();

        if (mTextureWidth != width || mTextureHeight != height) {
            mTextureWidth = width;
            mTextureHeight = height;

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureY);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, mTextureWidth, mTextureHeight, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, null);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureUV);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE_ALPHA, mTextureWidth / 2, mTextureHeight / 2, 0, GLES20.GL_LUMINANCE_ALPHA, GLES20.GL_UNSIGNED_BYTE, null);
        }
    }

    protected void updateTextures() {
        if(mBufferUpdated) {
            synchronized (mBufferUpdateLock) {
                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureY);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D, 0, 0, 0, mTextureWidth, mTextureHeight, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, mBufferY.position(0));
                GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureUV);
                GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D, 0, 0, 0, mTextureWidth / 2, mTextureHeight / 2, GLES20.GL_LUMINANCE_ALPHA, GLES20.GL_UNSIGNED_BYTE, mBufferUV.position(0));
                mBufferUpdated = false;
            }
        } else {
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureY);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureUV);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        super.onSurfaceCreated(gl, config);

        mYUVDrawer = TextureDrawerNV12ToRGB.create();
        mYUVDrawer.setFlipScale(1.0f, 1.0f);
        mYUVDrawer.setRotation((float) (Math.PI / 2.0));

//        mTextureID = Common.genSurfaceTextureID();
//        mSurfaceTexture = new SurfaceTexture(mTextureID);
        mSurfaceTexture = new SurfaceTexture(0);
//        mSurfaceTexture.setOnFrameAvailableListener(this);

//        mTextureDrawer = TextureDrawer.create();
    }

    @Override
    public void resumePreview() {
        if(mYUVDrawer == null) {
            return;
        }

        if (!cameraInstance().isCameraOpened()) {
            int facing = mIsCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

            cameraInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                @Override
                public void cameraReady() {
                    Log.i(LOG_TAG, "tryOpenCamera OK...");
                }
            }, facing);
        }

        if (!cameraInstance().isPreviewing()) {
            Camera camera = cameraInstance().getCameraDevice();
            Camera.Parameters parameters = camera.getParameters();
            parameters.getPreviewFormat();
            Camera.Size sz = parameters.getPreviewSize();
            int format = parameters.getPreviewFormat();

            if(format != ImageFormat.NV21)
            {
                try {
                    parameters.setPreviewFormat(ImageFormat.NV21);
                    camera.setParameters(parameters);
                    format = ImageFormat.NV21;
                } catch (Exception e) {
                    e.printStackTrace();
                    return ;
                }
            }

            mYSize = sz.width * sz.height;
            int newBufferSize = mYSize * ImageFormat.getBitsPerPixel(format) / 8;

            if(mBufferSize != newBufferSize) {
                mBufferSize = newBufferSize;
                mUVSize = mBufferSize - mYSize;
                mBufferY = ByteBuffer.allocateDirect(mYSize).order(ByteOrder.nativeOrder());
                mBufferUV = ByteBuffer.allocateDirect(mUVSize).order(ByteOrder.nativeOrder());

                mPreviewBuffer0 = new byte[mBufferSize];
                mPreviewBuffer1 = new byte[mBufferSize];
            }

            camera.addCallbackBuffer(mPreviewBuffer0);
            camera.addCallbackBuffer(mPreviewBuffer1);

            cameraInstance().startPreview(mSurfaceTexture, this);
        }

        if(mIsCameraBackForward) {
            mYUVDrawer.setFlipScale(-1.0f, 1.0f);
            mYUVDrawer.setRotation((float) (Math.PI / 2.0));
        } else {
            mYUVDrawer.setFlipScale(1.0f, 1.0f);
            mYUVDrawer.setRotation((float) (Math.PI / 2.0));
        }

        resizeTextures();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        super.onSurfaceChanged(gl, width, height);

        if (!cameraInstance().isPreviewing()) {
            resumePreview();
        }
    }

    public void drawCurrentFrame() {
        if(mYUVDrawer == null) {
            return;
        }

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClearColor(0,0,0,1);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glViewport(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
        updateTextures();
        mYUVDrawer.drawTextures();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        drawCurrentFrame();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        synchronized (mBufferUpdateLock) {
            mBufferY.position(0);
            mBufferUV.position(0);
            mBufferY.put(data, 0, mYSize);
            mBufferUV.put(data, mYSize, mUVSize);
            mBufferUpdated = true;
        }

        camera.addCallbackBuffer(data);
    }
}
