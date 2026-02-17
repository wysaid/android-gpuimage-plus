package org.wysaid.nativePort;

import android.os.Build;
import android.util.Log;

import org.wysaid.common.Common;
import org.wysaid.library.BuildConfig;

/**
 * Created by wangyang on 15/7/30.
 */
public class NativeLibraryLoader {

    public static void load() {
        try {
            if (BuildConfig.CGE_USE_VIDEO_MODULE) {
                System.loadLibrary("ffmpeg");
            }
            System.loadLibrary("CGE");
            System.loadLibrary("CGEExt");
            if (BuildConfig.CGE_USE_VIDEO_MODULE) {
                CGEFFmpegNativeLibrary.avRegisterAll();
            }
            onLoad();
        } catch (UnsatisfiedLinkError e) {
            // Log device info for debugging
            String abi = (Build.SUPPORTED_ABIS != null && Build.SUPPORTED_ABIS.length > 0)
                ? Build.SUPPORTED_ABIS[0] : "unknown";
            Log.e(Common.LOG_TAG, String.format(
                "Failed to load native libraries on %s %s (Android %s, ABI: %s)",
                Build.MANUFACTURER, Build.MODEL, Build.VERSION.RELEASE, abi
            ), e);
            throw e;
        }
    }

    static native void onLoad();
}
