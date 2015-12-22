package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.graphics.PointF;

import org.wysaid.common.Common;
import org.wysaid.common.SharedContext;

/**
 * Created by wangyang on 15/12/20.
 */
public class CGEFaceFunctions {

    public static class FaceFeature {

        public FaceFeature() {}
        public FaceFeature(PointF leftEye, PointF rightEye, PointF mouse, PointF chin, float imgWidth, float imgHeight) {
            leftEyePos = leftEye;
            rightEyePos = rightEye;
            mousePos = mouse;
            chinPos = chin;
            faceImageWidth = imgWidth;
            faceImageHeight = imgHeight;
        }

        PointF leftEyePos, rightEyePos;
        PointF mousePos;
        PointF chinPos;
        float faceImageWidth, faceImageHeight;
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
                srcFeature.mousePos.x, srcFeature.mousePos.y,
                srcFeature.chinPos.x, srcFeature.chinPos.y,
                srcFeature.faceImageWidth, srcFeature.faceImageHeight,
        };

        float[] dstFaceFeature = {
                dstFeature.leftEyePos.x, dstFeature.leftEyePos.y,
                dstFeature.rightEyePos.x, dstFeature.rightEyePos.y,
                dstFeature.mousePos.x, dstFeature.mousePos.y,
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
