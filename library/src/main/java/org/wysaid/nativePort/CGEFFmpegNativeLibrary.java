package org.wysaid.nativePort;

import android.graphics.Bitmap;

/**
 * Created by wangyang on 15/7/30.
 */
public class CGEFFmpegNativeLibrary {
    static {
        NativeLibraryLoader.load();
    }

    //CN: 视频转换+特效可能执行较长的时间， 请置于后台线程运行.
    //EN: Convert video + Filter Effects may take some time, so you'd better put it on another thread.
    public static boolean generateVideoWithFilter(String outputFilename, String inputFilename, String filterConfig, float filterIntensity, Bitmap blendImage, CGENativeLibrary.TextureBlendMode blendMode, float blendIntensity, boolean mute) {

        return nativeGenerateVideoWithFilter(outputFilename, inputFilename, filterConfig, filterIntensity, blendImage, blendMode == null ? 0 : blendMode.ordinal(), blendIntensity, mute);

    }

    //////////////////////////////////////////

    private static native boolean nativeGenerateVideoWithFilter(String outputFilename, String inputFilename, String filterConfig, float filterIntensity, Bitmap blendImage, int blendMode, float blendIntensity, boolean mute);

    public static native void avRegisterAll();

}
