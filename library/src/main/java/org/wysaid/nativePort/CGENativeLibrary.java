/**
 * Created by wysaid on 15/7/8.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import org.wysaid.common.Common;

public class CGENativeLibrary {

    static {
        NativeLibraryLoader.load();
    }

    public interface LoadImageCallback {
        Bitmap loadImage(String name, Object arg);
        void loadImageOK(Bitmap bmp, Object arg);
    }

    static LoadImageCallback loadImageCallback;
    static Object callbackArg;

    public static void setLoadImageCallback(LoadImageCallback callback, Object arg) {
        loadImageCallback = callback;
        callbackArg = arg;
    }

    public static class TextureResult {
        int texID;
        int width, height;
    }

    //will be called from jni.
    public static TextureResult loadTextureByName(String sourceName) {
        if(loadImageCallback == null) {
            Log.i(Common.LOG_TAG, "The loading callback is not set!");
            return null;
        }

        Bitmap bmp = loadImageCallback.loadImage(sourceName, callbackArg);

        if(bmp == null) {
            return null;
        }

        TextureResult result = new TextureResult();
        int[] texID = new int[1];
        GLES20.glGenTextures(1, texID, 0);
        result.texID = texID[0];
        result.width = bmp.getWidth();
        result.height = bmp.getHeight();
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, result.texID);
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bmp, 0);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        loadImageCallback.loadImageOK(bmp, callbackArg);
        return result;
    }

    public static Bitmap filterImage_MultipleEffects(Bitmap bmp, String config, float intensity) {
        if(config == null || config == "") {
            return bmp;
        }
        return cgeFilterImage_MultipleEffects(bmp, config, intensity);
    }

    public static void filterImage_MultipleEffectsWriteBack(Bitmap bmp, String config, float intensity) {
        if(config != null && config != "") {
            cgeFilterImage_MultipleEffectsWriteBack(bmp, config, intensity);
        }
    }

    // 多重特效滤镜， 提供配置文件内容直接进行， 返回相同大小的bitmap。
    // intensity 表示滤镜强度 [0, 1]
    public static native Bitmap cgeFilterImage_MultipleEffects(Bitmap bmp, String config, float intensity);

    // 同上， 结果直接写回传入bitmap， 无返回值
    public static native void cgeFilterImage_MultipleEffectsWriteBack(Bitmap bmp, String config, float intensity);

    ////////////////////////////////////

//    private static native boolean cgeContextInit();
//    private static native void cgeContextRelease();



}
