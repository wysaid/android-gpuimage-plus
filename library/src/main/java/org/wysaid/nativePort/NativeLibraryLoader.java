package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/7/30.
 */
public class NativeLibraryLoader {

    private static boolean mLibraryLoaded = false;

    public static void load() {
        if(mLibraryLoaded)
            return;
        mLibraryLoaded = true;
//        System.loadLibrary("gnustl_shared");
//        System.loadLibrary("x264.142");
//        System.loadLibrary("ffmpeg");
        System.loadLibrary("CGE");
//        FFmpegNativeLibrary.avRegisterAll();
    }

}
