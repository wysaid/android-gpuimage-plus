package org.wysaid.camera;

import android.content.Context;
import android.os.Build;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Factory class for creating camera backends
 * Handles selection between legacy Camera API and Camera2 API
 */
public class CameraBackendFactory {
    
    public static final String LOG_TAG = Common.LOG_TAG;
    
    public enum CameraBackendType {
        LEGACY,    // Use legacy Camera API
        CAMERA2,   // Use Camera2 API
        AUTO       // Automatically choose based on API level and availability
    }
    
    private static CameraBackendType sSelectedBackendType = CameraBackendType.LEGACY; // Default to legacy for compatibility
    
    /**
     * Set the preferred camera backend type
     * @param backendType The backend type to use
     */
    public static void setPreferredBackendType(CameraBackendType backendType) {
        sSelectedBackendType = backendType;
        Log.i(LOG_TAG, "Camera backend set to: " + backendType);
    }
    
    /**
     * Get the currently selected backend type
     * @return The current backend type
     */
    public static CameraBackendType getSelectedBackendType() {
        return sSelectedBackendType;
    }
    
    /**
     * Create a camera backend instance
     * @param context The application context (required for Camera2)
     * @return A camera backend instance
     */
    public static CameraBackend createCameraBackend(Context context) {
        CameraBackendType actualType = resolveBackendType();
        
        switch (actualType) {
            case CAMERA2:
                if (isCamera2Supported()) {
                    Log.i(LOG_TAG, "Creating Camera2 backend");
                    return new CameraBackend2(context.getApplicationContext());
                } else {
                    Log.w(LOG_TAG, "Camera2 not supported, falling back to legacy");
                    return new CameraBackendLegacy();
                }
                
            case LEGACY:
            default:
                Log.i(LOG_TAG, "Creating Legacy Camera backend");
                return new CameraBackendLegacy();
        }
    }
    
    /**
     * Check if Camera2 API is supported on this device
     * @return true if Camera2 is supported
     */
    public static boolean isCamera2Supported() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP;
    }
    
    /**
     * Check if Camera2 API should be used based on current settings
     * @return true if Camera2 should be used
     */
    public static boolean shouldUseCamera2() {
        CameraBackendType actualType = resolveBackendType();
        return actualType == CameraBackendType.CAMERA2 && isCamera2Supported();
    }
    
    /**
     * Resolve the actual backend type to use based on settings and device capabilities
     * @return The resolved backend type
     */
    private static CameraBackendType resolveBackendType() {
        switch (sSelectedBackendType) {
            case AUTO:
                // For auto mode, prefer Camera2 if available, otherwise use legacy
                return isCamera2Supported() ? CameraBackendType.CAMERA2 : CameraBackendType.LEGACY;
                
            case CAMERA2:
                // User explicitly wants Camera2, but we'll check if it's supported
                return sSelectedBackendType;
                
            case LEGACY:
            default:
                // User wants legacy or unknown type
                return CameraBackendType.LEGACY;
        }
    }
    
    /**
     * Get a human-readable description of the current backend configuration
     * @return Description string
     */
    public static String getBackendDescription() {
        CameraBackendType actualType = resolveBackendType();
        String base = "Selected: " + sSelectedBackendType + ", Using: " + actualType;
        
        if (actualType == CameraBackendType.CAMERA2 && !isCamera2Supported()) {
            base += " (but Camera2 not supported, will use Legacy)";
        }
        
        return base;
    }
}