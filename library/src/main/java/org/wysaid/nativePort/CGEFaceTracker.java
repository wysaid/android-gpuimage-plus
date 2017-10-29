package org.wysaid.nativePort;

import android.graphics.Bitmap;
import android.graphics.PointF;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * Created by wysaid on 16/2/23.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */
public class CGEFaceTracker {

    static {
        //You can ignore the tracker libraries below(for decreasing your package size, just delete the .so file below), if you don't want any face features.
//        System.loadLibrary("opencv_java3");  // OpenCV is statically built into FaceTracker now.
        System.loadLibrary("FaceTracker");
    }

    //临时处理， 后续将扩展更复杂的操作
    public static class FaceResultSimple {

        public PointF leftEyePos, rightEyepos; //左眼&右眼位置
        public PointF nosePos; //鼻子正中间
        public PointF mouthPos; //嘴巴正中间
        public PointF jawPos; //下巴最下面那个点
    }

    protected static boolean sIsTrackerSetup = false;

    public static boolean isTrackerSetup() {
        return sIsTrackerSetup;
    }

    protected long mNativeAddress;

    private CGEFaceTracker() {
        mNativeAddress = nativeCreateFaceTracker();
    }

    public static CGEFaceTracker createFaceTracker() {

        if(!sIsTrackerSetup) {
            nativeSetupTracker(null, null, null);
            sIsTrackerSetup = true;
        }

        return new CGEFaceTracker();
    }

    public void release() {
        if(mNativeAddress != 0) {
            nativeRelease(mNativeAddress);
            mNativeAddress = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    //Send your own tracking file, face tracking ref: https://github.com/kylemcdonald/FaceTracker
//    public static void setupTracker(Context context) {
//        setupTracker(context, "face2.tracker", "face.tri", "face.con");
//    }
//
//    public static void setupTracker(Context context, String modelFile, String triFile, String conFile) {
//
//        if(sIsTrackerSetup) {
//            Log.e(Common.LOG_TAG, "The tracker is already setup!");
//            return;
//        }
//
//        AssetManager am = context.getAssets();
//        String pathInPackage = FileUtil.getPathInPackage(context, false);
//        String model = pathInPackage + "/" + modelFile;
//        String tri = pathInPackage + "/" + triFile;
//        String con = pathInPackage + "/" + conFile;
//
//        try {
//
//            InputStream modelStream = am.open(modelFile);
//            FileUtil.saveStreamContent(modelStream, model);
//            modelStream.close();
//
//            InputStream triStream = am.open(triFile);
//            FileUtil.saveStreamContent(triStream, tri);
//            triStream.close();
//
//            InputStream conStream = am.open(conFile);
//            FileUtil.saveStreamContent(conStream, con);
//            conStream.close();
//
//        } catch (Exception e) {
//            Log.e(Common.LOG_TAG, "Can not setup face tracker!");
//            return;
//        }
//
//        nativeSetupTracker(model, tri, con);
//
//        nativeSetupTracker(null, null, null);
//        Log.i(Common.LOG_TAG, "Face Tracker is set!");
//        sIsTrackerSetup = true;
//    }

    //命名Simple 是因为后续将接入更加完整的结果数据(包含66个点的网格等等)
    public FaceResultSimple detectFaceWithSimpleResult(Bitmap bmp, boolean drawFeature) {
        float[] result = nativeDetectFaceWithSimpleResult(mNativeAddress, bmp, drawFeature);

        if(result == null) {
            return null;
        }

        FaceResultSimple faceResultSimple = new FaceResultSimple();

        faceResultSimple.leftEyePos = new PointF(result[0], result[1]);
        faceResultSimple.rightEyepos = new PointF(result[2], result[3]);
        faceResultSimple.nosePos = new PointF(result[4], result[5]);
        faceResultSimple.mouthPos = new PointF(result[6], result[7]);
        faceResultSimple.jawPos = new PointF(result[8], result[9]);

        return faceResultSimple;
    }

    /////////// for video frames

    public static class FaceResult {
        // 66 key points.
        public FloatBuffer faceKeyPoints = ByteBuffer.allocateDirect(66 * 8).order(ByteOrder.nativeOrder()).asFloatBuffer();
    }

    protected FaceResult mFaceResult = new FaceResult();

    public FaceResult getFaceResult() {
        return mFaceResult;
    }

    //recommended
    public boolean detectFaceWithGrayBuffer(ByteBuffer buffer, int width, int height, int bytesPerRow) {
        return nativeDetectFaceWithBuffer(mNativeAddress, buffer, width, height, 1, bytesPerRow, mFaceResult.faceKeyPoints);
    }

    public boolean detectFaceWithBGRABuffer(ByteBuffer buffer, int width, int height, int bytesPerRow) {
        return nativeDetectFaceWithBuffer(mNativeAddress, buffer, width, height, 4, bytesPerRow, mFaceResult.faceKeyPoints);
    }

    public boolean detectFaceWithBGRBuffer(ByteBuffer buffer, int width, int height, int bytesPerRow) {
        return nativeDetectFaceWithBuffer(mNativeAddress, buffer, width, height, 3, bytesPerRow, mFaceResult.faceKeyPoints);
    }

    ////////////////////////////////////////

    //static
    private static native void nativeSetupTracker(String modelFile, String triFile, String conFile);

    //non-static
    protected native long nativeCreateFaceTracker();
    protected native void nativeRelease(long addr);
    protected native float[] nativeDetectFaceWithSimpleResult(long addr, Bitmap bmp, boolean drawFeature);

    protected native boolean nativeDetectFaceWithBuffer(long addr, ByteBuffer buffer, int w, int h, int channel, int bytesPerRow, FloatBuffer outputBuffer);


}
