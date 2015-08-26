package org.wysaid.texUtils;

/**
 * Created by wangyang on 15/7/23.
 */
public class TextureRendererEmboss extends TextureRendererDrawOrigin {
    private static final String fshEmboss = "" +
            "precision mediump float;\n" +
            "uniform %s inputImageTexture;\n" +
            "varying vec2 texCoord;\n" +
            "uniform vec2 samplerSteps;\n" +
            "const float stride = 2.0;\n" +
            "const vec2 norm = vec2(0.72, 0.72);\n" +
            "void main() {\n" +
            "  vec4 src = texture2D(inputImageTexture, texCoord);\n" +
            "  vec3 tmp = texture2D(inputImageTexture, texCoord + samplerSteps * stride * norm).rgb - src.rgb + 0.5;\n" +
            "  float f = (tmp.r + tmp.g + tmp.b) / 3.0;\n" +
            "  gl_FragColor = vec4(f, f, f, src.a);\n" +
            "}";

    protected static final String SAMPLER_STEPS = "samplerSteps";

    public static TextureRendererEmboss create(boolean isExternalOES) {
        TextureRendererEmboss renderer = new TextureRendererEmboss();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            return null;
        }
        return renderer;
    }

    @Override
    public boolean init(boolean isExternalOES) {
        if(setProgramDefualt(getVertexShaderString(), getFragmentShaderString(), isExternalOES)) {
            mProgram.bind();
            mProgram.sendUniformf(SAMPLER_STEPS, 1.0f / 640.0f, 1.0f / 640.0f);
            return true;
        }
        return false;
    }

    @Override
    public void setTextureSize(int w, int h) {
        super.setTextureSize(w, h);
        mProgram.bind();
        mProgram.sendUniformf(SAMPLER_STEPS, 1.0f / w, 1.0f / h);
    }


    @Override
    public String getFragmentShaderString() {
        return fshEmboss;
    }
}
