package org.wysaid.myUtils;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import org.wysaid.common.Common;

import java.util.Vector;

public class PermissionUtil {
    private static final int REQUEST_PERMISSION = 0;

    /**
     * Returns the set of permissions appropriate for the current Android version.
     * <ul>
     *   <li>API 33+ (Android 13): Use fine-grained media permissions instead of the
     *       deprecated READ_EXTERNAL_STORAGE, and drop WRITE_EXTERNAL_STORAGE which
     *       has been a no-op since API 29.</li>
     *   <li>API 29–32 (Android 10–12L): WRITE_EXTERNAL_STORAGE is a no-op; only
     *       READ_EXTERNAL_STORAGE is still meaningful.</li>
     *   <li>API ≤ 28 (Android 9 and below): Both storage permissions still apply.</li>
     * </ul>
     */
    private static String[] getRequiredPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            // Android 13+: READ_MEDIA_IMAGES / READ_MEDIA_VIDEO replace READ_EXTERNAL_STORAGE.
            // WRITE_EXTERNAL_STORAGE has been a no-op since Android 10 — omit entirely.
            return new String[]{
                    Manifest.permission.CAMERA,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.READ_MEDIA_IMAGES,
                    Manifest.permission.READ_MEDIA_VIDEO
            };
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            // Android 10–12L: WRITE_EXTERNAL_STORAGE is silently denied by the OS — omit it.
            return new String[]{
                    Manifest.permission.CAMERA,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.READ_EXTERNAL_STORAGE
            };
        } else {
            // Android 9 and below: both legacy storage permissions are required.
            return new String[]{
                    Manifest.permission.CAMERA,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE
            };
        }
    }

    public static void verifyStoragePermissions(Activity activity) {
        try {
            String[] permissions = getRequiredPermissions();
            java.util.List<String> missingPermissions = new java.util.ArrayList<>();
            StringBuilder toastText = null;
            for (int i = 0; i != permissions.length; ++i) {
                int reqCode = ActivityCompat.checkSelfPermission(activity, permissions[i]);
                if (reqCode != PackageManager.PERMISSION_GRANTED) {
                    missingPermissions.add(permissions[i]);
                    if (toastText == null) {
                        toastText = new StringBuilder("Request permission");
                    }
                    toastText.append(" ").append(permissions[i]);
                }
            }
            if (toastText != null && !missingPermissions.isEmpty()) {
                Toast.makeText(activity, toastText.toString(), Toast.LENGTH_LONG).show();
                ActivityCompat.requestPermissions(activity,
                        missingPermissions.toArray(new String[0]), REQUEST_PERMISSION);
            }
        } catch (Exception e) {
            Log.e(Common.LOG_TAG, "Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
