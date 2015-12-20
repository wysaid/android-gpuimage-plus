package org.wysaid.texUtils;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;

import org.wysaid.common.FrameBufferObject;
import org.wysaid.common.ProgramObject;

/**
 * Created by wangyang on 15/7/23.
 */
public class TextureRendererBlur extends TextureRendererDrawOrigin {

    private static final String vshBlur = vshDrawDefault;

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
            "uniform %s inputImageTexture;\n" +
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
//            "  gl_FragColor.r = texture2D(inputImageTexture, texCoord).r;\n" +
            "}";

    protected int mTexCache = 0;

    protected FrameBufferObject mFBO;

    protected int mCacheTexWidth, mCacheTexHeight;

    private static final String SAMPLER_STEPS = "samplerSteps";

    private int mStepsLoc = 0;
    private int mStepsLocCache = 0;
    private float mSamplerScale = 1.0f;

    private ProgramObject mProgramDrawCache;

    public static TextureRendererBlur create(boolean isExternalOES) {
        TextureRendererBlur renderer = new TextureRendererBlur();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    public void setSamplerRadius(float radius) {
        mSamplerScale = radius / 4.0f;
    }


    //TODO 优化非external_OES逻辑， cache和原始相同
    @Override
    public boolean init(boolean isExternalOES) {
        TEXTURE_2D_BINDABLE = isExternalOES ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES : GLES20.GL_TEXTURE_2D;
        final String fshBlurExtOES = (isExternalOES ? REQUIRE_STRING_EXTERNAL_OES : "") + String.format(fshBlur, isExternalOES ? SAMPLER2D_VAR_EXTERNAL_OES : SAMPLER2D_VAR);
        final String fshBlurTex2D = String.format(fshBlur, SAMPLER2D_VAR);
        mFBO = new FrameBufferObject();

        mProgramDrawCache = new ProgramObject();
        mProgramDrawCache.bindAttribLocation(POSITION_NAME, 0);

        if(!mProgramDrawCache.init(vshBlurCache, fshBlurExtOES)) {
            Log.e(LOG_TAG, "blur filter program init failed - 1...");
            return false;
        }

        mProgramDrawCache.bind();
        mStepsLocCache = mProgramDrawCache.getUniformLoc(SAMPLER_STEPS);

        mProgram = new ProgramObject();
        mProgram.bindAttribLocation(POSITION_NAME, 0);

        if(!mProgram.init(vshBlur, fshBlurTex2D)) {
            Log.e(LOG_TAG, "blur filter program init failed - 2...");
            return false;
        }

        mProgram.bind();
        mStepsLoc = mProgram.getUniformLoc(SAMPLER_STEPS);
        setRotation(0.0f);

        return true;
    }

    @Override
    public void release() {
        if(mProgramDrawCache != mProgram)
            mProgramDrawCache.release();
        super.release();
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        mFBO.release();
        mFBO = null;
        GLES20.glDeleteTextures(1, new int[]{mTexCache}, 0);
        mTexCache = 0;
        mProgramDrawCache = null;
    }

    @Override
    public void renderTexture(int texID, Viewport viewport) {

        if(mTexCache == 0 || mCacheTexWidth != mTextureWidth || mCacheTexHeight != mTextureHeight) {
            resetCacheTexture();
        }

        mFBO.bind();

        GLES20.glViewport(0, 0, mCacheTexWidth, mCacheTexHeight);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(TEXTURE_2D_BINDABLE, texID);

        mProgramDrawCache.bind();
        GLES20.glUniform2f(mStepsLocCache, (1.0f / mTextureWidth) * mSamplerScale, 0.0f);

        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);

        if(viewport != null)
            GLES20.glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

        mProgram.bind();
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTexCache);
        GLES20.glUniform2f(mStepsLoc, 0.0f, (1.0f / mCacheTexWidth) * mSamplerScale);
        GLES20.glDrawArrays(DRAW_FUNCTION, 0, 4);
    }

    @Override
    public void setTextureSize(int w, int h) {
        super.setTextureSize(w, h);
    }

    @Override
    public String getVertexShaderString() {
        return vshBlur;
    }

    @Override
    public String getFragmentShaderString() {
        return fshBlur;
    }


    protected void resetCacheTexture() {
        Log.i(LOG_TAG, "resetCacheTexture...");
        mCacheTexWidth = mTextureWidth;
        mCacheTexHeight = mTextureHeight;
        if(mTexCache == 0)
        {
            int[] texCache = new int[1];
            GLES20.glGenTextures(1, texCache, 0);
            mTexCache = texCache[0];
        }

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTexCache);

        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, mCacheTexWidth, mCacheTexHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        mFBO.bindTexture(mTexCache);
    }

}
