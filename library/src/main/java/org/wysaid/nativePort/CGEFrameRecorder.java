package org.wysaid.nativePort;

import android.util.Log;

import org.wysaid.common.Common;

import java.nio.ShortBuffer;

/**
 * Created by wangyang on 15/7/29.
 */

//A wrapper for native class FrameRecorder
public class CGEFrameRecorder extends CGEFrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    /**
     * Video quality presets for adaptive bitrate calculation
     */
    public enum VideoQuality {
        LOW,      // Suitable for network sharing, smaller file size
        MEDIUM,   // Balance between quality and size
        HIGH,     // High quality (recommended for most use cases)
        ULTRA     // Ultra high quality (near lossless)
    }

    // Track output dimensions for bitrate calculation
    private int mOutputWidth = 0;
    private int mOutputHeight = 0;

    public CGEFrameRecorder() {
        super(0); //avoid multiple creation.
        mNativeAddress = nativeCreateRecorder();
    }

    @Override
    public boolean init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        boolean result = super.init(srcWidth, srcHeight, dstWidth, dstHeight);
        if (result) {
            mOutputWidth = dstWidth;
            mOutputHeight = dstHeight;
        }
        return result;
    }

    /**
     * Calculate recommended bitrate based on resolution and quality preset
     * @param width video width
     * @param height video height
     * @param quality quality preset
     * @return recommended bitrate in bps
     */
    private int calculateBitrate(int width, int height, VideoQuality quality) {
        int baseBitrate = getBaseBitrate(width, height);

        // Adjust based on quality setting
        switch (quality) {
            case LOW:
                return (int) (baseBitrate * 0.5);   // 50%
            case MEDIUM:
                return (int) (baseBitrate * 0.75);  // 75%
            case HIGH:
                return baseBitrate;                  // 100%
            case ULTRA:
                return (int) (baseBitrate * 1.5);   // 150%
            default:
                return baseBitrate;
        }
    }

    private static int getBaseBitrate(int width, int height) {
        int pixels = width * height;
        int baseBitrate;

        // Base bitrate based on resolution
        if (pixels <= 640 * 480) {           // VGA (~0.3 MP)
            baseBitrate = 2_000_000;         // 2 Mbps
        } else if (pixels <= 1280 * 720) {   // 720p (~0.9 MP)
            baseBitrate = 5_000_000;         // 5 Mbps
        } else if (pixels <= 1920 * 1080) {  // 1080p (~2 MP)
            baseBitrate = 10_000_000;        // 10 Mbps
        } else if (pixels <= 2560 * 1440) {  // 1440p (~3.7 MP)
            baseBitrate = 16_000_000;        // 16 Mbps
        } else {                              // 4K+ (8 MP+)
            baseBitrate = 40_000_000;        // 40 Mbps
        }
        return baseBitrate;
    }

    ///////////////// Video recording related ////////////////////

    /**
     * Start recording with adaptive bitrate based on resolution (HIGH quality)
     * For custom quality, use {@link #startRecording(int, String, VideoQuality)}
     * @param fps frames per second
     * @param filename output file path
     * @return true if recording started successfully
     * @deprecated Default bitrate changed from 1.65 Mbps to adaptive HIGH (~10 Mbps for 1080p).
     *             Use {@link #startRecording(int, String, VideoQuality)} for explicit control.
     */
    @Deprecated
    public boolean startRecording(int fps, String filename) {
        Log.w(Common.LOG_TAG,
            "startRecording: Using HIGH quality preset (~10Mbps for 1080p). " +
            "Previous default was 1.65Mbps. Use startRecording(fps, filename, VideoQuality) for explicit control.");
        return startRecording(fps, filename, VideoQuality.HIGH);
    }

    /**
     * Start recording with specified quality preset (recommended)
     * @param fps frames per second
     * @param filename output file path
     * @param quality video quality preset (LOW/MEDIUM/HIGH/ULTRA)
     * @return true if recording started successfully
     */
    public boolean startRecording(int fps, String filename, VideoQuality quality) {
        // Use default 1080p if dimensions not set
        int width = (mOutputWidth > 0) ? mOutputWidth : 1920;
        int height = (mOutputHeight > 0) ? mOutputHeight : 1080;
        
        if (mOutputWidth <= 0 || mOutputHeight <= 0) {
            Log.w(Common.LOG_TAG,
                String.format("startRecording: Output dimensions not initialized, defaulting to %dx%d. " +
                    "Call init() or use startRecording(fps, filename, width, height, quality) for accurate bitrate.",
                    width, height));
        }
        
        int bitrate = calculateBitrate(width, height, quality);
        return startRecording(fps, bitrate, filename);
    }

    /**
     * Start recording with specified quality and explicit dimensions
     * @param fps frames per second
     * @param filename output file path
     * @param width video width (for bitrate calculation)
     * @param height video height (for bitrate calculation)
     * @param quality video quality preset
     * @return true if recording started successfully
     */
    public boolean startRecording(int fps, String filename, int width, int height, VideoQuality quality) {
        int bitrate = calculateBitrate(width, height, quality);
        return startRecording(fps, bitrate, filename);
    }

    /**
     * Start recording with manual bitrate specification (advanced usage)
     * @param fps frames per second
     * @param bitRate bitrate in bps
     * @param filename output file path
     * @return true if recording started successfully
     */
    public boolean startRecording(int fps, int bitRate, String filename) {
        if(mNativeAddress != 0)
            return nativeStartRecording(mNativeAddress, fps, filename, bitRate);
        return false;
    }

    /**
     * Get recommended bitrate for specified quality
     * @param quality video quality preset
     * @return recommended bitrate in bps
     */
    public int getRecommendedBitrate(VideoQuality quality) {
        int width = (mOutputWidth > 0) ? mOutputWidth : 1920;
        int height = (mOutputHeight > 0) ? mOutputHeight : 1080;
        
        if (mOutputWidth <= 0 || mOutputHeight <= 0) {
            Log.w(Common.LOG_TAG,
                String.format("getRecommendedBitrate: Output dimensions not initialized, using default %dx%d. " +
                    "Call init() first for resolution-specific bitrate.",
                    width, height));
        }
        
        return calculateBitrate(width, height, quality);
    }

    public boolean isRecordingStarted() {
        if(mNativeAddress != 0)
            return nativeIsRecordingStarted(mNativeAddress);
        return false;
    }

    public boolean endRecording(boolean shouldSave) {
        if(mNativeAddress != 0)
            return nativeEndRecording(mNativeAddress, shouldSave);
        return false;
    }

    public void pauseRecording() {
        if(mNativeAddress != 0)
            nativePauseRecording(mNativeAddress);
    }

    //Not completed by now
//    public boolean isRecordingPaused() {
//        if(mNativeAddress != 0)
//            return nativeIsRecordingPaused(mNativeAddress);
//        return false;
//    }
//
//    public boolean resumeRecording() {
//        if(mNativeAddress != 0)
//            return nativeResumeRecording(mNativeAddress);
//        return false;
//    }

    public double getTimestamp() {
        if(mNativeAddress != 0)
            return nativeGetTimestamp(mNativeAddress);
        return 0.0;
    }

    public double getVideoStreamtime() {
        if(mNativeAddress != 0)
            return nativeGetVideoStreamtime(mNativeAddress);
        return 0.0;
    }

    public double getAudioStreamtime() {
        if(mNativeAddress != 0)
            return nativeGetAudioStreamtime(mNativeAddress);
        return 0.0;
    }

    public void setTempDir(String dir) {
        if(mNativeAddress != 0)
            nativeSetTempDir(mNativeAddress, dir);
    }

    //需要置于GPU绘制线程
    public void recordImageFrame() {
        if(mNativeAddress != 0)
            nativeRecordImageFrame(mNativeAddress);
    }

    //需要自行loop
    public void recordAudioFrame(ShortBuffer audioBuffer, int bufferLen) {
        if(mNativeAddress != 0)
            nativeRecordAudioFrame(mNativeAddress, audioBuffer, bufferLen);
    }

    public void setGlobalFilter(String config) {
        if(mNativeAddress != 0)
            nativeSetGlobalFilter(mNativeAddress, config);
    }

    public void setBeautifyFilter() {
        if(mNativeAddress != 0)
            nativeSetBeautifyFilter(mNativeAddress);
    }

    public void setGlobalFilterIntensity(float intensity) {
        if(mNativeAddress != 0)
            nativeSetGlobalFilterIntensity(mNativeAddress, intensity);
    }

    public void isGlobalFilterEnabled() {
        if(mNativeAddress != 0)
            nativeIsGlobalFilterEnabled(mNativeAddress);
    }

    /////////////////      private         ///////////////////////

    private native long nativeCreateRecorder();

    ///////////////// Video recording related ////////////////////
    private native boolean nativeStartRecording(long holder, int fps, String filename, int bitRate);
    private native boolean nativeIsRecordingStarted(long holder);
    private native boolean nativeEndRecording(long holder, boolean shouldSave);
    private native void nativePauseRecording(long holder);
//    private native boolean nativeIsRecordingPaused(long holder);
//    private native boolean nativeResumeRecording(long holder);
    private native double nativeGetTimestamp(long holder);

    private native double nativeGetVideoStreamtime(long holder);
    private native double nativeGetAudioStreamtime(long holder);

    private native void nativeSetTempDir(long holder, String dir);

    private native void nativeRecordImageFrame(long holder);
    private native void nativeRecordAudioFrame(long holder, ShortBuffer audioBuffer, int bufferLen);

    private native void nativeSetGlobalFilter(long holder, String config);
    private native void nativeSetBeautifyFilter(long holder);
    private native void nativeSetGlobalFilterIntensity(long holder, float intensity);
    private native void nativeIsGlobalFilterEnabled(long holder);
}
