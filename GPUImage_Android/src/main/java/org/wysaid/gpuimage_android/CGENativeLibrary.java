package org.wysaid.gpuimage_android;

import android.content.Context;

/**
 * Created by wysaid on 15/7/7.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

// 定义整个JNI导入接口以及加载library
public class CGENativeLibrary {

    static {
        System.loadLibrary("CGE");
    }

    //全局初始化接口， 调用后将初始化并在Native层创建一个OpenGL的Context以供后面使用
    public static boolean globalInit() {
        return _globalInit();
    }

    //全局释放接口， 调用后将清除已创建的Context
    public static void globalRelease() {
        _globalRelease();
    }

    ////////////////////////////////////


    private static native boolean _globalInit();
    private static native void _globalRelease();

//    private final Context
}
