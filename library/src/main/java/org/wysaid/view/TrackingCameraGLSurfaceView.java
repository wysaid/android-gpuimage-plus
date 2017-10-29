package org.wysaid.view;

import android.content.Context;
import android.opengl.GLES20;
import android.util.AttributeSet;
import android.util.Log;

import org.wysaid.nativePort.CGEFrameRenderer;

import javax.microedition.khronos.opengles.GL10;

/**
 * @Author: wangyang
 * @Mail: admin@wysaid.org
 * @Date: 2017/10/29
 * @Description:
 */

// A simple case for extra tracking.

public class TrackingCameraGLSurfaceView extends CameraRecordGLSurfaceView {

    public TrackingCameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public interface TrackingProc {
        boolean setup(CGEFrameRenderer renderer, int width, int height);
        void resize(int width, int height);
        void processTracking(CGEFrameRenderer renderer);
        void render(CGEFrameRenderer renderer);
        void release();
    }

    protected TrackingProc mTrackingProc;

    public TrackingProc getTrackingProc() {
        return mTrackingProc;
    }

    //must be called in the gl thread.
    public boolean setTrackingProc(final TrackingProc proc) {
        if (mTrackingProc != null) {
            mTrackingProc.release();
            mTrackingProc = null;
        }

        if (proc == null || mFrameRecorder == null)
            return true;

        if(!proc.setup(mFrameRecorder, mRecordWidth, mRecordHeight)) {
            Log.e(LOG_TAG, "setup proc failed!");
            proc.release();
            return false;
        }
        mTrackingProc = proc;
        return true;
    }

    @Override
    protected void onRelease() {
        super.onRelease();
        if (mTrackingProc != null) {
            mTrackingProc.release();
            mTrackingProc = null;
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if(mSurfaceTexture == null || !cameraInstance().isPreviewing()) {
            return;
        }

        mSurfaceTexture.updateTexImage();
        mSurfaceTexture.getTransformMatrix(mTransformMatrix);
        mFrameRecorder.update(mTextureID, mTransformMatrix);

        if(mTrackingProc != null) {

            //`processTracking` is writing here for this case.
            mTrackingProc.processTracking(mFrameRecorder);
            mTrackingProc.render(mFrameRecorder);
        }

        mFrameRecorder.runProc();

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        if(mIsUsingMask) {
            GLES20.glEnable(GLES20.GL_BLEND);
            GLES20.glBlendFunc(GLES20.GL_ONE, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        }

        mFrameRecorder.render(mDrawViewport.x, mDrawViewport.y, mDrawViewport.width, mDrawViewport.height);
        GLES20.glDisable(GLES20.GL_BLEND);
    }
}
