package org.wysaid.gpuCodec;

import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.Common;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Date: 2017/5/3
 * Description:
 */

public class TextureDrawerI420ToRGB extends TextureDrawerCodec {

    protected static final String fshI420ToRGB = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform sampler2D textureY;\n" +
            "uniform sampler2D textureU;\n" +
            "uniform sampler2D textureV;\n" +
            "uniform mat3 colorConversion;\n" +
            "void main()\n" +
            "{\n" +
            "    vec3 yuv;\n" +
            "    yuv.x = texture2D(textureY, texCoord).r;\n" +
            "    yuv.y = texture2D(textureU, texCoord).r - 0.5;\n" +
            "    yuv.z = texture2D(textureV, texCoord).r - 0.5;\n" +
            "    vec3 rgb = colorConversion * yuv;\n" +
            "    gl_FragColor = vec4(rgb, 1.0);\n" +
            "}";

    public static TextureDrawerI420ToRGB create() {
        TextureDrawerI420ToRGB drawer = new TextureDrawerI420ToRGB();
        if(!drawer.init(vshDrawer, fshI420ToRGB))
        {
            Log.e(Common.LOG_TAG, "TextureDrawerI420ToRGB create failed!");
            drawer.release();
            drawer = null;
        }
        return drawer;
    }

    @Override
    protected boolean init(String vsh, String fsh) {

        if(super.init(vsh, fsh)) {
            mProgram.bind();
            mProgram.sendUniformi("textureU", 1);
            mProgram.sendUniformi("textureV", 2);
            mProgram.sendUniformMat3(COLOR_CONVERSION_NAME, 1, false, MATRIX_YUV2RGB);
            return true;
        }

        return false;
    }

    //Just draw. Please ensure the textures are bound to the right target.
    public void drawTextures() {
        mProgram.bind();
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void drawTextures(int texY, int texU, int texV) {

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texY);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texU);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texV);

        drawTextures();
    }


}
