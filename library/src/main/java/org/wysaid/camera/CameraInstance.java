package org.wysaid.camera;

import android.content.Context;
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
 * Created by wangyang on 15/7/27.
 * 
 * Modified to support both legacy Camera API and Camera2 API through backend abstraction
 */


// Camera 仅适用单例
public class CameraInstance {
    public static final String LOG_TAG = Common.LOG_TAG;

    private static final String ASSERT_MSG = "检测到CameraDevice 为 null! 请检查";

    // Backend abstraction
    private CameraBackend mCameraBackend;
    private Context mContext;

    // Legacy compatibility fields - these delegate to the backend
    private Camera mCameraDevice; // Kept for legacy compatibility
    private Camera.Parameters mParams; // Kept for legacy compatibility

    public static final int DEFAULT_PREVIEW_RATE = 30;

    private static CameraInstance mThisInstance;

    private CameraInstance() {}

    public static synchronized CameraInstance getInstance() {
        if(mThisInstance == null) {
            mThisInstance = new CameraInstance();
        }
        return mThisInstance;
    }

    /**
     * Initialize the camera instance with a context (required for Camera2)
     * This should be called before using the camera
     */
    public void initializeWithContext(Context context) {
        mContext = context.getApplicationContext();
        Log.i(LOG_TAG, "CameraInstance initialized with context. Backend: " + 
            CameraBackendFactory.getBackendDescription());
    }

    /**
     * Get or create the camera backend
     */
    private CameraBackend getCameraBackend() {
        if (mCameraBackend == null) {
            if (mContext == null) {
                Log.w(LOG_TAG, "Context not set, using legacy backend");
                mCameraBackend = new CameraBackendLegacy();
            } else {
                mCameraBackend = CameraBackendFactory.createCameraBackend(mContext);
            }
            Log.i(LOG_TAG, "Created camera backend: " + mCameraBackend.getBackendType());
        }
        return mCameraBackend;
    }

    public boolean isPreviewing() { 
        return getCameraBackend().isPreviewing();
    }

    public int previewWidth() { 
        return getCameraBackend().previewWidth();
    }
    
    public int previewHeight() { 
        return getCameraBackend().previewHeight();
    }
    
    public int pictureWidth() { 
        return getCameraBackend().pictureWidth();
    }
    
    public int pictureHeight() { 
        return getCameraBackend().pictureHeight();
    }

    public void setPreferPreviewSize(int w, int h) {
        getCameraBackend().setPreferPreviewSize(w, h);
    }

    public interface CameraOpenCallback {
        void cameraReady();
    }

    public boolean tryOpenCamera(CameraOpenCallback callback) {
        return tryOpenCamera(callback, Camera.CameraInfo.CAMERA_FACING_BACK);
    }

    public int getFacing() {
        return getCameraBackend().getFacing();
    }

    public synchronized boolean tryOpenCamera(CameraOpenCallback callback, int facing) {
        Log.i(LOG_TAG, "CameraInstance: try open camera with backend: " + getCameraBackend().getBackendType());
        
        // Convert legacy facing constants to backend constants
        int backendFacing = (facing == Camera.CameraInfo.CAMERA_FACING_BACK) ? 
            CameraBackend.CAMERA_FACING_BACK : CameraBackend.CAMERA_FACING_FRONT;
        
        // Wrap the callback to convert between interfaces
        CameraBackend.CameraOpenCallback backendCallback = null;
        if (callback != null) {
            backendCallback = new CameraBackend.CameraOpenCallback() {
                @Override
                public void cameraReady() {
                    callback.cameraReady();
                }
            };
        }
            
        return getCameraBackend().tryOpenCamera(backendCallback, backendFacing);
    }

    public synchronized void stopCamera() {
        if (mCameraBackend != null) {
            mCameraBackend.stopCamera();
            // Reset backend to allow switching
            mCameraBackend = null;
        }
    }

    public boolean isCameraOpened() {
        return getCameraBackend().isCameraOpened();
    }

    public synchronized void startPreview(SurfaceTexture texture, Camera.PreviewCallback callback) {
        getCameraBackend().startPreview(texture, callback);
    }

    public void startPreview(SurfaceTexture texture) {
        getCameraBackend().startPreview(texture);
    }

    public void startPreview(Camera.PreviewCallback callback) {
        getCameraBackend().startPreview(callback);
    }

    public synchronized void stopPreview() {
        getCameraBackend().stopPreview();
    }

    public synchronized Camera.Parameters getParams() {
        Object params = getCameraBackend().getParams();
        if (params instanceof Camera.Parameters) {
            mParams = (Camera.Parameters) params;
            return mParams;
        }
        // For Camera2, this will return null
        return null;
    }

    public synchronized void setParams(Camera.Parameters param) {
        getCameraBackend().setParams(param);
        mParams = param;
    }

    public Camera getCameraDevice() {
        Object device = getCameraBackend().getCameraDevice();
        if (device instanceof Camera) {
            mCameraDevice = (Camera) device;
            return mCameraDevice;
        }
        // For Camera2, this will return null for legacy compatibility
        return null;
    }

    // Legacy compatibility methods - these now delegate to the backend
    
    //保证从大到小排列
    private Comparator<Camera.Size> comparatorBigger = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = rhs.width - lhs.width;
            if(w == 0)
                return rhs.height - lhs.height;
            return w;
        }
    };

    //保证从小到大排列
    private Comparator<Camera.Size> comparatorSmaller= new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = lhs.width - rhs.width;
            if(w == 0)
                return lhs.height - rhs.height;
            return w;
        }
    };

    public void initCamera(int previewRate) {
        // This method is kept for legacy compatibility but is no longer used
        // The backend handles camera initialization internally
        Log.i(LOG_TAG, "CameraInstance: initCamera called - delegating to backend");
    }

    public synchronized void setFocusMode(String focusMode) {
        getCameraBackend().setFocusMode(focusMode);
    }

    public synchronized void setPictureSize(int width, int height, boolean isBigger) {
        getCameraBackend().setPictureSize(width, height, isBigger);
    }

    public void focusAtPoint(float x, float y, final Camera.AutoFocusCallback callback) {
        focusAtPoint(x, y, 0.2f, callback);
    }

    public synchronized void focusAtPoint(float x, float y, float radius, final Camera.AutoFocusCallback callback) {
        getCameraBackend().focusAtPoint(x, y, radius, new CameraBackend.AutoFocusCallback() {
            @Override
            public void onAutoFocus(boolean success) {
                if (callback != null) {
                    // For legacy compatibility, we need to call with a Camera instance
                    // For Camera2, this will be null, but most code should handle that
                    callback.onAutoFocus(success, getCameraDevice());
                }
            }
        });
    }
    
    /**
     * Set the camera backend type preference
     * This should be called before opening the camera
     */
    public static void setCameraBackendType(CameraBackendFactory.CameraBackendType backendType) {
        CameraBackendFactory.setPreferredBackendType(backendType);
    }
    
    /**
     * Get the current camera backend type
     */
    public static CameraBackendFactory.CameraBackendType getCameraBackendType() {
        return CameraBackendFactory.getSelectedBackendType();
    }
    
    /**
     * Check if Camera2 is supported on this device
     */
    public static boolean isCamera2Supported() {
        return CameraBackendFactory.isCamera2Supported();
    }
    
    /**
     * Get information about the current camera backend configuration
     */
    public static String getCameraBackendInfo() {
        return CameraBackendFactory.getBackendDescription();
    }
}
