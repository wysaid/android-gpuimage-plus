package org.wysaid.camera;

import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Build;
import android.util.Log;

import org.wysaid.myUtils.Common;

import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Created by wangyang on 15/7/27.
 */


// Camera 仅适用单例
public class CameraInstance {
    public static final String LOG_TAG = Common.LOG_TAG;

    private Camera mCameraDevice;
    private Camera.Parameters mParams;

    public static final int DEFAULT_PREVIEW_RATE = 30;


    private boolean mIsPreviewing = false;

    private int mDefaultCameraID = -1;

    private static CameraInstance mThisInstance;
    private int mPreviewWidth;
    private int mPreviewHeight;

    private int mPictureWidth;
    private int mPictureHeight;

    private int mPreferPreviewWidth;
    private int mPreferPreviewHeight;

    private int mFacing = 0;

    private CameraInstance() {}

    public static synchronized CameraInstance getInstance() {
        if(mThisInstance == null) {
            mThisInstance = new CameraInstance();
        }
        return mThisInstance;
    }

    public boolean isPreviewing() { return mIsPreviewing; }

    public int previewWidth() { return mPreviewWidth; }
    public int previewHeight() { return mPreviewHeight; }
    public int pictureWidth() { return mPictureWidth; }
    public int pictureHeight() { return mPictureHeight; }

    public void setPreferPreviewSize(int w, int h) {
        mPreferPreviewHeight = w;
        mPreferPreviewWidth = h;
    }

    public interface CameraOpenCallback {
        void cameraReady();
    }

    public boolean tryOpenCamera(CameraOpenCallback callback) {
        return tryOpenCamera(callback, Camera.CameraInfo.CAMERA_FACING_BACK);
    }

    public int getFacing() {
        return mFacing;
    }

    public boolean tryOpenCamera(CameraOpenCallback callback, int facing) {
        Log.i(LOG_TAG, "try open camera...");

        try
        {
            if(Build.VERSION.SDK_INT > Build.VERSION_CODES.FROYO)
            {
                int numberOfCameras = Camera.getNumberOfCameras();

                Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
                for (int i = 0; i < numberOfCameras; i++) {
                    Camera.getCameraInfo(i, cameraInfo);
                    if (cameraInfo.facing == facing) {
                        mDefaultCameraID = i;
                        mFacing = facing;
                    }
                }
            }
            stopPreview();
            if(mCameraDevice != null)
                mCameraDevice.release();

            if(mDefaultCameraID >= 0)
                mCameraDevice = Camera.open(mDefaultCameraID);
            else
                mCameraDevice = Camera.open();
        }
        catch(Exception e)
        {
            Log.e(LOG_TAG, "Open Camera Failed!");
            e.printStackTrace();
            return false;
        }

        if(mCameraDevice != null) {
            Log.i(LOG_TAG, "Camera opened!");
            initCamera(DEFAULT_PREVIEW_RATE);

            if (callback != null) {
                callback.cameraReady();
            }
        }
        return true;
    }

    public void stopCamera() {
        if(mCameraDevice != null) {
            mIsPreviewing = false;
            mCameraDevice.stopPreview();
            mCameraDevice.setPreviewCallback(null);
            mCameraDevice.release();
            mCameraDevice = null;
        }
    }

    public boolean isCameraOpened() {
        return mCameraDevice != null;
    }

    public void startPreview(SurfaceTexture texture) {
        Log.i(LOG_TAG, "Camera startPreview...");
        if(mIsPreviewing) {
            Log.e(LOG_TAG, "Err: camera is previewing...");
//            stopPreview();
            return ;
        }

        if(mCameraDevice != null) {
            try {
                mCameraDevice.setPreviewTexture(texture);
            } catch (IOException e) {
                e.printStackTrace();
            }

            mCameraDevice.startPreview();
            mIsPreviewing = true;
        }
    }

    public void stopPreview() {
        if(mIsPreviewing && mCameraDevice != null) {
            Log.i(LOG_TAG, "Camera stopPreview...");
            mIsPreviewing = false;
            mCameraDevice.stopPreview();
        }
    }

    public Camera.Parameters getParams() {
        return mCameraDevice.getParameters();
    }

    public void setParams(Camera.Parameters param) {
        mParams = param;
        mCameraDevice.setParameters(mParams);
    }

    public Camera getCameraDevice() {
        return mCameraDevice;
    }

    //保证从大到小排列
    private Comparator<Camera.Size> comparatorBigger = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            return rhs.width - lhs.width;
        }
    };

    //保证从小到大排列
    private Comparator<Camera.Size> comparatorSmaller= new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            return lhs.width - rhs.width;
        }
    };

    public void initCamera(int previewRate) {
        if(mCameraDevice == null) {
            Log.e(LOG_TAG, "initCamera: Camera is not opened!");
            return;
        }

        mParams = mCameraDevice.getParameters();
        List<Integer> supportedPictureFormats = mParams.getSupportedPictureFormats();

        for(int fmt : supportedPictureFormats) {
            Log.i(LOG_TAG, String.format("Picture Format: %x", fmt));
        }

        mParams.setPictureFormat(PixelFormat.JPEG);

        List<Camera.Size> picSizes = mParams.getSupportedPictureSizes();
        Camera.Size picSz = null;

        Collections.sort(picSizes, comparatorBigger);

        for(Camera.Size sz : picSizes) {
            Log.i(LOG_TAG, String.format("Supported picture size: %d x %d", sz.width, sz.height));
            if(picSz == null || (sz.width >= 1000 && sz.height >= 1000)) {
                picSz = sz;
            }
        }

        List<Camera.Size> prevSizes = mParams.getSupportedPreviewSizes();
        Camera.Size prevSz = null;

        Collections.sort(prevSizes, comparatorBigger);

        for(Camera.Size sz : prevSizes) {
            Log.i(LOG_TAG, String.format("Supported preview size: %d x %d", sz.width, sz.height));
            if(prevSz == null || (sz.width >= mPreferPreviewWidth && sz.height >= mPreferPreviewHeight)) {
                prevSz = sz;
            }
        }

        List<Integer> frameRates = mParams.getSupportedPreviewFrameRates();

        int fpsMax = 0;

        for(Integer n : frameRates) {
            Log.i(LOG_TAG, String.format("Supported frame rate: " + n));
            if(fpsMax < n) {
                fpsMax = n;
            }
        }

        mParams.setPreviewSize(prevSz.width, prevSz.height);
        mParams.setPictureSize(picSz.width, picSz.height);

        List<String> focusModes = mParams.getSupportedFocusModes();
        if(focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)){
            mParams.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }

        previewRate = fpsMax;
        mParams.setPreviewFrameRate(previewRate); //设置相机预览帧率
//        mParams.setPreviewFpsRange(20, 60);

        try {
            mCameraDevice.setParameters(mParams);
        }catch (Exception e) {
            e.printStackTrace();
        }


        mParams = mCameraDevice.getParameters();

        Camera.Size szPic = mParams.getPictureSize();
        Camera.Size szPrev = mParams.getPreviewSize();

        mPreviewWidth = szPrev.width;
        mPreviewHeight = szPrev.height;

        mPictureWidth = szPic.width;
        mPictureHeight = szPic.height;

        Log.i(LOG_TAG, String.format("Camera Picture Size: %d x %d", szPic.width, szPic.height));
        Log.i(LOG_TAG, String.format("Camera Preview Size: %d x %d", szPrev.width, szPrev.height));
    }

    public void setPictureSize(int width, int height, boolean isBigger) {
        mParams = mCameraDevice.getParameters();

        List<Camera.Size> picSizes = mParams.getSupportedPictureSizes();
        Camera.Size picSz = null;

        if(isBigger) {
            Collections.sort(picSizes, comparatorBigger);
            for(Camera.Size sz : picSizes) {
                if(picSz == null || (sz.width >= width && sz.height >= height)) {
                    picSz = sz;
                }
            }
        } else {
            Collections.sort(picSizes, comparatorSmaller);
            for(Camera.Size sz : picSizes) {
                if(picSz == null || (sz.width <= width && sz.height <= height)) {
                    picSz = sz;
                }
            }
        }

        mPictureWidth = picSz.width;
        mPictureHeight= picSz.height;

        try {
            mParams.setPictureSize(mPictureWidth, mPictureHeight);
            mCameraDevice.setParameters(mParams);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
