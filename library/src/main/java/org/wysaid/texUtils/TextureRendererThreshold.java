package org.wysaid.texUtils;

/**
 * Created by wysaid on 16/3/9.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */
public class TextureRendererThreshold extends TextureRendererDrawOrigin {

    private static final String fshThreshold = "" +
            "precision mediump float;\n" +
            "varying vec2 texCoord;\n" +
            " uniform %s inputImageTexture;\n" +
            " uniform float thresholdValue;\n" +
            " void main()\n" +
            "{\n" +
            "    vec4 color = texture2D(inputImageTexture, texCoord);\n" +
            "    \n" +
            "    float weight = (color.r + color.g + color.b) / 3.0;\n" +
            "    color.a = smoothstep(0.0, thresholdValue, weight);\n" +
            "    \n" +
            "    gl_FragColor = color;\n" +
            "}";

    protected static final String THRESHOLD_VALUE = "thresholdValue";

    public static TextureRendererThreshold create(boolean isExternalOES) {
        TextureRendererThreshold renderer = new TextureRendererThreshold();
        if(!renderer.init(isExternalOES)) {
            renderer.release();
            renderer = null;
        }
        return renderer;
    }

    public void setThresholdValue(float thresholdValue) {
        mProgram.bind();
        mProgram.sendUniformf(THRESHOLD_VALUE, thresholdValue);
    }

    @Override
    public String getFragmentShaderString() {
        return fshThreshold;
    }

}
