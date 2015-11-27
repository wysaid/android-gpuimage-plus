package org.wysaid.nativePort;

//import java.nio.ByteBuffer;
//import java.nio.ShortBuffer;

/**
 * Created by wangyang on 15/7/29.
 */

//A wrapper for native class FrameRecorder
public class CGEFrameRecorder extends CGEFrameRenderer {

//    static {
//        NativeLibraryLoader.load();
//    }

//    public CGEFrameRecorder() {
//        super(0); //防止多次初始化
//        mNativeAddress = nativeCreateRecorder();
//    }

//    public void enableFaceDetectWithDefaultFilter(boolean shouldDetect) {
//        if(shouldDetect) {
//            nativeSetTrackedFilter(mNativeAddress, "@beautify bilateral 100 6.8 1 @adjust brightness 0.14 @adjust contrast 1.12 ");
//        } else {
//            nativeSetTrackedFilter(mNativeAddress, "");
//        }
//    }
//
//    public void setTrackedFilterIntensity(float intensity) {
//        nativeSetTrackedFilterIntensity(mNativeAddress, intensity);
//    }
//
//    public void setFaceArea(float x, float y, float gx, float gy) {
//        nativeSetFaceArea(mNativeAddress, x, y, gx, gy);
//    }

    /////////////////视频录制相关////////////////////

//    public boolean startRecording(int fps, String filename) {
//        if(mNativeAddress != null)
//            return nativeStartRecording(mNativeAddress, fps, filename);
//        return false;
//    }
//
//    public boolean isRecordingStarted() {
//        if(mNativeAddress != null)
//            return nativeIsRecordingStarted(mNativeAddress);
//        return false;
//    }
//
//    public boolean endRecording(boolean shouldSave) {
//        if(mNativeAddress != null)
//            return nativeEndRecording(mNativeAddress, shouldSave);
//        return false;
//    }
//
//    public void pauseRecording() {
//        if(mNativeAddress != null)
//            nativePauseRecording(mNativeAddress);
//    }
//
//    public boolean isRecordingPaused() {
//        if(mNativeAddress != null)
//            return nativeIsRecordingPaused(mNativeAddress);
//        return false;
//    }
//
//    public boolean resumeRecording() {
//        if(mNativeAddress != null)
//            return nativeResumeRecording(mNativeAddress);
//        return false;
//    }
//
//    public double getTimestamp() {
//        if(mNativeAddress != null)
//            return nativeGetTimestamp(mNativeAddress);
//        return 0.0;
//    }
//
//    public double getVideoStreamtime() {
//        if(mNativeAddress != null)
//            return nativeGetVideoStreamtime(mNativeAddress);
//        return 0.0;
//    }
//
//    public double getAudioStreamtime() {
//        if(mNativeAddress != null)
//            return nativeGetAudioStreamtime(mNativeAddress);
//        return 0.0;
//    }
//
//    public void setTempDir(String dir) {
//        if(mNativeAddress != null)
//            nativeSetTempDir(mNativeAddress, dir);
//    }
//
//    //需要置于GPU绘制线程
//    public void recordImageFrame() {
//        if(mNativeAddress != null)
//            nativeRecordImageFrame(mNativeAddress);
//    }
//
//    //需要自行loop
//    public void recordAudioFrame(ShortBuffer audioBuffer, int bufferLen) {
//        if(mNativeAddress != null)
//            nativeRecordAudioFrame(mNativeAddress, audioBuffer, bufferLen);
//    }

    /////////////////      private         ///////////////////////

//    private native ByteBuffer nativeCreateRecorder();

    //人脸美化相关

//    private native void nativeSetTrackedFilter(ByteBuffer holder, String config);
//    private native void nativeSetTrackedFilterIntensity(ByteBuffer holder, float intensity);
//    private native void nativeSetFaceArea(ByteBuffer holder, float x, float y, float gx, float gy);

    /////////////////视频录制相关////////////////////
//    private native boolean nativeStartRecording(ByteBuffer holder, int fps, String filename);
//    private native boolean nativeIsRecordingStarted(ByteBuffer holder);
//    private native boolean nativeEndRecording(ByteBuffer holder, boolean shouldSave);
//    private native void nativePauseRecording(ByteBuffer holder);
//    private native boolean nativeIsRecordingPaused(ByteBuffer holder);
//    private native boolean nativeResumeRecording(ByteBuffer holder);
//    private native double nativeGetTimestamp(ByteBuffer holder);
//
//    private native double nativeGetVideoStreamtime(ByteBuffer holder);
//    private native double nativeGetAudioStreamtime(ByteBuffer holder);
//
//    private native void nativeSetTempDir(ByteBuffer holder, String dir);
//
//    private native void nativeRecordImageFrame(ByteBuffer holder);
//    private native void nativeRecordAudioFrame(ByteBuffer holder, ShortBuffer audioBuffer, int bufferLen);

}
