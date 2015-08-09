package org.wysaid.myUtils;

import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by wangyang on 15/7/27.
 */

public class ImageUtil {
    public static final String LOG_TAG = Common.LOG_TAG;
    public static final File parentPath = Environment.getExternalStorageDirectory();
    public static String storagePath = null;
    public static final String DST_FOLDER = "cgeDemo";

    public static String getPath() {
        if(storagePath == null) {
            storagePath = parentPath.getAbsolutePath() + "/" + DST_FOLDER;
            File file = new File(storagePath);
            if(!file.exists()) {
                file.mkdir();
            }
        }

        return storagePath;
    }

    public static void saveBitmap(Bitmap bmp) {
        String path = getPath();
        long currentTime = System.currentTimeMillis();
        String filename = path + "/" + currentTime + ".jpg";
        Log.i(LOG_TAG, "saving Bitmap : " + filename);

        try {
            FileOutputStream fileout = new FileOutputStream(filename);
            BufferedOutputStream bufferOutStream = new BufferedOutputStream(fileout);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, bufferOutStream);
            bufferOutStream.flush();
            bufferOutStream.close();
        } catch (IOException e) {
            Log.e(LOG_TAG, "Err when saving bitmap...");
            e.printStackTrace();
            return;
        }

        Log.i(LOG_TAG, "Bitmap " + filename + " saved!");
    }

}

