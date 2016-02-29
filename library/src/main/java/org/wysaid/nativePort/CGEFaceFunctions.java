package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.graphics.PointF;

import org.wysaid.common.Common;
import org.wysaid.common.SharedContext;

/**
 * Created by wangyang on 15/12/20.
 */
public class CGEFaceFunctions {

    static {
        NativeLibraryLoader.load();
    }

    public static class FaceFeature {

        public FaceFeature() {
            leftEyePos = new PointF();
            rightEyePos = new PointF();
            mouthPos = new PointF();
            chinPos = new PointF();
        }

        public FaceFeature(PointF leftEye, PointF rightEye, PointF mouth, PointF chin, float imgWidth, float imgHeight) {
            leftEyePos = leftEye;
            rightEyePos = rightEye;
            mouthPos = mouth;
            chinPos = chin;
            faceImageWidth = imgWidth;
            faceImageHeight = imgHeight;
        }

        public PointF leftEyePos, rightEyePos;
        public PointF mouthPos;
        public PointF chinPos;
        public float faceImageWidth, faceImageHeight;
    }

    public static Bitmap blendFaceWidthFeatures(Bitmap srcImage, FaceFeature srcFeature, Bitmap dstImage, FaceFeature dstFeature, SharedContext context) {
        SharedContext ctx = context == null ? SharedContext.create() : context;
        ctx.makeCurrent();

        int srcTexID = Common.genNormalTextureID(srcImage);
        int dstTexID = Common.genNormalTextureID(dstImage);

        if(srcTexID == 0 || dstTexID == 0) {
            return null;
        }

        float[] srcFaceFeature = {
                srcFeature.leftEyePos.x, srcFeature.leftEyePos.y,
                srcFeature.rightEyePos.x, srcFeature.rightEyePos.y,
                srcFeature.mouthPos.x, srcFeature.mouthPos.y,
                srcFeature.chinPos.x, srcFeature.chinPos.y,
                srcFeature.faceImageWidth, srcFeature.faceImageHeight,
        };

        float[] dstFaceFeature = {
                dstFeature.leftEyePos.x, dstFeature.leftEyePos.y,
                dstFeature.rightEyePos.x, dstFeature.rightEyePos.y,
                dstFeature.mouthPos.x, dstFeature.mouthPos.y,
                dstFeature.chinPos.x, dstFeature.chinPos.y,
                dstFeature.faceImageWidth, dstFeature.faceImageHeight,
        };

        Bitmap result = nativeBlendFaceWithFeatures(srcTexID, srcFaceFeature, dstTexID, dstFaceFeature);

        if(context == null) {
            ctx.release();
        }

        return result;
    }


    ////////////////////////////////////

    private static native Bitmap nativeBlendFaceWithFeatures(int srcTexID, float[] srcFeature, int dstTexID, float[] dstFeature);

}
