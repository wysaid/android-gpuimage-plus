package org.wysaid.myUtils;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.util.Log;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import org.wysaid.common.Common;

public class PermissionUtil {
    private static final int REQUEST_PERMISSION = 0;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};

    public static void verifyStoragePermissions(Activity activity) {
        try {

            for (int i = 0; i != PERMISSIONS_STORAGE.length; ++i) {
                int reqCode = ActivityCompat.checkSelfPermission(activity, PERMISSIONS_STORAGE[i]);
                if (reqCode != PackageManager.PERMISSION_GRANTED) {
                    // 没有写的权限，去申请写的权限，会弹出对话框
                    ActivityCompat.requestPermissions(activity, new String[]{PERMISSIONS_STORAGE[i]}, REQUEST_PERMISSION);
                    Toast.makeText(activity, "request permission " + PERMISSIONS_STORAGE[i] + " ...", Toast.LENGTH_LONG).show();
                }
            }
        } catch (Exception e) {
            Log.e(Common.LOG_TAG, "Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
