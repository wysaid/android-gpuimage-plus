package org.wysaid.cgeDemo.demoUtils;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.util.AttributeSet;

import org.wysaid.common.Common;
import org.wysaid.nativePort.CGEMultiInputFilterWrapper;
import org.wysaid.view.CameraRecordGLSurfaceView;

import java.io.InputStream;

import javax.microedition.khronos.opengles.GL10;

public class MultiInputDemo extends CameraRecordGLSurfaceView {
    public MultiInputDemo(Context context, AttributeSet attrs) {
        super(context, attrs);
        mAssetManager = context.getAssets();
    }

    public static final String VSH = "" +
            "attribute vec2 vPosition;\n" +
            "varying vec2 textureCoordinate;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
            "   textureCoordinate = (vPosition / 2.0) + 0.5;\n" +
            "}";

    public static final String FSH = "" +
            "precision mediump float;\n" +
            "varying vec2 textureCoordinate;\n" +
            "\n" +
            "uniform sampler2D inputImageTexture;\n" +
            "uniform sampler2D inputTexture0;\n" +
            "uniform sampler2D inputTexture1;\n" +
            "uniform sampler2D inputTexture2;\n" +
            "uniform sampler2D inputTexture3;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    vec3 camera = texture2D(inputImageTexture, textureCoordinate).rgb;\n" +
            "\n" +
            "    if(textureCoordinate.x < 0.5 && textureCoordinate.y < 0.5)\n" +
            "    {\n" +
            "        vec4 c = texture2D(inputTexture0, textureCoordinate * 2.0);\n" +
            "        camera = mix(camera, c.rgb, c.a);\n" +
            "    }\n" +
            "\n" +
            "    if(textureCoordinate.x < 0.5 && textureCoordinate.y > 0.5)\n" +
            "    {\n" +
            "        vec3 c = texture2D(inputTexture1, textureCoordinate * 2.0 + vec2(0.0, -1.0)).rgb;\n" +
            "        camera += c;\n" +
            "    }\n" +
            "\n" +
            "    if(textureCoordinate.x > 0.5 && textureCoordinate.y < 0.5)\n" +
            "    {\n" +
            "        vec4 c = texture2D(inputTexture2, textureCoordinate * 2.0 + vec2(-1.0, 0.0));\n" +
            "        camera = mix(camera, c.rgb, c.a);\n" +
            "    }\n" +
            "\n" +
            "    if(textureCoordinate.x > 0.5 && textureCoordinate.y > 0.5)\n" +
            "    {\n" +
            "        vec4 c = texture2D(inputTexture3, (textureCoordinate - 0.5) * 2.0);\n" +
            "        camera = mix(camera, c.rgb, c.a);\n" +
            "    }\n" +
            "\n" +
            "    gl_FragColor.rgb = camera;\n" +
            "    gl_FragColor.a = 1.0;\n" +
            "}";

    CGEMultiInputFilterWrapper mWrapper = null;
    AssetManager mAssetManager;
    int[] mInputTexturesPerFrame;

    class ResourceGroup {
        String[] resourcePath;
        int[] textureIDs;
        int currentIndex;
        int count;

        int getNextTexture() {
            currentIndex %= count;
            return getTextureByIndex(currentIndex++);
        }

        int getTextureByIndex(int index) {
            if (index < 0 || index >= count || textureIDs == null || resourcePath == null) {
                return 0;
            }

            if (textureIDs[index] == 0 && resourcePath[index] != null) {
                try {
                    InputStream is = mAssetManager.open(resourcePath[index]);
                    Bitmap bmp = BitmapFactory.decodeStream(is);
                    if (bmp != null) {
                        textureIDs[index] = Common.genNormalTextureID(bmp);
                        bmp.recycle();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            return textureIDs[index];
        }
    }

    ResourceGroup[] mResourceLists = null;

    protected void setResource(ResourceGroup group, String rule, int max) {
        group.resourcePath = new String[max];
        group.textureIDs = new int[max];
        for (int i = 0; i < max; ++i) {
            group.resourcePath[i] = String.format(rule, i);
            group.textureIDs[i] = 0;
        }
        group.count = max;
        group.currentIndex = 0;
    }

    protected void initResourceList() {
        mResourceLists = new ResourceGroup[4];

        for (int i = 0; i != 4; ++i) {
            mResourceLists[i] = new ResourceGroup();
        }

        setResource(mResourceLists[0], "multiInput/group1/%02d.png", 10);
        setResource(mResourceLists[1], "multiInput/group2/%02d.png", 31);
        setResource(mResourceLists[2], "multiInput/group3/%02d.png", 11);
        setResource(mResourceLists[3], "multiInput/group4/%02d.png", 11);
    }


    public void triggerEffect() {

        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mWrapper != null)
                    return;

                initResourceList();
                mWrapper = CGEMultiInputFilterWrapper.create(VSH, FSH);
                if(mWrapper != null) {
                    mFrameRecorder.setNativeFilter(mWrapper.getNativeAddress());
                }
            }
        });
    }

    @Override
    protected void onRelease() {
        super.onRelease();

        if (mWrapper != null) {
            mWrapper.release(false);
            mWrapper = null;
        }

        if (mResourceLists != null) {
            for (ResourceGroup group : mResourceLists) {
                if (group != null && group.textureIDs != null) {
                    GLES20.glDeleteTextures(group.textureIDs.length, group.textureIDs, 0);
                }
            }
            mResourceLists = null;
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if (mWrapper != null) {
            if (mInputTexturesPerFrame == null) {
                mInputTexturesPerFrame = new int[4];
            }
            for (int i = 0; i != 4; ++i) {
                mInputTexturesPerFrame[i] = mResourceLists[i].getNextTexture();
            }
            mWrapper.updateInputTextures(mInputTexturesPerFrame);
        }

        super.onDrawFrame(gl);
    }
}
