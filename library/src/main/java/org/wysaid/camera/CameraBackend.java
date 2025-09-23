package org.wysaid.camera;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;

/**
 * Abstract camera backend interface for supporting both legacy Camera API and Camera2 API
 * This provides a unified interface that can be implemented by different camera backends
 */
public abstract class CameraBackend {
    
    public static final int CAMERA_FACING_BACK = 0;
    public static final int CAMERA_FACING_FRONT = 1;
    
    public interface CameraOpenCallback {
        void cameraReady();
    }
    
    public interface AutoFocusCallback {
        void onAutoFocus(boolean success);
    }
    
    // Core camera operations
    public abstract boolean tryOpenCamera(CameraOpenCallback callback, int facing);
    public abstract void stopCamera();
    public abstract boolean isCameraOpened();
    
    // Preview operations
    public abstract void startPreview(SurfaceTexture texture, Camera.PreviewCallback callback);
    public abstract void startPreview(SurfaceTexture texture);
    public abstract void startPreview(Camera.PreviewCallback callback);
    public abstract void stopPreview();
    public abstract boolean isPreviewing();
    
    // Camera properties
    public abstract int previewWidth();
    public abstract int previewHeight();
    public abstract int pictureWidth();
    public abstract int pictureHeight();
    public abstract int getFacing();
    
    // Configuration
    public abstract void setPreferPreviewSize(int w, int h);
    public abstract void setPictureSize(int width, int height, boolean isBigger);
    public abstract void setFocusMode(String focusMode);
    public abstract void focusAtPoint(float x, float y, AutoFocusCallback callback);
    public abstract void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback);
    
    // Parameter access (for legacy compatibility)
    public abstract Object getParams();
    public abstract void setParams(Object params);
    public abstract Object getCameraDevice();
    
    // Helper method for backend type identification
    public abstract String getBackendType();
}