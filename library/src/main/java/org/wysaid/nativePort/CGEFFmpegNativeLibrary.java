package org.wysaid.nativePort;

import android.graphics.Bitmap;

/**
 * Created by wangyang on 15/7/30.
 */
public class CGEFFmpegNativeLibrary {
    static {
        NativeLibraryLoader.load();
    }

    //视频转换+特效可能执行较长的时间， 请置于后台线程运行.
    public static boolean generateVideoWithFilter(String outputFilename, String inputFilename, String filterConfig, float filterIntensity, Bitmap blendImage, CGENativeLibrary.TextureBlendMode blendMode, float blendIntensity) {

        return nativeGenerateVideoWithFilter(outputFilename, inputFilename, filterConfig, filterIntensity, blendImage, blendMode.ordinal(), blendIntensity);

    }

    //////////////////////////////////////////

    private static native boolean nativeGenerateVideoWithFilter(String outputFilename, String inputFilename, String filterConfig, float filterIntensity, Bitmap blendImage, int blendMode, float blendIntensity);

    public static native void avRegisterAll();

}
