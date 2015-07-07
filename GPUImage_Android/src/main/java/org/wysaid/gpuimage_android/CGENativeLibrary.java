package org.wysaid.gpuimage_android;

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

    public static native void testFunc();


}
