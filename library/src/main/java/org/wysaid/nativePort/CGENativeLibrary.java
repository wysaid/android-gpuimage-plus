/**
 * Created by wysaid on 15/7/8.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import org.wysaid.common.Common;

public class CGENativeLibrary {

    static {
        NativeLibraryLoader.load();
    }

    public enum TextureBlendMode
    {
        CGE_BLEND_MIX,            // 0 正常 - "result.rgb = src.rgb * (1 - alpha) + dst.rgb * alpha, alpha = intensity * dst.a, result.a = src.a" - Because android is using premultiplied bitmap&texture, 'CGE_BLEND_MIX' may get a result of twice the strength of the blendImage's alpha channel(The result would be darker than you want). For common usage of mix blending, please use 'CGE_BLEND_ADDREV'.
        CGE_BLEND_DISSOLVE,       // 1 溶解

        CGE_BLEND_DARKEN,         // 2 变暗
        CGE_BLEND_MULTIPLY,       // 3 正片叠底
        CGE_BLEND_COLORBURN,      // 4 颜色加深
        CGE_BLEND_LINEARBURN,     // 5 线性加深
        CGE_BLEND_DARKER_COLOR,   // 6 深色

        CGE_BLEND_LIGHTEN,        // 7 变亮
        CGE_BLEND_SCREEN,         // 8 滤色
        CGE_BLEND_COLORDODGE,     // 9 颜色减淡
        CGE_BLEND_LINEARDODGE,    // 10 线性减淡
        CGE_BLEND_LIGHTERCOLOR,  // 11 浅色

        CGE_BLEND_OVERLAY,        // 12 叠加
        CGE_BLEND_SOFTLIGHT,      // 13 柔光
        CGE_BLEND_HARDLIGHT,      // 14 强光
        CGE_BLEND_VIVIDLIGHT,     // 15 亮光
        CGE_BLEND_LINEARLIGHT,    // 16 线性光
        CGE_BLEND_PINLIGHT,       // 17 点光
        CGE_BLEND_HARDMIX,        // 18 实色混合

        CGE_BLEND_DIFFERENCE,     // 19 差值
        CGE_BLEND_EXCLUDE,        // 20 排除
        CGE_BLEND_SUBTRACT,       // 21 减去
        CGE_BLEND_DIVIDE,         // 22 划分

        CGE_BLEND_HUE,            // 23 色相
        CGE_BLEND_SATURATION,     // 24 饱和度
        CGE_BLEND_COLOR,          // 25 颜色
        CGE_BLEND_LUMINOSITY,     // 26 明度

        /////////////    More blend mode below (You can't see them in Adobe Photoshop)    //////////////

        CGE_BLEND_ADD,			  // 27
        CGE_BLEND_ADDREV,         // 28 - A fix for premultiplied BLEND_MIX
        CGE_BLEND_COLORBW,		  // 29

        /////////////    More blend mode above     //////////////

        CGE_BLEND_TYPE_MAX_NUM //Its value defines the max num of blend.
    };

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

        TextureResult result = loadTextureByBitmap(bmp);

        loadImageCallback.loadImageOK(bmp, callbackArg);
        return result;
    }

    //May be called from jni.
    public static TextureResult loadTextureByBitmap(Bitmap bmp) {

        if(bmp == null) {
            return null;
        }

        TextureResult result = new TextureResult();

        result.texID = Common.genNormalTextureID(bmp);
        result.width = bmp.getWidth();
        result.height = bmp.getHeight();
        return result;
    }

    public static TextureResult loadTextureByFile(String fileName) {
        Bitmap bmp = BitmapFactory.decodeFile(fileName);
        TextureResult result = loadTextureByBitmap(bmp);
        bmp.recycle();
        return result;
    }

    public static Bitmap filterImage_MultipleEffects(Bitmap bmp, String config, float intensity) {
        if(config == null || config.length() == 0) {
            return bmp;
        }
        return cgeFilterImage_MultipleEffects(bmp, config, intensity);
    }

    public static void filterImage_MultipleEffectsWriteBack(Bitmap bmp, String config, float intensity) {
        if(config != null && config.length() != 0) {
            cgeFilterImage_MultipleEffectsWriteBack(bmp, config, intensity);
        }
    }

    public enum BlendFilterType {
        BLEND_NORMAL,
        BLEND_KEEP_RATIO,
        BLEND_TILE,
    }

    //带纹理的 blendFilter 较为特殊， 增加单独处理方法, 第二个参数 texID 表示将要使用到的纹理id
    public static long createBlendFilter(TextureBlendMode blendMode, int texID, int texWidth, int texHeight, BlendFilterType blendFilterType, float intensity) {
        return cgeCreateBlendFilter(blendMode.ordinal(), texID, texWidth, texHeight, blendFilterType.ordinal(), intensity);
    }

    //////////////////////   native method  //////////////////

    // 多重特效滤镜， 提供配置文件内容直接进行， 返回相同大小的bitmap。
    // intensity 表示滤镜强度 [0, 1]
    public static native Bitmap cgeFilterImage_MultipleEffects(Bitmap bmp, String config, float intensity);

    // 同上， 结果直接写回传入bitmap， 无返回值
    public static native void cgeFilterImage_MultipleEffectsWriteBack(Bitmap bmp, String config, float intensity);

    ////////////////////////////////////

    public static native long cgeCreateFilterWithConfig(String config, float intensity);
    public static native void cgeDeleteFilterWithAddress(long address);
    public static native long cgeCreateBlendFilter(int blendMode, int texID, int texWidth, int texHeight, int blendFilterType, float intensity);

    ////////////////////////////////////
    // demo for custom filter， see more at `customFilter` of `jni/source`.
    // `hasContext` marks if the function is called width gl context already bind. (Extra gl context may be created if `hasContext` is false)
    // `intensity`:  0 for origin, 1 for most. (It can be more than 1 when `useWrapper` is true)
    // `useWrapper` marks if you want to use a wrapper. (The wrapper will receive the intensity, and do interpolation between the origin and the result when it's true)
    public static native Bitmap cgeFilterImageWithCustomFilter(Bitmap bmp, int index, float intensity, boolean hasContext, boolean useWrapper);
    // `index` marks which you want from your custom filter list.
    // `intensity`:  0 for origin, 1 for most. (It can be more than 1 when `useWrapper` is true)
    // `useWrapper` marks if you want to use a wrapper. (The wrapper will receive the intensity, and do interpolation between the origin and the result when it's true)
    public static native long cgeCreateCustomNativeFilter(int index, float intensity, boolean useWrapper);
    public static native int cgeGetCustomFilterNum();

}
