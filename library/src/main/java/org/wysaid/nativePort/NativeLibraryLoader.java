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
        System.loadLibrary("x264.142");
        System.loadLibrary("ffmpeg");
        System.loadLibrary("CGE");

        //You ignore the two libraries below(decreasing your package size), if you don't want any face features.
        System.loadLibrary("opencv_java3");
        System.loadLibrary("FaceTracker");

        CGEFFmpegNativeLibrary.avRegisterAll();
    }

}
