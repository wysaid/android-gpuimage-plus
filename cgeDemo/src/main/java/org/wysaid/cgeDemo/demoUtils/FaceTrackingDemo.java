package org.wysaid.cgeDemo.demoUtils;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.Common;
import org.wysaid.common.FrameBufferObject;
import org.wysaid.common.ProgramObject;
import org.wysaid.nativePort.CGEFaceTracker;
import org.wysaid.view.TrackingCameraGLSurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Data: 2017/10/29
 * Description:
 */

public class FaceTrackingDemo implements TrackingCameraGLSurfaceView.TrackingProc {

    protected static final String VSH = "" +
            "attribute vec2 vPosition;\n" +
            "uniform vec2 canvasSize;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_PointSize = 10.0;\n" +
            "   gl_Position = vec4((vPosition / canvasSize) * 2.0 - 1.0, 0.0, 1.0);\n" +
            "   gl_Position.y = -gl_Position.y;\n" + // Mirror on screen.
            "}";

    protected static final String FSH = "" +
            "precision mediump float;\n" +
            "uniform vec4 color;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = color;\n" +
            "}";

    private int mOriginWidth, mOriginHeight;
    private int mMaxSize = 320;
    private ByteBuffer mImageCacheBuffer;
    private Bitmap mBitmapSrc, mBitmapDst;
    private Matrix mTransformMatrix;
    private Canvas mTransformCanvas;
    protected CGEFaceTracker mTracker;
    protected int mWidth, mHeight;
    protected int[] mFaceTrackingLock = new int[0];
    protected ProgramObject mProgramObject;
    protected CGEFaceTracker.FaceResult mFaceResult;

    @Override
    public boolean setup(int width, int height) {
        mTracker = CGEFaceTracker.createFaceTracker();
        mProgramObject = new ProgramObject();
        mProgramObject.bindAttribLocation("vPosition", 0);
        if (!mProgramObject.init(VSH, FSH)) {
            mProgramObject.release();
            mProgramObject = null;
            return false;
        }

        resize(width, height);
        return true;
    }

    @Override
    public void resize(int width, int height) {

        if (mOriginWidth == width && mOriginHeight == height)
            return;

        float scaling = mMaxSize / (float) Math.max(width, height);

        if (scaling > 1.0f) {
            mWidth = width;
            mHeight = height;
        } else {
            mWidth = (int) (width * scaling);
            mHeight = (int) (height * scaling);
        }

        mOriginWidth = width;
        mOriginHeight = height;

        mProgramObject.bind();
        mProgramObject.sendUniformf("canvasSize", mWidth, mHeight);
        mProgramObject.sendUniformf("color", 1, 0, 0, 0);

        mImageCacheBuffer = ByteBuffer.allocateDirect(mWidth * mHeight).order(ByteOrder.nativeOrder());
        mBitmapSrc = Bitmap.createBitmap(mOriginHeight, mOriginWidth, Bitmap.Config.ALPHA_8);
        mBitmapDst = Bitmap.createBitmap(mWidth, mHeight, Bitmap.Config.ALPHA_8);
        mTransformMatrix = new Matrix();
        mTransformMatrix.setScale(scaling, -scaling);
        mTransformMatrix.postRotate(90);
        mTransformMatrix.postTranslate(-mWidth / 2, -mHeight / 2);
        mTransformMatrix.postRotate(180);
        mTransformMatrix.postTranslate(mWidth / 2, mHeight / 2);
        mTransformCanvas = new Canvas(mBitmapDst);
        mTransformCanvas.setMatrix(mTransformMatrix);
    }


    @Override
    public void processTracking(ByteBuffer luminanceBuffer) {
        if (luminanceBuffer == null) {
            return;
        }

        mBitmapSrc.copyPixelsFromBuffer(luminanceBuffer.position(0));
        mBitmapDst.eraseColor(0);
        mTransformCanvas.drawBitmap(mBitmapSrc, 0.0f, 0.0f, null);
        mBitmapDst.copyPixelsToBuffer(mImageCacheBuffer.position(0));

        long tm = System.currentTimeMillis();

        mImageCacheBuffer.position(0);
        boolean ret = mTracker.detectFaceWithGrayBuffer(mImageCacheBuffer, mWidth, mHeight, mWidth);

        synchronized (mFaceTrackingLock) {
            if (ret) {
                mFaceResult = mTracker.getFaceResult();
            } else {
                mFaceResult = null;
            }
        }

        long totalTime = System.currentTimeMillis() - tm;
        Log.i(Common.LOG_TAG, String.format("tracking time: %g s", totalTime / 1000.0f));
    }

    @Override
    public void render(TrackingCameraGLSurfaceView glView) {

        glView.drawCurrentFrame();

        synchronized (mFaceTrackingLock) {
            if (mProgramObject == null || mFaceResult == null)
                return;

            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
            GLES20.glEnableVertexAttribArray(0);
            GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, mFaceResult.faceKeyPoints.position(0));

            mProgramObject.bind();
            GLES20.glDrawArrays(GLES20.GL_POINTS, 0, 66);
        }

        GLES20.glFinish();
    }

    @Override
    public void release() {
        if (mTracker != null) {
            mTracker.release();
            mTracker = null;
        }

        if (mProgramObject != null) {
            mProgramObject.release();
            mProgramObject = null;
        }
    }
}
