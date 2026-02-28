package org.wysaid.camera;

import android.graphics.SurfaceTexture;

import androidx.lifecycle.LifecycleOwner;

/**
 * Abstraction layer for camera operations. Implementations can use Camera1 API or CameraX.
 * All camera-specific types (such as android.hardware.Camera) are hidden behind this interface.
 *
 * <p>Usage:
 * <pre>
 *   ICameraProvider provider = CameraProviderFactory.create(CameraAPI.CAMERA1);
 *   provider.openCamera(CameraFacing.BACK, () -> { ... });
 *   provider.startPreview(surfaceTexture);
 * </pre>
 */
public interface ICameraProvider {

    // ========== Enums ==========

    /** Camera facing direction, replacing Camera.CameraInfo.CAMERA_FACING_* constants. */
    enum CameraFacing {
        BACK,
        FRONT
    }

    /** Flash mode, replacing Camera.Parameters.FLASH_MODE_* constants. */
    enum FlashMode {
        OFF,
        ON,
        AUTO,
        TORCH,
        RED_EYE
    }

    // ========== Callbacks ==========

    /** Callback notified when the camera is ready to use. */
    interface CameraOpenCallback {
        void cameraReady();
    }

    /** Callback for auto-focus result, replacing Camera.AutoFocusCallback. */
    interface AutoFocusCallback {
        void onAutoFocus(boolean success);
    }

    /** Callback for shutter event, replacing Camera.ShutterCallback. */
    interface ShutterCallback {
        void onShutter();
    }

    /** Callback for receiving captured picture data. */
    interface PictureDataCallback {
        /**
         * Called when picture data is available.
         *
         * @param jpegData   JPEG-encoded picture data, or null on failure.
         * @param facing     The camera facing when the picture was taken.
         * @param rotation   Rotation to apply for correct display (degrees, 0/90/180/270).
         */
        void onPictureTaken(byte[] jpegData, CameraFacing facing, int rotation);
    }

    // ========== Lifecycle ==========

    /**
     * Open the camera with the specified facing direction.
     *
     * @param facing   Which camera to open.
     * @param callback Called when the camera is ready; may be null.
     * @return true if the camera was opened successfully.
     */
    boolean openCamera(CameraFacing facing, CameraOpenCallback callback);

    /**
     * Open the default back-facing camera.
     */
    default boolean openCamera(CameraOpenCallback callback) {
        return openCamera(CameraFacing.BACK, callback);
    }

    /**
     * Close the camera and release all resources.
     */
    void closeCamera();

    /**
     * Whether the camera is currently opened.
     */
    boolean isCameraOpened();

    // ========== Preview ==========

    /**
     * Callback notified when the actual preview size is known.
     * For synchronous providers (Camera1) this may fire before startPreview returns;
     * for async providers (CameraX) it fires once the Surface is bound.
     */
    interface PreviewSizeReadyCallback {
        /** @param width  actual preview width  (sensor orientation, landscape)
         *  @param height actual preview height (sensor orientation, landscape) */
        void onPreviewSizeReady(int width, int height);
    }

    /**
     * Start preview, rendering frames to the given SurfaceTexture.
     *
     * @param texture The SurfaceTexture to receive camera frames (as an OES external texture).
     */
    void startPreview(SurfaceTexture texture);

    /**
     * Start preview with an optional callback to be notified once the actual
     * preview resolution is determined (important for async backends like CameraX).
     *
     * <p>The default implementation calls {@link #startPreview(SurfaceTexture)} and then
     * immediately fires the callback using the values from {@link #getPreviewWidth()} /
     * {@link #getPreviewHeight()} — suitable for synchronous Camera1 backend.
     *
     * <p>Async backends (CameraX) should override this to fire the callback <em>after</em>
     * the real resolution is available.
     *
     * @param texture  The SurfaceTexture to receive camera frames.
     * @param callback Called (on any thread) once width/height are known; may be null.
     */
    default void startPreview(SurfaceTexture texture, PreviewSizeReadyCallback callback) {
        startPreview(texture);
        if (callback != null) {
            callback.onPreviewSizeReady(getPreviewWidth(), getPreviewHeight());
        }
    }

    /**
     * Stop the current preview.
     */
    void stopPreview();

    /**
     * Whether the camera is currently previewing.
     */
    boolean isPreviewing();

    /**
     * Get the actual preview width (in sensor orientation, typically landscape).
     */
    int getPreviewWidth();

    /**
     * Get the actual preview height (in sensor orientation, typically landscape).
     */
    int getPreviewHeight();

    /**
     * Set the preferred preview size. The actual preview size will be the closest supported size.
     *
     * @param width  Preferred width.
     * @param height Preferred height.
     */
    void setPreferredPreviewSize(int width, int height);

    // ========== Camera Controls ==========

    /**
     * Get the current camera facing direction.
     */
    CameraFacing getFacing();

    /**
     * Auto-focus at the specified point.
     *
     * @param x        Normalized x coordinate [0, 1].
     * @param y        Normalized y coordinate [0, 1].
     * @param radius   Focus area radius (0 to 1).
     * @param callback Focus result callback; may be null.
     */
    void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback);

    /**
     * Set the flash mode.
     *
     * @param mode The desired flash mode.
     * @return true if the mode was set successfully.
     */
    boolean setFlashMode(FlashMode mode);

    /**
     * Get the current flash mode. Returns null if not available.
     */
    FlashMode getFlashMode();

    /**
     * Set the picture (capture) size. The actual size will be the closest supported size.
     *
     * @param width    Preferred width.
     * @param height   Preferred height.
     * @param isBigger If true, prefer the nearest larger size; otherwise prefer the nearest smaller.
     */
    void setPictureSize(int width, int height, boolean isBigger);

    /**
     * Set the focus mode by name (e.g., "continuous-video").
     * Implementations should ignore unsupported modes gracefully.
     */
    void setFocusMode(String focusMode);

    // ========== Zoom ==========

    /**
     * Whether the camera hardware supports zoom.
     *
     * <p>CameraX always returns {@code true} (handled by the framework).
     * Camera1 returns the result of {@link android.hardware.Camera.Parameters#isZoomSupported()}.
     *
     * @return {@code true} if zoom is supported, {@code false} otherwise.
     */
    default boolean isZoomSupported() {
        return false;
    }

    /**
     * Get the minimum zoom ratio supported by the camera hardware.
     * Always 1.0 (no zoom, native field of view).
     *
     * @return minimum zoom ratio, typically 1.0f.
     */
    default float getMinZoomRatio() {
        return 1.0f;
    }

    /**
     * Get the maximum zoom ratio supported by the camera hardware.
     *
     * @return maximum zoom ratio (e.g. 10.0f); 1.0f when zoom is unsupported.
     */
    default float getMaxZoomRatio() {
        return 1.0f;
    }

    /**
     * Set the zoom ratio.
     *
     * <p>CameraX applies the ratio asynchronously via {@code CameraControl.setZoomRatio()}.
     * Camera1 maps the float ratio to the closest supported integer zoom index
     * ({@link android.hardware.Camera.Parameters#setZoom(int)}) — slight rounding may occur
     * because Camera1 only exposes discrete steps.
     *
     * <p>Values are clamped to [{@link #getMinZoomRatio()}, {@link #getMaxZoomRatio()}].
     * Calling this method when {@link #isZoomSupported()} returns {@code false} is a no-op.
     *
     * @param ratio The desired zoom ratio. 1.0f means no zoom.
     */
    default void setZoomRatio(float ratio) {
        // Default no-op; implementations override.
    }

    // ========== Capture ==========

    /**
     * Take a full-resolution picture.
     *
     * @param shutterCallback Called at the moment of capture; may be null.
     * @param pictureCallback Called with the JPEG data; must not be null.
     */
    void takePicture(ShutterCallback shutterCallback, PictureDataCallback pictureCallback);

    /**
     * Resume preview after takePicture (some implementations stop preview after capture).
     */
    void resumePreviewAfterCapture();

    // ========== CameraX Lifecycle (optional) ==========

    /**
     * Attach a LifecycleOwner for lifecycle-aware camera providers (e.g., CameraX).
     * Camera1Provider may ignore this call. Should be called before openCamera.
     *
     * @param lifecycleOwner The lifecycle owner (typically an Activity or Fragment).
     */
    default void attachLifecycleOwner(LifecycleOwner lifecycleOwner) {
        // Default no-op for Camera1
    }

    /**
     * Whether the rendering pipeline needs to apply a manual 90° rotation.
     *
     * <p>Camera1 sensors deliver frames in landscape orientation and rely on the app to
     * rotate the render by PI/2. CameraX, by contrast, encodes the necessary rotation
     * inside the transform matrix returned by {@link android.graphics.SurfaceTexture#getTransformMatrix},
     * so an additional manual rotation would double-rotate the image.
     *
     * @return {@code true}  for Camera1 (manual rotation required),
     *         {@code false} for CameraX (transform matrix already rotated).
     */
    default boolean needsManualRotation() {
        return true; // Camera1 default
    }

    // ========== Utility ==========

    /**
     * Convert a {@link CameraFacing} enum value to the legacy Camera1 facing constant.
     */
    static int facingToCamera1(CameraFacing facing) {
        return facing == CameraFacing.FRONT
                ? android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT
                : android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK;
    }

    /**
     * Convert a legacy Camera1 facing constant to {@link CameraFacing}.
     */
    static CameraFacing camera1ToFacing(int camera1Facing) {
        return camera1Facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT
                ? CameraFacing.FRONT
                : CameraFacing.BACK;
    }

    /**
     * Convert a {@link FlashMode} to the legacy Camera1 flash mode string.
     */
    static String flashModeToCamera1(FlashMode mode) {
        if (mode == null) return null;
        switch (mode) {
            case ON:      return android.hardware.Camera.Parameters.FLASH_MODE_ON;
            case AUTO:    return android.hardware.Camera.Parameters.FLASH_MODE_AUTO;
            case TORCH:   return android.hardware.Camera.Parameters.FLASH_MODE_TORCH;
            case RED_EYE: return android.hardware.Camera.Parameters.FLASH_MODE_RED_EYE;
            case OFF:
            default:      return android.hardware.Camera.Parameters.FLASH_MODE_OFF;
        }
    }

    /**
     * Convert a Camera1 flash mode string to {@link FlashMode}.
     */
    static FlashMode camera1ToFlashMode(String mode) {
        if (mode == null) return FlashMode.OFF;
        switch (mode) {
            case android.hardware.Camera.Parameters.FLASH_MODE_ON:      return FlashMode.ON;
            case android.hardware.Camera.Parameters.FLASH_MODE_AUTO:    return FlashMode.AUTO;
            case android.hardware.Camera.Parameters.FLASH_MODE_TORCH:   return FlashMode.TORCH;
            case android.hardware.Camera.Parameters.FLASH_MODE_RED_EYE: return FlashMode.RED_EYE;
            default: return FlashMode.OFF;
        }
    }
}
