package org.wysaid.view;

import android.content.Context;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;

import java.nio.ByteBuffer;

import javax.microedition.khronos.opengles.GL10;

/**
 * @Author: wangyang
 * @Mail: admin@wysaid.org
 * @Date: 2017/10/29
 * @Description:
 */

// A simple case for extra tracking.

public class TrackingCameraGLSurfaceView extends CameraGLSurfaceViewWithBuffer {

    public TrackingCameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public interface TrackingProc {
        boolean setup(int width, int height);
        void resize(int width, int height);
        void processTracking(ByteBuffer luminanceBuffer);
        void render(TrackingCameraGLSurfaceView glView);
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

        if (proc == null)
            return true;

        if(!proc.setup(mRecordWidth, mRecordHeight)) {
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

        if(mBufferUpdated && mTrackingProc != null) {
            synchronized (mBufferUpdateLock) {
                mTrackingProc.processTracking(mBufferY);
            }
        }

        if(mTrackingProc == null) {
            super.onDrawFrame(gl);
        } else {
            mTrackingProc.render(this);
        }
    }
}
