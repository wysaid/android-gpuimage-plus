package org.wysaid.camera;

import android.content.Context;
import android.util.Log;

import org.wysaid.common.Common;

import java.lang.reflect.Constructor;

/**
 * Factory for creating {@link ICameraProvider} instances.
 * Supports runtime selection between Camera1 and CameraX backends.
 *
 * <p>CameraX is loaded via reflection so the library can ship without a hard
 * runtime dependency on {@code androidx.camera.*}.  Consumers that want CameraX
 * must add the CameraX artifacts as {@code implementation} dependencies in
 * their own Gradle build file.
 */
public class CameraProviderFactory {

    private static final String LOG_TAG = Common.LOG_TAG;

    /** Fully-qualified class name of the CameraX provider implementation. */
    private static final String CAMERAX_PROVIDER_CLASS =
            "org.wysaid.camera.CameraXProvider";

    /** Cached result of the CameraX availability probe ({@code null} = not yet checked). */
    private static volatile Boolean sCameraXAvailable;

    /** Enumeration of supported camera API backends. */
    public enum CameraAPI {
        /** Legacy android.hardware.Camera API. Always available. */
        CAMERA1,
        /** AndroidX CameraX API. Requires CameraX dependencies at runtime. */
        CAMERAX
    }

    /**
     * Check whether the CameraX runtime classes are available on the classpath.
     *
     * @return {@code true} if CameraX can be instantiated at runtime.
     */
    public static boolean isCameraXAvailable() {
        if (sCameraXAvailable == null) {
            try {
                Class.forName(CAMERAX_PROVIDER_CLASS);
                sCameraXAvailable = true;
            } catch (ClassNotFoundException e) {
                sCameraXAvailable = false;
            }
        }
        return sCameraXAvailable;
    }

    /**
     * Create a camera provider for the specified API backend.
     *
     * <p>If {@link CameraAPI#CAMERAX} is requested but the CameraX classes are
     * not available at runtime, an {@link IllegalStateException} is thrown.
     *
     * @param api     The camera API to use.
     * @param context Android context (required for CameraX; may be null for Camera1).
     * @return A new ICameraProvider instance.
     * @throws IllegalStateException    if CameraX is requested but unavailable.
     * @throws IllegalArgumentException if context is null when CameraX is requested.
     */
    public static ICameraProvider create(CameraAPI api, Context context) {
        switch (api) {
            case CAMERAX:
                if (context == null) {
                    throw new IllegalArgumentException("Context is required for CameraX provider.");
                }
                return createCameraXByReflection(context);

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

    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    private static ICameraProvider createCameraXByReflection(Context context) {
        if (!isCameraXAvailable()) {
            throw new IllegalStateException(
                    "CameraX is not available. Add the CameraX dependencies to your build.gradle.");
        }
        try {
            Class<?> clazz = Class.forName(CAMERAX_PROVIDER_CLASS);
            Constructor<?> ctor = clazz.getConstructor(Context.class);
            return (ICameraProvider) ctor.newInstance(context);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Failed to create CameraXProvider via reflection: " + e);
            throw new IllegalStateException("Could not instantiate CameraXProvider.", e);
        }
    }
}
