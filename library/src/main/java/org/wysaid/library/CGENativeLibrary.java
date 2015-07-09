/**
 * Created by wysaid on 15/7/8.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

package org.wysaid.library;

import android.graphics.Bitmap;

public class CGENativeLibrary {

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("CGE");
    }

    //全局初始化接口， 调用后将初始化并在Native层创建一个OpenGL的Context以供后面使用
    public static boolean globalInit() {
        return cgeContextInit();
    }

    //全局释放接口， 调用后将清除已创建的Context
    public static void globalRelease() {
        cgeContextRelease();
    }

    public static native void filterImage_MultipleEffects(Bitmap src, Bitmap dst, String str);

    ////////////////////////////////////

    private static native boolean cgeContextInit();
    private static native void cgeContextRelease();



}
