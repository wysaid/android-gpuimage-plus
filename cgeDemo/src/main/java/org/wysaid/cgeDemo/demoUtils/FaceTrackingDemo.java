package org.wysaid.cgeDemo.demoUtils;

import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.Common;
import org.wysaid.common.ProgramObject;
import org.wysaid.nativePort.CGEFaceTracker;
import org.wysaid.nativePort.CGEFrameRenderer;
import org.wysaid.view.TrackingCameraGLSurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * @Author: wangyang
 * @Mail: admin@wysaid.org
 * @Date: 2017/10/29
 * @Description:
 */

public class FaceTrackingDemo implements TrackingCameraGLSurfaceView.TrackingProc {

    protected static final String VSH = "" +
            "attribute vec2 vPosition;\n" +
            "uniform vec2 canvasSize;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_PointSize = 3.0;\n" +
            "   gl_Position = vec4((vPosition / canvasSize) * 2.0 - 1.0, 0.0, 1.0);\n" +
            "}";

    protected static final String FSH = "" +
            "precision mediump float;\n" +
            "uniform vec4 color;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = color;\n" +
            "}";

    CGEFaceTracker mTracker;
    int mWidth, mHeight;
    int mOriginWidth, mOriginHeight;
    int mMaxSize = 240;
    ByteBuffer mImageCacheBuffer;
    ProgramObject mProgramObject;
    CGEFaceTracker.FaceResult mFaceResult;

    @Override
    public boolean setup(CGEFrameRenderer renderer, int width, int height) {
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

        float scaling = mMaxSize / (float) Math.min(width, height);

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

        mImageCacheBuffer = ByteBuffer.allocateDirect(mWidth * mHeight * 4).order(ByteOrder.nativeOrder());
    }

    @Override
    public void processTracking(CGEFrameRenderer renderer) {
        if (mImageCacheBuffer == null) {
            return;
        }

        long tm = System.currentTimeMillis();

        //A simple but risky trick. Please choose a better way to receive the buffer.
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glViewport(0, 0, mWidth, mHeight);
        renderer.drawCache();
        //You may have a better way to avoid calling of readpixel.
        GLES20.glReadPixels(0, 0, mWidth, mHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mImageCacheBuffer.rewind());

        long readPixelTime = System.currentTimeMillis() - tm;

        //No matter what the rgb order is, only gray space is used at last.
        //So just call this.
        boolean ret = mTracker.detectFaceWithBGRABuffer(mImageCacheBuffer, mWidth, mHeight, mWidth * 4);
        if (ret) {
            //The processTracking & render function is in the same thread for this demo.
            //So no sync code is write.
            mFaceResult = mTracker.getFaceResult();
        } else {
            mFaceResult = null;
        }

        long totalTime = System.currentTimeMillis() - tm;
        Log.i(Common.LOG_TAG, String.format("read pixel time: %g, tracking time: %g, total: %g", readPixelTime / 1000.0f, (totalTime - readPixelTime) / 1000.0f, totalTime / 1000.0f));
    }

    @Override
    public void render(CGEFrameRenderer renderer) {
        if (mProgramObject == null || mFaceResult == null)
            return;

        renderer.bindImageFBO();
        GLES20.glViewport(0, 0, mOriginWidth, mOriginHeight);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, mFaceResult.faceKeyPoints.position(0));

        mProgramObject.bind();
        GLES20.glDrawArrays(GLES20.GL_POINTS, 0, 66);
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
