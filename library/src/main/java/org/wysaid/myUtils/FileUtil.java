package org.wysaid.myUtils;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import org.wysaid.common.Common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

/**
 * Created by wangyang on 15/11/27.
 */
public class FileUtil {

    public static final String LOG_TAG = Common.LOG_TAG;
    public static final File externalStorageDirectory = Environment.getExternalStorageDirectory();
    public static String packageFilesDirectory = null;
    public static String storagePath = null;
    private static String mDefaultFolder = "libCGE";
    private static Context sAppContext = null;

    /**
     * Initialize FileUtil with application context.
     * Must be called once before using {@link #getPath()} without a context parameter.
     * Recommended: call in {@code Activity.onCreate()} or {@code Application.onCreate()}.
     *
     * @param context Any context; the application context will be extracted automatically.
     */
    public static void init(Context context) {
        if (context != null) {
            sAppContext = context.getApplicationContext();
        }
    }

    public static void setDefaultFolder(String defaultFolder) {
        mDefaultFolder = defaultFolder;
    }

    public static String getPath() {
        return getPath(sAppContext);
    }

    /**
     * Get the storage path for saving files.
     *
     * <p>Strategy (in order of preference):
     * <ol>
     *   <li>App-specific external storage ({@code context.getExternalFilesDir()}) — works on
     *       all Android versions without extra runtime permissions, and native code can write
     *       to it directly via file-system paths.</li>
     *   <li>Legacy external storage ({@code /sdcard/libCGE}) — only attempted as a fallback
     *       and only if the directory already exists or can be created.</li>
     *   <li>App-internal storage ({@code context.getFilesDir()}) — last resort.</li>
     * </ol>
     *
     * @param context A context used to resolve app-specific directories. May be null,
     *                in which case only the cached path or legacy path is returned.
     */
    public static String getPath(Context context) {

        if (storagePath != null) {
            return storagePath;
        }

        // 1. Prefer app-specific external storage (no permissions needed, native-writable)
        if (context != null) {
            File externalDir = context.getExternalFilesDir(null);
            if (externalDir != null) {
                String path = externalDir.getAbsolutePath() + "/" + mDefaultFolder;
                File dir = new File(path);
                if (dir.exists() || dir.mkdirs()) {
                    storagePath = path;
                    Log.i(LOG_TAG, "FileUtil: Using app-specific external storage: " + storagePath);
                    return storagePath;
                }
            }
        }

        // 2. Fallback: legacy external storage (may fail on Android 10+ with scoped storage)
        String legacyPath = externalStorageDirectory.getAbsolutePath() + "/" + mDefaultFolder;
        File legacyDir = new File(legacyPath);
        if (legacyDir.exists() || legacyDir.mkdirs()) {
            storagePath = legacyPath;
            Log.w(LOG_TAG, "FileUtil: Falling back to legacy external storage: " + storagePath);
            return storagePath;
        }

        // 3. Last resort: app-internal storage
        storagePath = getPathInPackage(context, true);
        if (storagePath != null) {
            Log.w(LOG_TAG, "FileUtil: Falling back to internal storage: " + storagePath);
        } else {
            Log.e(LOG_TAG, "FileUtil: All storage paths unavailable!");
        }
        return storagePath;
    }

    public static String getPathInPackage(Context context, boolean grantPermissions) {

        if(context == null || packageFilesDirectory != null)
            return packageFilesDirectory;

        // Phone does not have sdcard, need to use data/data/name.of.package/files directory
        String path = context.getFilesDir() + "/" + mDefaultFolder;
        File file = new File(path);

        if(!file.exists()) {
            if(!file.mkdirs()) {
                Log.e(LOG_TAG, "Create package dir of CGE failed!");
                return null;
            }

            if(grantPermissions) {

                //设置隐藏目录权限.
                if (file.setExecutable(true, false)) {
                    Log.i(LOG_TAG, "Package folder is executable");
                }

                if (file.setReadable(true, false)) {
                    Log.i(LOG_TAG, "Package folder is readable");
                }

                if (file.setWritable(true, false)) {
                    Log.i(LOG_TAG, "Package folder is writable");
                }
            }
        }

        packageFilesDirectory = path;
        return packageFilesDirectory;
    }

    public static void saveTextContent(String text, String filename) {
        Log.i(LOG_TAG, "Saving text : " + filename);

        try {
            FileOutputStream fileout = new FileOutputStream(filename);
            fileout.write(text.getBytes());
            fileout.flush();
            fileout.close();
        } catch (Exception e) {
            Log.e(LOG_TAG, "Error: " + e.getMessage());
        }
    }

    public static String getTextContent(String filename) {
        Log.i(LOG_TAG, "Reading text : " + filename);

        if(filename == null) {
            return null;
        }

        String content = "";
        byte[] buffer = new byte[256]; //Create cache for reading.

        try {

            FileInputStream filein = new FileInputStream(filename);
            int len;

            while(true) {
                len = filein.read(buffer);

                if(len <= 0)
                    break;

                content += new String(buffer, 0, len);
            }

        } catch (Exception e) {
            Log.e(LOG_TAG, "Error: " + e.getMessage());
            return null;
        }

        return content;
    }

}
