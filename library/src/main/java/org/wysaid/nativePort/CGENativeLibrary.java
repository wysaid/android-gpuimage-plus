/**
 * Created by wysaid on 15/7/8.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

package org.wysaid.nativePort;

import android.graphics.Bitmap;

public class CGENativeLibrary {

    static {
        NativeLibraryLoader.load();
    }

    //全局初始化接口， 调用后将初始化并在Native层创建一个OpenGL的Context以供后面使用
    public static boolean globalInit() {
        return cgeContextInit();
    }

    //全局释放接口， 调用后将清除已创建的Context
    public static void globalRelease() {
        cgeContextRelease();
    }

    // 多重特效滤镜， 提供配置文件内容直接进行， 返回相同大小的bitmap。
    // intensity 表示滤镜强度 [0, 1]
    // (未实现)
    public static native Bitmap filterImage_MultipleEffects(Bitmap bmp, String config, float intensity);

    // 同上， 结果直接写回传入bitmap， 无返回值
    public static native void filterImage_MultipleEffectsWriteBack(Bitmap bmp, String config, float intensity);

    ////////////////////////////////////

    private static native boolean cgeContextInit();
    private static native void cgeContextRelease();



}
