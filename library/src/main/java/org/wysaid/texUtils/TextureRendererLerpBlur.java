package org.wysaid.texUtils;

import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.FrameBufferObject;
import org.wysaid.common.ProgramObject;

/**
 * Created by wangyang on 15/7/24.
 */
public class TextureRendererLerpBlur extends TextureRendererDrawOrigin {

    private static final String vshUpScale = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 texCoord;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
            "   texCoord = vPosition / 2.0 + 0.5;\n" +
            "}";

    private static final String fshUpScale = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform sampler2D inputImageTexture;\n" +

            "void main()\n" +
            "{\n" +
            "   gl_FragColor = texture2D(inputImageTexture, texCoord);\n" +
            "}";

    private static final String vshBlurUpScale = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 texCoords[5];\n" +
            "uniform vec2 samplerSteps;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "  gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
            "  vec2 texCoord = vPosition / 2.0 + 0.5;\n" +
            "  texCoords[0] = texCoord - 2.0 * samplerSteps;\n" +
            "  texCoords[1] = texCoord - 1.0 * samplerSteps;\n" +
            "  texCoords[2] = texCoord;\n" +
            "  texCoords[3] = texCoord + 1.0 * samplerSteps;\n" +
            "  texCoords[4] = texCoord + 2.0 * samplerSteps;\n" +
            "}";

    private static final String fshBlurUpScale = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoords[5];\n" +
            "uniform sampler2D inputImageTexture;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "  vec3 color = texture2D(inputImageTexture, texCoords[0]).rgb * 0.1;\n" +
            "  color += texture2D(inputImageTexture, texCoords[1]).rgb * 0.2;\n" +
            "  color += texture2D(inputImageTexture, texCoords[2]).rgb * 0.4;\n" +
            "  color += texture2D(inputImageTexture, texCoords[3]).rgb * 0.2;\n" +
            "  color += texture2D(inputImageTexture, texCoords[4]).rgb * 0.1;\n" +
            "\n" +
            "  gl_FragColor = vec4(color, 1.0);\n" +
            "}";

    private static final String vshBlurCache = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 texCoord;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
            "   texCoord = vPosition / 2.0 + 0.5;\n" +
            "}";

    private static final String fshBlur = "" +
            "precision highp float;\n" +
            "varying vec2 texCoord;\n" +
            "uniform sampler2D inputImageTexture;\n" +
            "uniform vec2 samplerSteps;\n" +

            "const int samplerRadius = 5;\n" +
            "const float samplerRadiusFloat = 5.0;\n" +

            "float random(vec2 seed)\n" +
            "{\n" +
            "  return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);\n" +
            "}\n" +

            "void main()\n" +
            "{\n" +
            "  vec3 resultColor = vec3(0.0);\n" +
            "  float blurPixels = 0.0;\n" +
            "  float offset = random(texCoord) - 0.5;\n" +
            "  \n" +
            "  for(int i = -samplerRadius; i <= samplerRadius; ++i)\n" +
            "  {\n" +
            "    float percent = (float(i) + offset) / samplerRadiusFloat;\n" +
            "    float weight = 1.0 - abs(percent);\n" +
            "    vec2 coord = texCoord + samplerSteps * percent;\n" +
            "    resultColor += texture2D(inputImageTexture, coord).rgb * weight;\n" +
            "    blurPixels += weight;\n" +
            "  }\n" +

            "  gl_FragColor = vec4(resultColor / blurPixels, 1.0);\n" +
            "}";

    private static final String SAMPLER_STEPS = "samplerSteps";

    private ProgramObject mScaleProgram;
    private int[] mTextureDownScale;

    private FrameBufferObject mFramebuffer;
    private Viewport mTexViewport;
    private int mSamplerStepLoc = 0;

    private int mIntensity = 0;

    private float mSampleScaling = 1.0f;

    private final int mLevel = 16;
    private final float mBase = 2.0f;

    public static TextureRendererLerpBlur create(boolean isExternalOES) {
        TextureRendererLerpBlur renderer = new TextureRendererLerpBlur();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    //intensity >= 0
    public void setIntensity(int intensity) {

        if(intensity == mIntensity)
            return;

        mIntensity = intensity;
        if(mIntensity > mLevel)
            mIntensity = mLevel;
    }

    @Override
    public boolean init(boolean isExternalOES) {
        return super.init(isExternalOES) && initLocal();
    }

    @Override
    public void renderTexture(int texID, Viewport viewport) {

        if(mIntensity == 0) {
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
            super.renderTexture(texID, viewport);
            return;
        }

//        if(mShouldUpdateTexture) {
//            updateTexture();
//        }

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);

        mFramebuffer.bindTexture(mTextureDownScale[0]);
        //down scale

        mTexViewport.width = calcMips(512, 1);
        mTexViewport.height = calcMips(512, 1);
        super.renderTexture(texID, mTexViewport);

        mScaleProgram.bind();
        for(int i = 1; i < mIntensity; ++i) {
            mFramebuffer.bindTexture(mTextureDownScale[i]);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[i - 1]);
            GLES20.glViewport(0, 0, calcMips(512, i + 1), calcMips(512, i + 1));
            GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
        }

        for(int i = mIntensity - 1; i > 0; --i) {
            mFramebuffer.bindTexture(mTextureDownScale[i - 1]);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[i]);
            GLES20.glViewport(0, 0, calcMips(512, i), calcMips(512, i));
            GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
        }

        GLES20.glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[0]);
//        GLES20.glUniform2f(mSamplerStepLoc, 0.0f, (0.5f / mTexViewport.width) * mSampleScaling);

        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }

    @Override
    public void release() {
        mScaleProgram.release();
        mFramebuffer.release();
        GLES20.glDeleteTextures(mTextureDownScale.length, mTextureDownScale, 0);
        mScaleProgram = null;
        mFramebuffer = null;
    }

    private boolean initLocal() {

        genMipmaps(mLevel, 512, 512);
        mFramebuffer = new FrameBufferObject();

        mScaleProgram = new ProgramObject();
        mScaleProgram.bindAttribLocation(POSITION_NAME, 0);

//        if(!mScaleProgram.init(vshBlurUpScale, fshBlurUpScale)) {
        if(!mScaleProgram.init(vshUpScale, fshUpScale)) {
            Log.e(LOG_TAG, "Lerp blur initLocal failed...");
            return false;
        }

//        mScaleProgram.bind();
//        mSamplerStepLoc = mScaleProgram.getUniformLoc(SAMPLER_STEPS);

        return true;
    }

    private void updateTexture() {
//        if(mIntensity == 0)
//            return;
//
//        int useIntensity = mIntensity;
//
//        if(useIntensity > 6) {
//            mSampleScaling = useIntensity / 6.0f;
//            useIntensity = 6;
//        }
//
//        int scalingWidth = mTextureHeight / useIntensity;
//        int scalingHeight = mTextureWidth / useIntensity;
//
//        if(scalingWidth == 0)
//            scalingWidth = 1;
//        if(scalingHeight == 0)
//            scalingHeight = 1;
//
//        mTexViewport = new Viewport(0, 0, scalingWidth, scalingHeight);
//
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[0]);
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, scalingWidth, scalingHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
//
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[1]);
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, scalingWidth, scalingHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
//
//        mShouldUpdateTexture = false;
//
//        Log.i(LOG_TAG, "Lerp blur - updateTexture");
//
//        Common.checkGLError("Lerp blur - updateTexture");
    }



    @Override
    public void setTextureSize(int w, int h) {
        super.setTextureSize(w, h);
    }

    private void genMipmaps(int level, int width, int height) {
        mTextureDownScale = new int[level];
        GLES20.glGenTextures(level, mTextureDownScale, 0);

        for(int i = 0; i < level; ++i) {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDownScale[i]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, calcMips(width, i + 1), calcMips(height, i + 1), 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        }

        mTexViewport = new Viewport(0, 0, 512, 512);
    }

    private int calcMips(int len, int level) {
//        return (int)(len / Math.pow(mBase, (level + 1)));
        return len / (level + 1);
    }

}
