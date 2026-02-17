package org.wysaid.camera;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Camera1 (android.hardware.Camera) implementation of {@link ICameraProvider}.
 *
 * <p>This is a pure delegation wrapper over the existing {@link CameraInstance} singleton.
 * No Camera1 logic is duplicated here — every call is forwarded to
 * {@link CameraInstance#getInstance()}. This ensures:
 * <ul>
 *   <li>Perfect backward compatibility with code that calls CameraInstance directly.</li>
 *   <li>The well-tested singleton lifecycle is preserved.</li>
 *   <li>New code can use the unified {@link ICameraProvider} interface transparently.</li>
 * </ul>
 *
 * <p>If a feature is missing from CameraInstance, add it in this class rather than
 * modifying CameraInstance, so that existing users pay zero upgrade cost.
 */
public class Camera1Provider implements ICameraProvider {

    private static final String LOG_TAG = Common.LOG_TAG;

    /**
     * Access the underlying CameraInstance singleton.
     * Prefer using {@link ICameraProvider} methods for new code.
     */
    public CameraInstance getCameraInstance() {
        return CameraInstance.getInstance();
    }

    // ========== Lifecycle ==========

    @Override
    public boolean openCamera(CameraFacing facing, CameraOpenCallback callback) {
        int camera1Facing = ICameraProvider.facingToCamera1(facing);
        CameraInstance.CameraOpenCallback legacyCallback = callback != null
                ? callback::cameraReady
                : null;
        return CameraInstance.getInstance().tryOpenCamera(legacyCallback, camera1Facing);
    }

    @Override
    public void closeCamera() {
        CameraInstance.getInstance().stopCamera();
    }

    @Override
    public boolean isCameraOpened() {
        return CameraInstance.getInstance().isCameraOpened();
    }

    // ========== Preview ==========

    @Override
    public void startPreview(SurfaceTexture texture) {
        CameraInstance.getInstance().startPreview(texture);
    }

    @Override
    public void stopPreview() {
        CameraInstance.getInstance().stopPreview();
    }

    @Override
    public boolean isPreviewing() {
        return CameraInstance.getInstance().isPreviewing();
    }

    @Override
    public int getPreviewWidth() {
        return CameraInstance.getInstance().previewWidth();
    }

    @Override
    public int getPreviewHeight() {
        return CameraInstance.getInstance().previewHeight();
    }

    @Override
    public void setPreferredPreviewSize(int width, int height) {
        CameraInstance.getInstance().setPreferPreviewSize(width, height);
    }

    // ========== Camera Controls ==========

    @Override
    public CameraFacing getFacing() {
        return ICameraProvider.camera1ToFacing(CameraInstance.getInstance().getFacing());
    }

    @Override
    public void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback) {
        Camera.AutoFocusCallback legacyCallback = callback != null
                ? (success, camera) -> callback.onAutoFocus(success)
                : null;
        CameraInstance.getInstance().focusAtPoint(x, y, radius, legacyCallback);
    }

    @Override
    public boolean setFlashMode(FlashMode mode) {
        Camera.Parameters params = CameraInstance.getInstance().getParams();
        if (params == null) return false;

        String camera1Mode = ICameraProvider.flashModeToCamera1(mode);
        try {
            java.util.List<String> supported = params.getSupportedFlashModes();
            if (supported == null || !supported.contains(camera1Mode)) {
                Log.e(LOG_TAG, "Flash mode not supported: " + camera1Mode);
                return false;
            }
            params.setFlashMode(camera1Mode);
            CameraInstance.getInstance().setParams(params);
            return true;
        } catch (Exception e) {
            Log.e(LOG_TAG, "Set flash mode failed: " + e.toString());
            return false;
        }
    }

    @Override
    public FlashMode getFlashMode() {
        Camera.Parameters params = CameraInstance.getInstance().getParams();
        if (params == null) return null;
        return ICameraProvider.camera1ToFlashMode(params.getFlashMode());
    }

    @Override
    public void setPictureSize(int width, int height, boolean isBigger) {
        CameraInstance.getInstance().setPictureSize(width, height, isBigger);
    }

    @Override
    public void setFocusMode(String focusMode) {
        CameraInstance.getInstance().setFocusMode(focusMode);
    }

    // ========== Capture ==========

    @Override
    public void takePicture(ShutterCallback shutterCallback, PictureDataCallback pictureCallback) {
        Camera cameraDevice = CameraInstance.getInstance().getCameraDevice();
        if (cameraDevice == null) {
            Log.e(LOG_TAG, "takePicture: camera device is null");
            if (pictureCallback != null) {
                pictureCallback.onPictureTaken(null, getFacing(), 0);
            }
            return;
        }

        // Set rotation
        Camera.Parameters params = CameraInstance.getInstance().getParams();
        if (params != null) {
            try {
                params.setRotation(90);
                CameraInstance.getInstance().setParams(params);
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
        Camera cameraDevice = CameraInstance.getInstance().getCameraDevice();
        if (cameraDevice != null) {
            cameraDevice.startPreview();
        }
    }
}
