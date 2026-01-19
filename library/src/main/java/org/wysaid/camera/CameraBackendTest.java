package org.wysaid.camera;

import android.content.Context;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Simple test class to validate camera backend functionality
 */
public class CameraBackendTest {
    
    private static final String LOG_TAG = Common.LOG_TAG;
    
    /**
     * Test camera backend creation and basic functionality
     */
    public static void testCameraBackends(Context context) {
        Log.i(LOG_TAG, "=== Camera Backend Test ===");
        
        // Test backend availability
        Log.i(LOG_TAG, "Camera2 supported: " + CameraBackendFactory.isCamera2Supported());
        Log.i(LOG_TAG, "Current backend type: " + CameraBackendFactory.getSelectedBackendType());
        Log.i(LOG_TAG, "Backend description: " + CameraBackendFactory.getBackendDescription());
        
        // Test legacy backend
        Log.i(LOG_TAG, "Testing Legacy backend:");
        CameraBackendFactory.setPreferredBackendType(CameraBackendFactory.CameraBackendType.LEGACY);
        CameraBackend legacyBackend = CameraBackendFactory.createCameraBackend(context);
        Log.i(LOG_TAG, "Legacy backend created: " + legacyBackend.getBackendType());
        
        // Test Camera2 backend if supported
        if (CameraBackendFactory.isCamera2Supported()) {
            Log.i(LOG_TAG, "Testing Camera2 backend:");
            CameraBackendFactory.setPreferredBackendType(CameraBackendFactory.CameraBackendType.CAMERA2);
            CameraBackend camera2Backend = CameraBackendFactory.createCameraBackend(context);
            Log.i(LOG_TAG, "Camera2 backend created: " + camera2Backend.getBackendType());
        } else {
            Log.i(LOG_TAG, "Camera2 backend not supported on this device");
        }
        
        // Test auto mode
        Log.i(LOG_TAG, "Testing Auto backend selection:");
        CameraBackendFactory.setPreferredBackendType(CameraBackendFactory.CameraBackendType.AUTO);
        CameraBackend autoBackend = CameraBackendFactory.createCameraBackend(context);
        Log.i(LOG_TAG, "Auto backend created: " + autoBackend.getBackendType());
        
        Log.i(LOG_TAG, "=== Camera Backend Test Complete ===");
    }
    
    /**
     * Get runtime information about camera backend
     */
    public static String getRuntimeInfo(Context context) {
        StringBuilder info = new StringBuilder();
        info.append("Camera Backend Runtime Information:\n");
        info.append("- Device API Level: ").append(android.os.Build.VERSION.SDK_INT).append("\n");
        info.append("- Camera2 Supported: ").append(CameraBackendFactory.isCamera2Supported()).append("\n");
        info.append("- Selected Backend Type: ").append(CameraBackendFactory.getSelectedBackendType()).append("\n");
        info.append("- Backend Description: ").append(CameraBackendFactory.getBackendDescription()).append("\n");
        
        // Create a test backend to get its type
        try {
            CameraBackend testBackend = CameraBackendFactory.createCameraBackend(context);
            info.append("- Active Backend: ").append(testBackend.getBackendType()).append("\n");
        } catch (Exception e) {
            info.append("- Error creating backend: ").append(e.getMessage()).append("\n");
        }
        
        return info.toString();
    }
}