package org.wysaid.camera;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Camera1 (android.hardware.Camera) implementation of {@link ICameraProvider}.
 * Wraps the legacy Camera1 API behind the unified provider interface.
 * Each instance manages its own Camera device — not a singleton.
 */
public class Camera1Provider implements ICameraProvider {

    private static final String LOG_TAG = Common.LOG_TAG;

    private final CameraInstance mCameraInstance;

    public Camera1Provider() {
        mCameraInstance = new CameraInstance();
    }

    /**
     * Access the underlying CameraInstance for advanced/legacy usage.
     *
     * @deprecated Prefer using ICameraProvider methods instead.
     */
    @Deprecated
    public CameraInstance getCameraInstance() {
        return mCameraInstance;
    }

    // ========== Lifecycle ==========

    @Override
    public boolean openCamera(CameraFacing facing, CameraOpenCallback callback) {
        int camera1Facing = ICameraProvider.facingToCamera1(facing);
        CameraInstance.CameraOpenCallback legacyCallback = callback != null
                ? callback::cameraReady
                : null;
        return mCameraInstance.tryOpenCamera(legacyCallback, camera1Facing);
    }

    @Override
    public void closeCamera() {
        mCameraInstance.stopCamera();
    }

    @Override
    public boolean isCameraOpened() {
        return mCameraInstance.isCameraOpened();
    }

    // ========== Preview ==========

    @Override
    public void startPreview(SurfaceTexture texture) {
        mCameraInstance.startPreview(texture);
    }

    @Override
    public void stopPreview() {
        mCameraInstance.stopPreview();
    }

    @Override
    public boolean isPreviewing() {
        return mCameraInstance.isPreviewing();
    }

    @Override
    public int getPreviewWidth() {
        return mCameraInstance.previewWidth();
    }

    @Override
    public int getPreviewHeight() {
        return mCameraInstance.previewHeight();
    }

    @Override
    public void setPreferredPreviewSize(int width, int height) {
        mCameraInstance.setPreferPreviewSize(width, height);
    }

    // ========== Camera Controls ==========

    @Override
    public CameraFacing getFacing() {
        return ICameraProvider.camera1ToFacing(mCameraInstance.getFacing());
    }

    @Override
    public void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback) {
        Camera.AutoFocusCallback legacyCallback = callback != null
                ? (success, camera) -> callback.onAutoFocus(success)
                : null;
        mCameraInstance.focusAtPoint(x, y, radius, legacyCallback);
    }

    @Override
    public boolean setFlashMode(FlashMode mode) {
        Camera.Parameters params = mCameraInstance.getParams();
        if (params == null) return false;

        String camera1Mode = ICameraProvider.flashModeToCamera1(mode);
        try {
            java.util.List<String> supported = params.getSupportedFlashModes();
            if (supported == null || !supported.contains(camera1Mode)) {
                Log.e(LOG_TAG, "Flash mode not supported: " + camera1Mode);
                return false;
            }
            params.setFlashMode(camera1Mode);
            mCameraInstance.setParams(params);
            return true;
        } catch (Exception e) {
            Log.e(LOG_TAG, "Set flash mode failed: " + e.toString());
            return false;
        }
    }

    @Override
    public FlashMode getFlashMode() {
        Camera.Parameters params = mCameraInstance.getParams();
        if (params == null) return null;
        return ICameraProvider.camera1ToFlashMode(params.getFlashMode());
    }

    @Override
    public void setPictureSize(int width, int height, boolean isBigger) {
        mCameraInstance.setPictureSize(width, height, isBigger);
    }

    @Override
    public void setFocusMode(String focusMode) {
        mCameraInstance.setFocusMode(focusMode);
    }

    // ========== Capture ==========

    @Override
    public void takePicture(ShutterCallback shutterCallback, PictureDataCallback pictureCallback) {
        Camera cameraDevice = mCameraInstance.getCameraDevice();
        if (cameraDevice == null) {
            Log.e(LOG_TAG, "takePicture: camera device is null");
            if (pictureCallback != null) {
                pictureCallback.onPictureTaken(null, getFacing(), 0);
            }
            return;
        }

        // Set rotation
        Camera.Parameters params = mCameraInstance.getParams();
        if (params != null) {
            try {
                params.setRotation(90);
                mCameraInstance.setParams(params);
            } catch (Exception e) {
                Log.e(LOG_TAG, "Error setting rotation: " + e.toString());
            }
        }

        Camera.ShutterCallback legacyShutter = shutterCallback != null
                ? shutterCallback::onShutter
                : null;

        CameraFacing currentFacing = getFacing();
        cameraDevice.takePicture(legacyShutter, null, (data, camera) -> {
            if (pictureCallback != null) {
                pictureCallback.onPictureTaken(data, currentFacing, 90);
            }
        });
    }

    @Override
    public void resumePreviewAfterCapture() {
        Camera cameraDevice = mCameraInstance.getCameraDevice();
        if (cameraDevice != null) {
            cameraDevice.startPreview();
        }
    }
}
