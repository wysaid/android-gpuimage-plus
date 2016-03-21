package org.wysaid.nativePort;

import android.graphics.Bitmap;

/**
 * Created by wangyang on 15/7/30.
 */
public class CGEFFmpegNativeLibrary {
    static {
        NativeLibraryLoader.load();
    }

    public static native void avRegisterAll();

}
