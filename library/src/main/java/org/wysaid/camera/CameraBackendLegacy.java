package org.wysaid.camera;

import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Build;
import android.util.Log;

import org.wysaid.common.Common;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Legacy Camera API backend implementation
 * This wraps the existing Camera API functionality to provide the CameraBackend interface
 */
public class CameraBackendLegacy extends CameraBackend {
    
    public static final String LOG_TAG = Common.LOG_TAG;
    public static final int DEFAULT_PREVIEW_RATE = 30;
    
    private static final String ASSERT_MSG = "检测到CameraDevice 为 null! 请检查";
    
    private Camera mCameraDevice;
    private Camera.Parameters mParams;
    private boolean mIsPreviewing = false;
    private int mDefaultCameraID = -1;
    private int mPreviewWidth;
    private int mPreviewHeight;
    private int mPictureWidth = 1000;
    private int mPictureHeight = 1000;
    private int mPreferPreviewWidth = 640;
    private int mPreferPreviewHeight = 640;
    private int mFacing = 0;
    
    @Override
    public String getBackendType() {
        return "Legacy Camera API";
    }
    
    @Override
    public boolean tryOpenCamera(CameraOpenCallback callback, int facing) {
        Log.i(LOG_TAG, "Legacy Camera: try open camera...");
        
        try {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.FROYO) {
                int numberOfCameras = Camera.getNumberOfCameras();
                
                Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
                for (int i = 0; i < numberOfCameras; i++) {
                    Camera.getCameraInfo(i, cameraInfo);
                    int legacyFacing = (facing == CAMERA_FACING_BACK) ? 
                        Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;
                    if (cameraInfo.facing == legacyFacing) {
                        mDefaultCameraID = i;
                        mFacing = facing;
                        break;
                    }
                }
            }
            
            stopPreview();
            if (mCameraDevice != null)
                mCameraDevice.release();
                
            if (mDefaultCameraID >= 0) {
                mCameraDevice = Camera.open(mDefaultCameraID);
            } else {
                mCameraDevice = Camera.open();
                mFacing = CAMERA_FACING_BACK; // default: back facing
            }
        } catch (Exception e) {
            Log.e(LOG_TAG, "Legacy Camera: Open Camera Failed!");
            e.printStackTrace();
            mCameraDevice = null;
            return false;
        }
        
        if (mCameraDevice != null) {
            Log.i(LOG_TAG, "Legacy Camera: Camera opened!");
            
            try {
                initCamera(DEFAULT_PREVIEW_RATE);
            } catch (Exception e) {
                mCameraDevice.release();
                mCameraDevice = null;
                return false;
            }
            
            if (callback != null) {
                callback.cameraReady();
            }
            
            return true;
        }
        
        return false;
    }
    
    @Override
    public void stopCamera() {
        if (mCameraDevice != null) {
            mIsPreviewing = false;
            mCameraDevice.stopPreview();
            mCameraDevice.setPreviewCallback(null);
            mCameraDevice.release();
            mCameraDevice = null;
        }
    }
    
    @Override
    public boolean isCameraOpened() {
        return mCameraDevice != null;
    }
    
    @Override
    public synchronized void startPreview(SurfaceTexture texture, Camera.PreviewCallback callback) {
        Log.i(LOG_TAG, "Legacy Camera: startPreview...");
        if (mIsPreviewing) {
            Log.e(LOG_TAG, "Legacy Camera: Err: camera is previewing...");
            return;
        }
        
        if (mCameraDevice != null) {
            try {
                mCameraDevice.setPreviewTexture(texture);
                mCameraDevice.setPreviewCallbackWithBuffer(callback);
            } catch (IOException e) {
                e.printStackTrace();
            }
            
            mCameraDevice.startPreview();
            mIsPreviewing = true;
        }
    }
    
    @Override
    public void startPreview(SurfaceTexture texture) {
        startPreview(texture, null);
    }
    
    @Override
    public void startPreview(Camera.PreviewCallback callback) {
        startPreview(null, callback);
    }
    
    @Override
    public synchronized void stopPreview() {
        if (mIsPreviewing && mCameraDevice != null) {
            Log.i(LOG_TAG, "Legacy Camera: stopPreview...");
            mIsPreviewing = false;
            mCameraDevice.stopPreview();
        }
    }
    
    @Override
    public boolean isPreviewing() {
        return mIsPreviewing;
    }
    
    @Override
    public int previewWidth() {
        return mPreviewWidth;
    }
    
    @Override
    public int previewHeight() {
        return mPreviewHeight;
    }
    
    @Override
    public int pictureWidth() {
        return mPictureWidth;
    }
    
    @Override
    public int pictureHeight() {
        return mPictureHeight;
    }
    
    @Override
    public int getFacing() {
        return mFacing;
    }
    
    @Override
    public void setPreferPreviewSize(int w, int h) {
        mPreferPreviewHeight = w;
        mPreferPreviewWidth = h;
    }
    
    @Override
    public synchronized void setPictureSize(int width, int height, boolean isBigger) {
        if (mCameraDevice == null) {
            mPictureWidth = width;
            mPictureHeight = height;
            return;
        }
        
        mParams = mCameraDevice.getParameters();
        
        List<Camera.Size> picSizes = mParams.getSupportedPictureSizes();
        Camera.Size picSz = null;
        
        if (isBigger) {
            Collections.sort(picSizes, comparatorBigger);
            for (Camera.Size sz : picSizes) {
                if (picSz == null || (sz.width >= width && sz.height >= height)) {
                    picSz = sz;
                }
            }
        } else {
            Collections.sort(picSizes, comparatorSmaller);
            for (Camera.Size sz : picSizes) {
                if (picSz == null || (sz.width <= width && sz.height <= height)) {
                    picSz = sz;
                }
            }
        }
        
        mPictureWidth = picSz.width;
        mPictureHeight = picSz.height;
        
        try {
            mParams.setPictureSize(mPictureWidth, mPictureHeight);
            mCameraDevice.setParameters(mParams);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    @Override
    public synchronized void setFocusMode(String focusMode) {
        if (mCameraDevice == null)
            return;
            
        mParams = mCameraDevice.getParameters();
        List<String> focusModes = mParams.getSupportedFocusModes();
        if (focusModes.contains(focusMode)) {
            mParams.setFocusMode(focusMode);
        }
    }
    
    @Override
    public void focusAtPoint(float x, float y, AutoFocusCallback callback) {
        focusAtPoint(x, y, 0.2f, callback);
    }
    
    @Override
    public synchronized void focusAtPoint(float x, float y, float radius, final AutoFocusCallback callback) {
        if (mCameraDevice == null) {
            Log.e(LOG_TAG, "Legacy Camera: Error: focus after release.");
            return;
        }
        
        mParams = mCameraDevice.getParameters();
        
        if (mParams.getMaxNumMeteringAreas() > 0) {
            int focusRadius = (int) (radius * 1000.0f);
            int left = (int) (x * 2000.0f - 1000.0f) - focusRadius;
            int top = (int) (y * 2000.0f - 1000.0f) - focusRadius;
            
            Rect focusArea = new Rect();
            focusArea.left = Math.max(left, -1000);
            focusArea.top = Math.max(top, -1000);
            focusArea.right = Math.min(left + focusRadius, 1000);
            focusArea.bottom = Math.min(top + focusRadius, 1000);
            List<Camera.Area> meteringAreas = new ArrayList<Camera.Area>();
            meteringAreas.add(new Camera.Area(focusArea, 800));
            
            try {
                mCameraDevice.cancelAutoFocus();
                mParams.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                mParams.setFocusAreas(meteringAreas);
                mCameraDevice.setParameters(mParams);
                mCameraDevice.autoFocus(new Camera.AutoFocusCallback() {
                    @Override
                    public void onAutoFocus(boolean success, Camera camera) {
                        if (callback != null) {
                            callback.onAutoFocus(success);
                        }
                    }
                });
            } catch (Exception e) {
                Log.e(LOG_TAG, "Legacy Camera: Error: focusAtPoint failed: " + e.toString());
            }
        } else {
            Log.i(LOG_TAG, "Legacy Camera: The device does not support metering areas...");
            try {
                mCameraDevice.autoFocus(new Camera.AutoFocusCallback() {
                    @Override
                    public void onAutoFocus(boolean success, Camera camera) {
                        if (callback != null) {
                            callback.onAutoFocus(success);
                        }
                    }
                });
            } catch (Exception e) {
                Log.e(LOG_TAG, "Legacy Camera: Error: focusAtPoint failed: " + e.toString());
            }
        }
    }
    
    @Override
    public synchronized Object getParams() {
        if (mCameraDevice != null)
            return mCameraDevice.getParameters();
        assert mCameraDevice != null : ASSERT_MSG;
        return null;
    }
    
    @Override
    public synchronized void setParams(Object params) {
        if (mCameraDevice != null && params instanceof Camera.Parameters) {
            mParams = (Camera.Parameters) params;
            mCameraDevice.setParameters(mParams);
        }
        assert mCameraDevice != null : ASSERT_MSG;
    }
    
    @Override
    public Object getCameraDevice() {
        return mCameraDevice;
    }
    
    // Private helper methods
    
    // 保证从大到小排列
    private Comparator<Camera.Size> comparatorBigger = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = rhs.width - lhs.width;
            if (w == 0)
                return rhs.height - lhs.height;
            return w;
        }
    };
    
    // 保证从小到大排列
    private Comparator<Camera.Size> comparatorSmaller = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = lhs.width - rhs.width;
            if (w == 0)
                return lhs.height - rhs.height;
            return w;
        }
    };
    
    private void initCamera(int previewRate) {
        if (mCameraDevice == null) {
            Log.e(LOG_TAG, "Legacy Camera: initCamera: Camera is not opened!");
            return;
        }
        
        mParams = mCameraDevice.getParameters();
        List<Integer> supportedPictureFormats = mParams.getSupportedPictureFormats();
        
        for (int fmt : supportedPictureFormats) {
            Log.i(LOG_TAG, String.format("Legacy Camera: Picture Format: %x", fmt));
        }
        
        mParams.setPictureFormat(PixelFormat.JPEG);
        
        List<Camera.Size> picSizes = mParams.getSupportedPictureSizes();
        Camera.Size picSz = null;
        
        Collections.sort(picSizes, comparatorBigger);
        
        for (Camera.Size sz : picSizes) {
            Log.i(LOG_TAG, String.format("Legacy Camera: Supported picture size: %d x %d", sz.width, sz.height));
            if (picSz == null || (sz.width >= mPictureWidth && sz.height >= mPictureHeight)) {
                picSz = sz;
            }
        }
        
        List<Camera.Size> prevSizes = mParams.getSupportedPreviewSizes();
        Camera.Size prevSz = null;
        
        Collections.sort(prevSizes, comparatorBigger);
        
        for (Camera.Size sz : prevSizes) {
            Log.i(LOG_TAG, String.format("Legacy Camera: Supported preview size: %d x %d", sz.width, sz.height));
            if (prevSz == null || (sz.width >= mPreferPreviewWidth && sz.height >= mPreferPreviewHeight)) {
                prevSz = sz;
            }
        }
        
        List<Integer> frameRates = mParams.getSupportedPreviewFrameRates();
        
        int fpsMax = 0;
        
        for (Integer n : frameRates) {
            Log.i(LOG_TAG, "Legacy Camera: Supported frame rate: " + n);
            if (fpsMax < n) {
                fpsMax = n;
            }
        }
        
        mParams.setPreviewSize(prevSz.width, prevSz.height);
        mParams.setPictureSize(picSz.width, picSz.height);
        
        List<String> focusModes = mParams.getSupportedFocusModes();
        if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            mParams.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }
        
        previewRate = fpsMax;
        mParams.setPreviewFrameRate(previewRate); // 设置相机预览帧率
        
        try {
            mCameraDevice.setParameters(mParams);
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        mParams = mCameraDevice.getParameters();
        
        Camera.Size szPic = mParams.getPictureSize();
        Camera.Size szPrev = mParams.getPreviewSize();
        
        mPreviewWidth = szPrev.width;
        mPreviewHeight = szPrev.height;
        
        mPictureWidth = szPic.width;
        mPictureHeight = szPic.height;
        
        Log.i(LOG_TAG, String.format("Legacy Camera: Picture Size: %d x %d", szPic.width, szPic.height));
        Log.i(LOG_TAG, String.format("Legacy Camera: Preview Size: %d x %d", szPrev.width, szPrev.height));
    }
}