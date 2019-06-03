package org.wysaid.gpuCodec;

import android.util.Log;

import org.wysaid.common.Common;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Date: 2017/5/3
 * Description:
 */

public class TextureDrawerNV12ToRGB extends TextureDrawerNV21ToRGB {

    private static final String fshNV12ToRGB = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform sampler2D textureY;\n" +
            "uniform sampler2D textureUV;\n" +
            "uniform mat3 colorConversion;\n" +
            "void main()\n" +
            "{\n" +
            "    vec3 yuv;\n" +
            "    yuv.x = texture2D(textureY, texCoord).r;\n" +
            "    yuv.yz = texture2D(textureUV, texCoord).ar - vec2(0.5, 0.5);\n" +
            "    vec3 rgb = colorConversion * yuv;\n" +
            "    gl_FragColor = vec4(rgb, 1.0);\n" +
            "}";

    public static TextureDrawerNV12ToRGB create() {
        TextureDrawerNV12ToRGB drawer = new TextureDrawerNV12ToRGB();
        if(!drawer.init(vshDrawer, fshNV12ToRGB))
        {
            Log.e(Common.LOG_TAG, "TextureDrawerNV12ToRGB create failed!");
            drawer.release();
            drawer = null;
        }
        return drawer;
    }

}
