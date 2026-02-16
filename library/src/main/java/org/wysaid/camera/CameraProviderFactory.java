package org.wysaid.camera;

import android.content.Context;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Factory for creating {@link ICameraProvider} instances.
 * Supports runtime selection between Camera1 and CameraX backends.
 */
public class CameraProviderFactory {

    private static final String LOG_TAG = Common.LOG_TAG;

    /** Enumeration of supported camera API backends. */
    public enum CameraAPI {
        /** Legacy android.hardware.Camera API. Always available. */
        CAMERA1,
        /** AndroidX CameraX API. */
        CAMERAX
    }

    /**
     * Create a camera provider for the specified API backend.
     *
     * @param api     The camera API to use.
     * @param context Android context (required for CameraX; may be null for Camera1).
     * @return A new ICameraProvider instance.
     */
    public static ICameraProvider create(CameraAPI api, Context context) {
        switch (api) {
            case CAMERAX:
                if (context == null) {
                    throw new IllegalArgumentException("Context is required for CameraX provider.");
                }
                return new CameraXProvider(context);

            case CAMERA1:
            default:
                return new Camera1Provider();
        }
    }

    /**
     * Create a Camera1 provider (convenience method, no context needed).
     */
    public static ICameraProvider createCamera1() {
        return new Camera1Provider();
    }
}
