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
import androidx.camera.camera2.interop.ExperimentalCamera2Interop;

import androidx.annotation.NonNull;
import androidx.camera.core.Camera;
import androidx.camera.core.CameraControl;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.FocusMeteringAction;
import androidx.camera.core.FocusMeteringResult;
import androidx.camera.core.ImageCapture;
import androidx.camera.core.ImageCaptureException;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.core.resolutionselector.ResolutionSelector;
import androidx.camera.core.resolutionselector.ResolutionStrategy;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.LifecycleOwner;

import com.google.common.util.concurrent.ListenableFuture;

import org.wysaid.common.Common;

import java.nio.ByteBuffer;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

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

    /** Single-thread executor used to process captured images off the main thread. */
    private final ExecutorService mCaptureExecutor = Executors.newSingleThreadExecutor();

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
            } catch (ExecutionException e) {
                Log.e(LOG_TAG, "CameraX: Failed to get ProcessCameraProvider: " + e.toString());
            } catch (InterruptedException e) {
                Log.e(LOG_TAG, "CameraX: Failed to get ProcessCameraProvider: " + e.toString());
                Thread.currentThread().interrupt();
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
     * <p>If called from a non-main thread the work is automatically posted to
     * the main thread, because {@link ProcessCameraProvider#unbindAll()} is a
     * main-thread-only CameraX API.
     */
    @Override
    public void closeCamera() {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            new Handler(Looper.getMainLooper()).post(this::closeCamera);
            return;
        }
        mIsPreviewing = false;
        if (mCameraProvider != null) {
            mCameraProvider.unbindAll();
            mCamera = null;
            mPreview = null;
            mImageCapture = null;
        }
        mCaptureExecutor.shutdown();
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
     * Stop the camera preview.
     *
     * <p>If called from a non-main thread the work is automatically posted to
     * the main thread.
     */
    @Override
    public void stopPreview() {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            new Handler(Looper.getMainLooper()).post(this::stopPreview);
            return;
        }
        mIsPreviewing = false;
        if (mCameraProvider != null) {
            mCameraProvider.unbindAll();
            mCamera = null;
            mPreview = null;
            mImageCapture = null;
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

        // Create a MeteringPointFactory from the Preview use case.
        // The 3-arg constructor accepts the bound Preview so that screen
        // coordinates are correctly mapped regardless of sensor orientation.
        // x, y here are normalized [0,1] coordinates.
        try {
            androidx.camera.core.MeteringPointFactory factory =
                    new androidx.camera.core.SurfaceOrientedMeteringPointFactory(
                            1.0f, 1.0f, mPreview);
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

        mImageCapture.takePicture(mCaptureExecutor,
                new ImageCapture.OnImageCapturedCallback() {
                    @Override
                    public void onCaptureSuccess(@NonNull ImageProxy image) {
                        // Convert ImageProxy to JPEG bytes on the background thread.
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

    @androidx.annotation.OptIn(markerClass = ExperimentalCamera2Interop.class)
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
            ResolutionSelector previewResolutionSelector = new ResolutionSelector.Builder()
                    .setResolutionStrategy(new ResolutionStrategy(
                            new Size(mPreferredPreviewWidth, mPreferredPreviewHeight),
                            ResolutionStrategy.FALLBACK_RULE_CLOSEST_HIGHER_THEN_LOWER))
                    .build();
            previewBuilder.setResolutionSelector(previewResolutionSelector);

            // Target 30fps by default for consistent performance and battery efficiency.
            // Camera2Interop allows setting Camera2 capture-request options on CameraX use cases.
            // Using Range(30, 30) to lock to 30fps; adjust both bounds to allow a higher cap if needed.
            Camera2Interop.Extender<Preview> extender = new Camera2Interop.Extender<>(previewBuilder);
            extender.setCaptureRequestOption(
                    CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, new Range<>(30, 30));

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
            int captureFallbackRule = mPictureSizeBigger
                    ? ResolutionStrategy.FALLBACK_RULE_CLOSEST_HIGHER_THEN_LOWER
                    : ResolutionStrategy.FALLBACK_RULE_CLOSEST_LOWER_THEN_HIGHER;
            ResolutionSelector captureResolutionSelector = new ResolutionSelector.Builder()
                    .setResolutionStrategy(new ResolutionStrategy(
                            new Size(mPictureWidth, mPictureHeight),
                            captureFallbackRule))
                    .build();
            captureBuilder.setResolutionSelector(captureResolutionSelector);

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
     *
     * <p>Correctly handles the case where a plane's {@code rowStride} is wider than the image
     * width (padding rows), by copying exactly {@code width} bytes per row for the Y plane and
     * using {@code rowStride}/{@code pixelStride} offsets for the chroma planes.
     */
    private android.graphics.YuvImage imageProxyToYuvImage(ImageProxy image) {
        if (image.getFormat() != android.graphics.ImageFormat.YUV_420_888) {
            Log.e(LOG_TAG, "CameraX: Unexpected image format: " + image.getFormat());
            return null;
        }

        ImageProxy.PlaneProxy[] planes = image.getPlanes();
        int width = image.getWidth();
        int height = image.getHeight();

        int yRowStride = planes[0].getRowStride();
        int chromaRowStride = planes[1].getRowStride();
        int chromaPixelStride = planes[1].getPixelStride();

        ByteBuffer yBuffer = planes[0].getBuffer();
        ByteBuffer uBuffer = planes[1].getBuffer();
        ByteBuffer vBuffer = planes[2].getBuffer();

        byte[] nv21 = new byte[width * height * 3 / 2];

        // Copy Y plane row-by-row to strip any row-stride padding.
        for (int row = 0; row < height; row++) {
            yBuffer.position(row * yRowStride);
            yBuffer.get(nv21, row * width, width);
        }

        // Read the full V and U plane buffers (may contain padding).
        int vSize = vBuffer.remaining();
        int uSize = uBuffer.remaining();
        byte[] vData = new byte[vSize];
        byte[] uData = new byte[uSize];
        vBuffer.rewind();
        vBuffer.get(vData);
        uBuffer.rewind();
        uBuffer.get(uData);

        // Interleave V then U into NV21, using per-pixel and per-row strides to skip padding.
        int uvIndex = width * height;
        for (int row = 0; row < height / 2; row++) {
            for (int col = 0; col < width / 2; col++) {
                int srcOffset = row * chromaRowStride + col * chromaPixelStride;
                if (srcOffset < vSize && uvIndex < nv21.length) {
                    nv21[uvIndex++] = vData[srcOffset];
                }
                if (srcOffset < uSize && uvIndex < nv21.length) {
                    nv21[uvIndex++] = uData[srcOffset];
                }
            }
        }

        return new android.graphics.YuvImage(nv21, android.graphics.ImageFormat.NV21, width, height, null);
    }
}
