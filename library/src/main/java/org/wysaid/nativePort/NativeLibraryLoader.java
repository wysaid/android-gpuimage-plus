package org.wysaid.nativePort;

import org.wysaid.library.BuildConfig;

/**
 * Created by wangyang on 15/7/30.
 */
public class NativeLibraryLoader {

    public static void load() {

        if (BuildConfig.CGE_USE_VIDEO_MODULE) {
            System.loadLibrary("ffmpeg");
        }
        System.loadLibrary("CGE");
        System.loadLibrary("CGEExt");
        if (BuildConfig.CGE_USE_VIDEO_MODULE) {
            CGEFFmpegNativeLibrary.avRegisterAll();
        }
        onLoad();
    }

    static native void onLoad();
}
