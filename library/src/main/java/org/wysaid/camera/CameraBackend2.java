package org.wysaid.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import androidx.annotation.NonNull;

import org.wysaid.common.Common;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Camera2 API backend implementation
 * This provides Camera2 API functionality through the CameraBackend interface
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class CameraBackend2 extends CameraBackend {
    
    public static final String LOG_TAG = Common.LOG_TAG;
    
    private Context mContext;
    private CameraManager mCameraManager;
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCaptureSession;
    private String mCameraId;
    private CameraCharacteristics mCameraCharacteristics;
    
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    
    private boolean mIsPreviewing = false;
    private int mFacing = CAMERA_FACING_BACK;
    
    private int mPreviewWidth = 640;
    private int mPreviewHeight = 480;
    private int mPictureWidth = 1000;
    private int mPictureHeight = 1000;
    private int mPreferPreviewWidth = 640;
    private int mPreferPreviewHeight = 640;
    
    private SurfaceTexture mPreviewSurfaceTexture;
    private Surface mPreviewSurface;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private CaptureRequest mPreviewRequest;
    
    public CameraBackend2(Context context) {
        mContext = context;
        mCameraManager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        startBackgroundThread();
    }
    
    @Override
    public String getBackendType() {
        return "Camera2 API";
    }
    
    @Override
    public boolean tryOpenCamera(CameraOpenCallback callback, int facing) {
        Log.i(LOG_TAG, "Camera2: try open camera...");
        
        try {
            mFacing = facing;
            String cameraId = getCameraId(facing);
            if (cameraId == null) {
                Log.e(LOG_TAG, "Camera2: No camera found for facing: " + facing);
                return false;
            }
            
            mCameraId = cameraId;
            mCameraCharacteristics = mCameraManager.getCameraCharacteristics(mCameraId);
            
            // Set up preview and picture sizes
            setupCameraSizes();
            
            mCameraManager.openCamera(mCameraId, new CameraDevice.StateCallback() {
                @Override
                public void onOpened(@NonNull CameraDevice camera) {
                    Log.i(LOG_TAG, "Camera2: Camera opened!");
                    mCameraDevice = camera;
                    if (callback != null) {
                        callback.cameraReady();
                    }
                }
                
                @Override
                public void onDisconnected(@NonNull CameraDevice camera) {
                    Log.w(LOG_TAG, "Camera2: Camera disconnected");
                    camera.close();
                    mCameraDevice = null;
                }
                
                @Override
                public void onError(@NonNull CameraDevice camera, int error) {
                    Log.e(LOG_TAG, "Camera2: Camera error: " + error);
                    camera.close();
                    mCameraDevice = null;
                }
            }, mBackgroundHandler);
            
            return true;
            
        } catch (CameraAccessException e) {
            Log.e(LOG_TAG, "Camera2: Open Camera Failed!");
            e.printStackTrace();
            return false;
        } catch (SecurityException e) {
            Log.e(LOG_TAG, "Camera2: Camera permission denied!");
            e.printStackTrace();
            return false;
        }
    }
    
    @Override
    public void stopCamera() {
        Log.i(LOG_TAG, "Camera2: Stopping camera...");
        
        stopPreview();
        
        if (mCameraDevice != null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        
        if (mPreviewSurface != null) {
            mPreviewSurface.release();
            mPreviewSurface = null;
        }
        
        stopBackgroundThread();
    }
    
    @Override
    public boolean isCameraOpened() {
        return mCameraDevice != null;
    }
    
    @Override
    public void startPreview(SurfaceTexture texture, Camera.PreviewCallback callback) {
        Log.i(LOG_TAG, "Camera2: Starting preview...");
        
        if (mCameraDevice == null) {
            Log.e(LOG_TAG, "Camera2: Camera device is null");
            return;
        }
        
        try {
            mPreviewSurfaceTexture = texture;
            if (mPreviewSurface != null) {
                mPreviewSurface.release();
            }
            mPreviewSurface = new Surface(texture);
            
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(mPreviewSurface);
            
            // Set up auto focus
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, 
                CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            
            mCameraDevice.createCaptureSession(Arrays.asList(mPreviewSurface), 
                new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(@NonNull CameraCaptureSession session) {
                        if (mCameraDevice == null) {
                            return;
                        }
                        
                        mCaptureSession = session;
                        try {
                            mPreviewRequest = mPreviewRequestBuilder.build();
                            mCaptureSession.setRepeatingRequest(mPreviewRequest, null, mBackgroundHandler);
                            mIsPreviewing = true;
                            Log.i(LOG_TAG, "Camera2: Preview started successfully");
                        } catch (CameraAccessException e) {
                            Log.e(LOG_TAG, "Camera2: Failed to start preview", e);
                        }
                    }
                    
                    @Override
                    public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                        Log.e(LOG_TAG, "Camera2: Failed to configure capture session");
                    }
                }, mBackgroundHandler);
                
        } catch (CameraAccessException e) {
            Log.e(LOG_TAG, "Camera2: Error starting preview", e);
        }
    }
    
    @Override
    public void startPreview(SurfaceTexture texture) {
        startPreview(texture, null);
    }
    
    @Override
    public void startPreview(Camera.PreviewCallback callback) {
        // For Camera2, we need a SurfaceTexture, so this is not directly supported
        Log.w(LOG_TAG, "Camera2: startPreview with only callback is not supported");
    }
    
    @Override
    public void stopPreview() {
        Log.i(LOG_TAG, "Camera2: Stopping preview...");
        
        if (mCaptureSession != null) {
            try {
                mCaptureSession.stopRepeating();
                mCaptureSession.close();
                mCaptureSession = null;
            } catch (CameraAccessException e) {
                Log.e(LOG_TAG, "Camera2: Error stopping preview", e);
            }
        }
        
        mIsPreviewing = false;
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
        mPreferPreviewWidth = w;
        mPreferPreviewHeight = h;
    }
    
    @Override
    public void setPictureSize(int width, int height, boolean isBigger) {
        if (mCameraCharacteristics == null) {
            mPictureWidth = width;
            mPictureHeight = height;
            return;
        }
        
        StreamConfigurationMap map = mCameraCharacteristics.get(
            CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        if (map == null) {
            return;
        }
        
        Size[] pictureSizes = map.getOutputSizes(ImageFormat.JPEG);
        if (pictureSizes == null || pictureSizes.length == 0) {
            return;
        }
        
        Size chosenSize = chooseOptimalSize(pictureSizes, width, height, isBigger);
        mPictureWidth = chosenSize.getWidth();
        mPictureHeight = chosenSize.getHeight();
        
        Log.i(LOG_TAG, String.format("Camera2: Set picture size: %d x %d", mPictureWidth, mPictureHeight));
    }
    
    @Override
    public void setFocusMode(String focusMode) {
        // Camera2 uses different focus modes, we'll map legacy modes to Camera2 equivalents
        Log.i(LOG_TAG, "Camera2: setFocusMode called with legacy mode: " + focusMode);
        // The focus mode is handled in the capture request setup
    }
    
    @Override
    public void focusAtPoint(float x, float y, AutoFocusCallback callback) {
        focusAtPoint(x, y, 0.2f, callback);
    }
    
    @Override
    public void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback) {
        Log.i(LOG_TAG, String.format("Camera2: Focus at point: %f, %f", x, y));
        
        if (mCameraDevice == null || mCaptureSession == null) {
            Log.e(LOG_TAG, "Camera2: Cannot focus, camera not ready");
            if (callback != null) {
                callback.onAutoFocus(false);
            }
            return;
        }
        
        try {
            // For simplicity, we'll just trigger auto focus
            // In a full implementation, you would set up metering areas
            CaptureRequest.Builder builder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            builder.addTarget(mPreviewSurface);
            builder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_AUTO);
            builder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_START);
            
            mCaptureSession.capture(builder.build(), null, mBackgroundHandler);
            
            // For callback compatibility, we'll assume success
            if (callback != null) {
                mBackgroundHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onAutoFocus(true);
                    }
                });
            }
            
        } catch (CameraAccessException e) {
            Log.e(LOG_TAG, "Camera2: Error focusing", e);
            if (callback != null) {
                callback.onAutoFocus(false);
            }
        }
    }
    
    @Override
    public Object getParams() {
        // Camera2 doesn't have a Parameters object, return null for compatibility
        Log.w(LOG_TAG, "Camera2: getParams() not supported in Camera2 API");
        return null;
    }
    
    @Override
    public void setParams(Object params) {
        // Camera2 doesn't use Parameters objects
        Log.w(LOG_TAG, "Camera2: setParams() not supported in Camera2 API");
    }
    
    @Override
    public Object getCameraDevice() {
        return mCameraDevice;
    }
    
    // Private helper methods
    
    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }
    
    private void stopBackgroundThread() {
        if (mBackgroundThread != null) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                Log.e(LOG_TAG, "Camera2: Error stopping background thread", e);
            }
        }
    }
    
    private String getCameraId(int facing) throws CameraAccessException {
        String[] cameraIds = mCameraManager.getCameraIdList();
        
        for (String cameraId : cameraIds) {
            CameraCharacteristics characteristics = mCameraManager.getCameraCharacteristics(cameraId);
            Integer cameraFacing = characteristics.get(CameraCharacteristics.LENS_FACING);
            
            if (cameraFacing != null) {
                if (facing == CAMERA_FACING_BACK && cameraFacing == CameraCharacteristics.LENS_FACING_BACK) {
                    return cameraId;
                } else if (facing == CAMERA_FACING_FRONT && cameraFacing == CameraCharacteristics.LENS_FACING_FRONT) {
                    return cameraId;
                }
            }
        }
        
        return null;
    }
    
    private void setupCameraSizes() {
        if (mCameraCharacteristics == null) {
            return;
        }
        
        StreamConfigurationMap map = mCameraCharacteristics.get(
            CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        if (map == null) {
            return;
        }
        
        // Setup preview size
        Size[] previewSizes = map.getOutputSizes(SurfaceTexture.class);
        if (previewSizes != null && previewSizes.length > 0) {
            Size chosenSize = chooseOptimalSize(previewSizes, mPreferPreviewWidth, mPreferPreviewHeight, true);
            mPreviewWidth = chosenSize.getWidth();
            mPreviewHeight = chosenSize.getHeight();
            Log.i(LOG_TAG, String.format("Camera2: Preview size: %d x %d", mPreviewWidth, mPreviewHeight));
        }
        
        // Setup picture size
        Size[] pictureSizes = map.getOutputSizes(ImageFormat.JPEG);
        if (pictureSizes != null && pictureSizes.length > 0) {
            Size chosenSize = chooseOptimalSize(pictureSizes, mPictureWidth, mPictureHeight, true);
            mPictureWidth = chosenSize.getWidth();
            mPictureHeight = chosenSize.getHeight();
            Log.i(LOG_TAG, String.format("Camera2: Picture size: %d x %d", mPictureWidth, mPictureHeight));
        }
    }
    
    private Size chooseOptimalSize(Size[] choices, int targetWidth, int targetHeight, boolean isBigger) {
        // Sort sizes by area
        List<Size> sizeList = Arrays.asList(choices);
        Collections.sort(sizeList, new Comparator<Size>() {
            @Override
            public int compare(Size lhs, Size rhs) {
                if (isBigger) {
                    // Sort from biggest to smallest
                    return Integer.compare(rhs.getWidth() * rhs.getHeight(), lhs.getWidth() * lhs.getHeight());
                } else {
                    // Sort from smallest to biggest
                    return Integer.compare(lhs.getWidth() * lhs.getHeight(), rhs.getWidth() * rhs.getHeight());
                }
            }
        });
        
        // Find the best matching size
        for (Size size : sizeList) {
            if (isBigger) {
                if (size.getWidth() >= targetWidth && size.getHeight() >= targetHeight) {
                    return size;
                }
            } else {
                if (size.getWidth() <= targetWidth && size.getHeight() <= targetHeight) {
                    return size;
                }
            }
        }
        
        // If no perfect match, return the first one (largest or smallest depending on isBigger)
        return sizeList.get(0);
    }
}