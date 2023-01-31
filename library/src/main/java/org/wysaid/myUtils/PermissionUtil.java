package org.wysaid.myUtils;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.util.Log;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import org.wysaid.common.Common;

import java.util.Vector;

public class PermissionUtil {
    private static final int REQUEST_PERMISSION = 0;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.CAMERA,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};

    public static void verifyStoragePermissions(Activity activity) {
        try {
            StringBuilder toastText = null;
            for (int i = 0; i != PERMISSIONS_STORAGE.length; ++i) {
                int reqCode = ActivityCompat.checkSelfPermission(activity, PERMISSIONS_STORAGE[i]);
                if (reqCode != PackageManager.PERMISSION_GRANTED) {
                    // No sdcard write permission, perform request.
                    if (toastText == null) {
                        toastText = new StringBuilder("Request permission");
                    }
                    toastText.append(" ").append(PERMISSIONS_STORAGE[i]);
                }
            }
            if (toastText != null) {
                Toast.makeText(activity, toastText.toString(), Toast.LENGTH_LONG).show();
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, REQUEST_PERMISSION);
            }
        } catch (Exception e) {
            Log.e(Common.LOG_TAG, "Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
