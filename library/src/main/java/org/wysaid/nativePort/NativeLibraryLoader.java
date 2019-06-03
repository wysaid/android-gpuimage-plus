package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/7/30.
 */
public class NativeLibraryLoader {

    public static void load() {
        System.loadLibrary("CGE");
        //User defined mothods, not required. (including offscreen video rendering)
        System.loadLibrary("CGEExt");
    }

}
