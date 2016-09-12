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
//        System.loadLibrary("x264.142"); //compiled to static library in the new version.

//        try {
            System.loadLibrary("ffmpeg");
            System.loadLibrary("CGE");

            //User defined mothods, not required. (including offscreen video rendering)
            System.loadLibrary("CGEExt");
            CGEFFmpegNativeLibrary.avRegisterAll();
//        } catch (Throwable t) {
//            Log.e(Common.LOG_TAG, "Load library for 'cge' failed! Please check your project config.", t);
//        }

    }

}
