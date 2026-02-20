package org.wysaid.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CaptureRequest;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.view.Surface;

import androidx.camera.camera2.interop.Camera2Interop;

import androidx.annotation.NonNull;
import androidx.camera.core.Camera;
import androidx.camera.core.CameraControl;
import androidx.camera.core.CameraInfo;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.FocusMeteringAction;
import androidx.camera.core.FocusMeteringResult;
import androidx.camera.core.ImageCapture;
import androidx.camera.core.ImageCaptureException;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.core.SurfaceRequest;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.LifecycleOwner;

import com.google.common.util.concurrent.ListenableFuture;

import org.wysaid.common.Common;

import java.nio.ByteBuffer;
import java.util.concurrent.ExecutionException;

/**
 * CameraX implementation of {@link ICameraProvider}.
 * Uses {@link ProcessCameraProvider} with lifecycle-aware camera management.
 *
 * <p>Requires CameraX dependencies to be present at runtime.
 * The frame output is directed to a caller-provided {@link SurfaceTexture} via
 * a custom {@link Preview.SurfaceProvider}, making it fully compatible with the
 * existing OpenGL rendering pipeline (OES texture → CGEFrameRecorder).
 *
 * <p>Usage:
 * <pre>
 *   CameraXProvider provider = new CameraXProvider(context);
 *   provider.attachLifecycleOwner(activity);
 *   provider.openCamera(CameraFacing.BACK, () -> { ... });
 *   provider.startPreview(surfaceTexture);
 * </pre>
 */
public class CameraXProvider implements ICameraProvider {

    private static final String LOG_TAG = Common.LOG_TAG;

    private final Context mContext;
    private LifecycleOwner mLifecycleOwner;
    private ProcessCameraProvider mCameraProvider;
    private Camera mCamera;
    private Preview mPreview;
    private ImageCapture mImageCapture;

    private CameraFacing mFacing = CameraFacing.BACK;
    private boolean mIsPreviewing = false;
    private SurfaceTexture mSurfaceTexture;
    private PreviewSizeReadyCallback mPreviewSizeReadyCallback;

    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private int mPreferredPreviewWidth = 640;
    private int mPreferredPreviewHeight = 640;
    private int mPictureWidth = 1000;
    private int mPictureHeight = 1000;
    private boolean mPictureSizeBigger = true;
    private FlashMode mFlashMode = FlashMode.OFF;

    public CameraXProvider(@NonNull Context context) {
        mContext = context.getApplicationContext();
    }

    // ========== Lifecycle ==========

    @Override
    public void attachLifecycleOwner(LifecycleOwner lifecycleOwner) {
        mLifecycleOwner = lifecycleOwner;
    }

    @Override
    public boolean openCamera(CameraFacing facing, CameraOpenCallback callback) {
        if (mLifecycleOwner == null) {
            Log.e(LOG_TAG, "CameraXProvider: LifecycleOwner not attached! Call attachLifecycleOwner() first.");
            return false;
        }

        mFacing = facing;

        ListenableFuture<ProcessCameraProvider> future = ProcessCameraProvider.getInstance(mContext);
        future.addListener(() -> {
            try {
                mCameraProvider = future.get();
                Log.i(LOG_TAG, "CameraX: ProcessCameraProvider obtained.");

                // Camera is now ready. Fire the callback so the view can call startPreview.
                // Do NOT call bindCamera here; the view controls when preview starts.
                if (callback != null) {
                    callback.cameraReady();
                }
            } catch (ExecutionException | InterruptedException e) {
                Log.e(LOG_TAG, "CameraX: Failed to get ProcessCameraProvider: " + e.toString());
            }
        }, ContextCompat.getMainExecutor(mContext));

        return true;
    }

    @Override
    public boolean needsManualRotation() {
        return false; // CameraX encodes rotation in the SurfaceTexture transform matrix
    }

    /**
     * Close the camera and release all CameraX use cases.
     *
     * <p><b>Must be called from the main thread.</b>
     * {@link ProcessCameraProvider#unbindAll()} is a main-thread-only CameraX API.
     * Callers on background threads (e.g., a GL thread) must dispatch to the main
     * thread before calling this method, for example:
     * <pre>
     *   new Handler(Looper.getMainLooper()).post(() -> provider.closeCamera());
     * </pre>
     *
     * @throws IllegalStateException if called from a non-main thread
     */
    @Override
    public void closeCamera() {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException(
                    "CameraXProvider.closeCamera() must be called on the main thread.");
        }
        mIsPreviewing = false;
        if (mCameraProvider != null) {
            mCameraProvider.unbindAll();
            mCamera = null;
        }
    }

    @Override
    public boolean isCameraOpened() {
        return mCamera != null;
    }

    // ========== Preview ==========

    @Override
    public void startPreview(SurfaceTexture texture) {
        startPreview(texture, null);
    }

    @Override
    public void startPreview(SurfaceTexture texture, PreviewSizeReadyCallback callback) {
        mSurfaceTexture = texture;
        mPreviewSizeReadyCallback = callback;
        // Ensure bindCamera always runs on the main thread.
        // When called from cameraReady() callback (openCamera path), we are already on the
        // main thread (MainExecutor). When called directly (resume path where mCameraProvider
        // is already set), we may be on the GL thread — post to main to be safe.
        if (Looper.myLooper() == Looper.getMainLooper()) {
            if (mCameraProvider != null && mLifecycleOwner != null) {
                bindCamera();
            }
        } else {
            new Handler(Looper.getMainLooper()).post(() -> {
                if (mCameraProvider != null && mLifecycleOwner != null) {
                    bindCamera();
                }
            });
        }
    }

    /**
     * Stop the camera preview. Same main-thread requirement as {@link #closeCamera()}.
     *
     * @throws IllegalStateException if called from a non-main thread
     */
    @Override
    public void stopPreview() {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException(
                    "CameraXProvider.stopPreview() must be called on the main thread.");
        }
        mIsPreviewing = false;
        if (mCameraProvider != null) {
            mCameraProvider.unbindAll();
            mCamera = null;
        }
    }

    @Override
    public boolean isPreviewing() {
        return mIsPreviewing;
    }

    @Override
    public int getPreviewWidth() {
        return mPreviewWidth;
    }

    @Override
    public int getPreviewHeight() {
        return mPreviewHeight;
    }

    @Override
    public void setPreferredPreviewSize(int width, int height) {
        mPreferredPreviewWidth = width;
        mPreferredPreviewHeight = height;
    }

    // ========== Camera Controls ==========

    @Override
    public CameraFacing getFacing() {
        return mFacing;
    }

    @Override
    public void focusAtPoint(float x, float y, float radius, AutoFocusCallback callback) {
        if (mCamera == null || mPreview == null) {
            Log.e(LOG_TAG, "CameraX: focusAtPoint called but camera not bound.");
            if (callback != null) callback.onAutoFocus(false);
            return;
        }

        CameraControl control = mCamera.getCameraControl();

        // Create a MeteringPointFactory from the Preview use case
        // x, y here are normalized [0,1] coordinates
        try {
            androidx.camera.core.MeteringPointFactory factory =
                    new androidx.camera.core.SurfaceOrientedMeteringPointFactory(
                            1.0f, 1.0f);
            androidx.camera.core.MeteringPoint point = factory.createPoint(x, y);
            FocusMeteringAction action = new FocusMeteringAction.Builder(point)
                    .build();

            ListenableFuture<FocusMeteringResult> result = control.startFocusAndMetering(action);
            result.addListener(() -> {
                try {
                    FocusMeteringResult focusResult = result.get();
                    if (callback != null) {
                        callback.onAutoFocus(focusResult.isFocusSuccessful());
                    }
                } catch (Exception e) {
                    Log.e(LOG_TAG, "CameraX: focusAtPoint failed: " + e.toString());
                    if (callback != null) callback.onAutoFocus(false);
                }
            }, ContextCompat.getMainExecutor(mContext));
        } catch (Exception e) {
            Log.e(LOG_TAG, "CameraX: focusAtPoint exception: " + e.toString());
            if (callback != null) callback.onAutoFocus(false);
        }
    }

    @Override
    public boolean setFlashMode(FlashMode mode) {
        mFlashMode = mode;

        if (mCamera == null) return false;

        try {
            CameraControl control = mCamera.getCameraControl();
            switch (mode) {
                case TORCH:
                    control.enableTorch(true);
                    break;
                case OFF:
                    control.enableTorch(false);
                    if (mImageCapture != null) {
                        mImageCapture.setFlashMode(ImageCapture.FLASH_MODE_OFF);
                    }
                    break;
                case ON:
                    control.enableTorch(false);
                    if (mImageCapture != null) {
                        mImageCapture.setFlashMode(ImageCapture.FLASH_MODE_ON);
                    }
                    break;
                case AUTO:
                    control.enableTorch(false);
                    if (mImageCapture != null) {
                        mImageCapture.setFlashMode(ImageCapture.FLASH_MODE_AUTO);
                    }
                    break;
                case RED_EYE:
                    // CameraX doesn't have RED_EYE mode, fall back to AUTO
                    control.enableTorch(false);
                    if (mImageCapture != null) {
                        mImageCapture.setFlashMode(ImageCapture.FLASH_MODE_AUTO);
                    }
                    break;
            }
            return true;
        } catch (Exception e) {
            Log.e(LOG_TAG, "CameraX: setFlashMode failed: " + e.toString());
            return false;
        }
    }

    @Override
    public FlashMode getFlashMode() {
        return mFlashMode;
    }

    @Override
    public void setPictureSize(int width, int height, boolean isBigger) {
        mPictureWidth = width;
        mPictureHeight = height;
        mPictureSizeBigger = isBigger;
        // Will take effect on next camera bind.
    }

    @Override
    public void setFocusMode(String focusMode) {
        // CameraX handles focus modes automatically.
        // "continuous-video" is the default behavior in CameraX.
        Log.i(LOG_TAG, "CameraX: setFocusMode ignored (CameraX manages focus automatically): " + focusMode);
    }

    // ========== Capture ==========

    @Override
    public void takePicture(ShutterCallback shutterCallback, PictureDataCallback pictureCallback) {
        if (mImageCapture == null) {
            Log.e(LOG_TAG, "CameraX: ImageCapture not initialized.");
            if (pictureCallback != null) {
                pictureCallback.onPictureTaken(null, mFacing, 0);
            }
            return;
        }

        if (shutterCallback != null) {
            // CameraX doesn't have an explicit shutter callback, call it immediately
            shutterCallback.onShutter();
        }

        mImageCapture.takePicture(ContextCompat.getMainExecutor(mContext),
                new ImageCapture.OnImageCapturedCallback() {
                    @Override
                    public void onCaptureSuccess(@NonNull ImageProxy image) {
                        // Convert ImageProxy to JPEG bytes
                        byte[] jpegData = imageProxyToJpeg(image);
                        int rotation = image.getImageInfo().getRotationDegrees();
                        image.close();

                        if (pictureCallback != null) {
                            pictureCallback.onPictureTaken(jpegData, mFacing, rotation);
                        }
                    }

                    @Override
                    public void onError(@NonNull ImageCaptureException exception) {
                        Log.e(LOG_TAG, "CameraX: takePicture failed: " + exception.toString());
                        if (pictureCallback != null) {
                            pictureCallback.onPictureTaken(null, mFacing, 0);
                        }
                    }
                });
    }

    @Override
    public void resumePreviewAfterCapture() {
        // CameraX does not stop preview after capture, so this is a no-op.
    }

    // ========== Internal ==========

    private void bindCamera() {
        if (mCameraProvider == null || mLifecycleOwner == null || mSurfaceTexture == null) {
            return;
        }

        try {
            mCameraProvider.unbindAll();

            CameraSelector cameraSelector = mFacing == CameraFacing.FRONT
                    ? CameraSelector.DEFAULT_FRONT_CAMERA
                    : CameraSelector.DEFAULT_BACK_CAMERA;

            // Build Preview use case with target resolution
            Preview.Builder previewBuilder = new Preview.Builder();
            previewBuilder.setTargetResolution(
                    new Size(mPreferredPreviewWidth, mPreferredPreviewHeight));

            // Request high frame rate (up to 60fps) to match Camera1 behavior.
            // Camera2Interop allows setting Camera2 capture-request options on CameraX use cases.
            // Using Range(30, 60) so the camera picks the best rate it supports.
            @SuppressWarnings("unchecked")
            Camera2Interop.Extender<Preview> extender = new Camera2Interop.Extender<>(previewBuilder);
            extender.setCaptureRequestOption(
                    CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, new Range<>(30, 60));

            mPreview = previewBuilder.build();

            // Set up the SurfaceProvider that directs frames to our SurfaceTexture
            mPreview.setSurfaceProvider(request -> {
                Size resolution = request.getResolution();
                mPreviewWidth = resolution.getWidth();
                mPreviewHeight = resolution.getHeight();

                Log.i(LOG_TAG, String.format("CameraX: Preview resolution: %d x %d",
                        mPreviewWidth, mPreviewHeight));

                // Set the buffer size on the SurfaceTexture
                mSurfaceTexture.setDefaultBufferSize(resolution.getWidth(), resolution.getHeight());

                Surface surface = new Surface(mSurfaceTexture);
                request.provideSurface(surface, ContextCompat.getMainExecutor(mContext), result -> {
                    surface.release();
                    Log.i(LOG_TAG, "CameraX: Surface released, result code: " + result.getResultCode());
                });

                // Notify the view now that the real preview size is known.
                if (mPreviewSizeReadyCallback != null) {
                    PreviewSizeReadyCallback cb = mPreviewSizeReadyCallback;
                    mPreviewSizeReadyCallback = null;
                    cb.onPreviewSizeReady(mPreviewWidth, mPreviewHeight);
                }
            });

            // Build ImageCapture use case
            ImageCapture.Builder captureBuilder = new ImageCapture.Builder();
            captureBuilder.setTargetResolution(new Size(mPictureWidth, mPictureHeight));

            // Apply current flash mode
            switch (mFlashMode) {
                case ON:
                    captureBuilder.setFlashMode(ImageCapture.FLASH_MODE_ON);
                    break;
                case AUTO:
                case RED_EYE:
                    captureBuilder.setFlashMode(ImageCapture.FLASH_MODE_AUTO);
                    break;
                default:
                    captureBuilder.setFlashMode(ImageCapture.FLASH_MODE_OFF);
                    break;
            }

            mImageCapture = captureBuilder.build();

            // Bind to lifecycle
            mCamera = mCameraProvider.bindToLifecycle(
                    mLifecycleOwner, cameraSelector, mPreview, mImageCapture);

            mIsPreviewing = true;
            Log.i(LOG_TAG, "CameraX: Camera bound successfully. Facing: " + mFacing);

        } catch (Exception e) {
            Log.e(LOG_TAG, "CameraX: bindCamera failed: " + e.toString());
            e.printStackTrace();
        }
    }

    /**
     * Convert an ImageProxy (YUV or JPEG) to JPEG byte array.
     */
    private byte[] imageProxyToJpeg(ImageProxy image) {
        if (image.getFormat() == android.graphics.ImageFormat.JPEG) {
            ByteBuffer buffer = image.getPlanes()[0].getBuffer();
            byte[] data = new byte[buffer.remaining()];
            buffer.get(data);
            return data;
        }

        // For YUV format, we need to convert — use Android's YuvImage
        try {
            android.graphics.YuvImage yuvImage = imageProxyToYuvImage(image);
            if (yuvImage == null) return null;

            java.io.ByteArrayOutputStream out = new java.io.ByteArrayOutputStream();
            yuvImage.compressToJpeg(
                    new android.graphics.Rect(0, 0, image.getWidth(), image.getHeight()),
                    95, out);
            return out.toByteArray();
        } catch (Exception e) {
            Log.e(LOG_TAG, "CameraX: imageProxyToJpeg conversion failed: " + e.toString());
            return null;
        }
    }

    /**
     * Convert ImageProxy (YUV_420_888) to YuvImage (NV21).
     */
    private android.graphics.YuvImage imageProxyToYuvImage(ImageProxy image) {
        if (image.getFormat() != android.graphics.ImageFormat.YUV_420_888) {
            Log.e(LOG_TAG, "CameraX: Unexpected image format: " + image.getFormat());
            return null;
        }

        ImageProxy.PlaneProxy[] planes = image.getPlanes();
        int width = image.getWidth();
        int height = image.getHeight();

        // Y plane
        ByteBuffer yBuffer = planes[0].getBuffer();
        // U plane
        ByteBuffer uBuffer = planes[1].getBuffer();
        // V plane
        ByteBuffer vBuffer = planes[2].getBuffer();

        int ySize = yBuffer.remaining();
        int uSize = uBuffer.remaining();
        int vSize = vBuffer.remaining();

        byte[] nv21 = new byte[ySize + width * height / 2];

        // Copy Y
        yBuffer.get(nv21, 0, ySize);

        // Interleave V and U (NV21 = YYYYVUVU)
        byte[] vData = new byte[vSize];
        byte[] uData = new byte[uSize];
        vBuffer.get(vData);
        uBuffer.get(uData);

        int uvIndex = ySize;
        int pixelStride = planes[1].getPixelStride();

        if (pixelStride == 2) {
            // Already interleaved (common case)
            // V plane in NV21 comes first, but the interleaved buffer from CameraX
            // may already be in the right order if the V plane starts first
            for (int i = 0; i < vSize && uvIndex < nv21.length; i += pixelStride) {
                nv21[uvIndex++] = vData[i];
                if (i < uSize && uvIndex < nv21.length) {
                    nv21[uvIndex++] = uData[i];
                }
            }
        } else {
            // Pixel stride == 1, need to manually interleave
            for (int i = 0; i < vSize && uvIndex < nv21.length; i++) {
                nv21[uvIndex++] = vData[i];
                if (i < uSize && uvIndex < nv21.length) {
                    nv21[uvIndex++] = uData[i];
                }
            }
        }

        return new android.graphics.YuvImage(nv21, android.graphics.ImageFormat.NV21, width, height, null);
    }
}
