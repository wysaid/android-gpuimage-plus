package com.sensetime.stmobileapi;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.sensetime.stmobileapi.STMobileApiBridge.ResultCode;
import com.sensetime.stmobileapi.STMobileApiBridge.st_mobile_106_t;
import com.sensetime.stmobileapi.STMobileApiBridge.st_mobile_face_action_t;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;

public class STMobileMultiTrack106 {
	private Pointer trackHandle;
    public static final int FACE_KEY_POINTS_COUNT = 106;
    static boolean DEBUG = true;// false;
    
    public static int ST_MOBILE_TRACKING_DEFAULT_CONFIG = 0x00000000;
    public static int ST_MOBILE_TRACKING_SINGLE_THREAD = 0x00000001;
    
	private Context mContext;
	private static final String BEAUTIFY_MODEL_NAME = "track_face_action1.0.0.model";
      
    PointerByReference ptrToArray = new PointerByReference();
	PointerByReference faceAction_ptrToArray = new PointerByReference();

    IntByReference ptrToSize = new IntByReference();		//face count int pointer

    /**
     * 
     * Note
        track only one face： 
        frist:trackHandle = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_create(modulePath, handlerPointer);
        second: setMaxDetectableFaces(1)参数设为1
     *  track多张人脸：	
     *  trackHandle = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_create(modulePath, handlerPointer);
        second:setMaxDetectableFaces(num)参数设为-1
     */
     
    public STMobileMultiTrack106(Context context, int config) {
        PointerByReference handlerPointer = new PointerByReference();
		mContext = context;
		synchronized(this.getClass())
		{
		   copyModelIfNeed(BEAUTIFY_MODEL_NAME);
		}
		String modulePath = getModelPath(BEAUTIFY_MODEL_NAME);
    	int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_create(modulePath, config, handlerPointer);
    	if(rst != ResultCode.ST_OK.getResultCode())
    	{
    		return;
    	}
    	trackHandle = handlerPointer.getValue();
    }
    
	private void copyModelIfNeed(String modelName) {
		String path = getModelPath(modelName);
		if (path != null) {
			File modelFile = new File(path);
			if (!modelFile.exists()) {
				//如果模型文件不存在或者当前模型文件的版本跟sdcard中的版本不一样
				try {
					if (modelFile.exists())
						modelFile.delete();
					modelFile.createNewFile();
					InputStream in = mContext.getApplicationContext().getAssets().open(modelName);
					if(in == null)
					{
						Log.e("MultiTrack106", "the src module is not existed");
					}
					OutputStream out = new FileOutputStream(modelFile);
					byte[] buffer = new byte[4096];
					int n;
					while ((n = in.read(buffer)) > 0) {
						out.write(buffer, 0, n);
					}
					in.close();
					out.close();
				} catch (IOException e) {
					modelFile.delete();
				}
			}
		}
	}
	
	protected String getModelPath(String modelName) {
		String path = null;
		File dataDir = mContext.getApplicationContext().getExternalFilesDir(null);
		if (dataDir != null) {
			path = dataDir.getAbsolutePath() + File.separator + modelName;
		}
		return path;
	}
    
    public int setMaxDetectableFaces(int max)
    {
    	int rst = -1;
    	if(trackHandle != null){
    	rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_set_facelimit(trackHandle,max);
    	}
        return rst;
    }
    
	public void destory()
	{
    	long start_destroy = System.currentTimeMillis();
    	if(trackHandle != null) {
    		STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_destroy(trackHandle);
    		trackHandle = null;
    	}
        long end_destroy = System.currentTimeMillis();
        Log.i("track106", "destroy cost "+(end_destroy - start_destroy)+" ms");
	}
    /**
     * Given the Image by Bitmap to track face
     * @param image Input image by Bitmap
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] track(Bitmap image, int orientation) {
    	if(DEBUG)System.out.println("SampleLiveness-------->CvFaceMultiTrack--------->track1");
    	
        int[] colorImage = STUtils.getBGRAImageByte(image);
        return track(colorImage, STImageFormat.ST_PIX_FMT_BGRA8888,image.getWidth(), image.getHeight(), image.getWidth(), orientation);
    }

    /**
     * Given the Image by Byte Array to track face
     * @param colorImage Input image by int
     * @param cvImageFormat Image format
     * @param imageWidth Image width
     * @param imageHeight Image height
     * @param imageStride Image stride
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] track(int[] colorImage,int cvImageFormat, int imageWidth, int imageHeight, int imageStride, int orientation) {
    	if(DEBUG)System.out.println("SampleLiveness-------->CvFaceMultiTrack--------->track2");
    	
    	if(trackHandle == null){
    		return null;
    	}
        long startTime = System.currentTimeMillis();
        /*
        int rst = STMobileApiBridge.FACESDK_INSTANCE.cv_face_track_106(trackHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
        */
        int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_track(trackHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
        long endTime = System.currentTimeMillis();
        
        if(DEBUG)Log.d("Test", "multi track time: "+(endTime-startTime)+"ms");
        
        if (rst != ResultCode.ST_OK.getResultCode()) {
            throw new RuntimeException("Calling cv_face_multi_track() method failed! ResultCode=" + rst);
        }

        if (ptrToSize.getValue() == 0) {
        	if(DEBUG)Log.d("Test", "ptrToSize.getValue() == 0");
            return new STMobile106[0];
        }

        st_mobile_106_t arrayRef = new st_mobile_106_t(ptrToArray.getValue());
        arrayRef.read();
        st_mobile_106_t[] array = st_mobile_106_t.arrayCopy((st_mobile_106_t[]) arrayRef.toArray(ptrToSize.getValue()));
        STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_release_result(ptrToArray.getValue(), ptrToSize.getValue());
        
        STMobile106[] ret = new STMobile106[array.length]; 
        for (int i = 0; i < array.length; i++) {
        	ret[i] = new STMobile106(array[i]);
        }
        
        if(DEBUG)Log.d("Test", "track : "+ ret);
        
        return ret;
    }
    
    /**
     * Given the Image by Byte to track face
     * @param image Input image by byte
     * @param orientation Image orientation
     * @param width Image width
     * @param height Image height
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] track(byte[] image, int orientation,int width,int height) {
    	if(DEBUG){
    		System.out.println("SampleLiveness-------->CvFaceMultiTrack--------->track3");
    	}
    	
        return track(image, STImageFormat.ST_PIX_FMT_NV21,width, height, width, orientation);
    }

    /**
     * Given the Image by Byte Array to track face
     * @param colorImage Input image by byte
     * @param cvImageFormat Image format
     * @param imageWidth Image width
     * @param imageHeight Image height
     * @param imageStride Image stride
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] track(byte[] colorImage,int cvImageFormat, int imageWidth, int imageHeight, int imageStride, int orientation) {
		Log.d("ST_mobile", "track: "+imageWidth+" "+imageHeight+" "+imageStride+" "+orientation);
		if(DEBUG){
    		System.out.println("SampleLiveness-------->CvFaceMultiTrack--------->track4");
    	}
    	
    	if(trackHandle == null){
    		return null;
    	}
        long startTime = System.currentTimeMillis();
        /*
           int rst = STMobileApiBridge.FACESDK_INSTANCE.cv_face_track_106(trackHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
         */

        int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_track(trackHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
//		int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_track_face_action(trackHandle, colorImage, cvImageFormat, imageWidth,
//				imageHeight, imageStride, orientation, ptrToArray, ptrToSize);

        long endTime = System.currentTimeMillis();
        
        if(DEBUG)Log.d("Test", "multi track time: "+(endTime-startTime)+"ms");
        
        if (rst != ResultCode.ST_OK.getResultCode()) {
            throw new RuntimeException("Calling cv_face_multi_track() method failed! ResultCode=" + rst);
        }

        if (ptrToSize.getValue() == 0) {
            return new STMobile106[0];
        }

        st_mobile_106_t arrayRef = new st_mobile_106_t(ptrToArray.getValue());
        arrayRef.read();
        st_mobile_106_t[] array = st_mobile_106_t.arrayCopy((st_mobile_106_t[]) arrayRef.toArray(ptrToSize.getValue()));
        //STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_release_result(ptrToArray.getValue(), ptrToSize.getValue());
        
        STMobile106[] ret = new STMobile106[array.length]; 
        for (int i = 0; i < array.length; i++) {
        	ret[i] = new STMobile106(array[i]);
        }
        
        if(DEBUG)Log.d("Test", "track : "+ ret);
        
        return ret;
    }

	/**
	 * Given the Image by Byte to trace face action
	 * @param image Input image by byte
	 * @param orientation Image orientation
	 * @param width Image width
	 * @param height Image height
	 * @return CvFace action array, each one in array is Detected by SDK native API
	 * */
	public STMobileFaceAction[] trackFaceAction(byte[] image, int orientation, int width, int height) {
		Log.d("ST_mobile", "track: "+width+" "+height+" "+orientation);
		if(DEBUG) {
			System.out.println("SampleTrackFaceAction-------->CvFaceMultiTrack--------->trackFaceAction1");
		}
		return trackFaceAction(image, STImageFormat.ST_PIX_FMT_NV21, width, height, width, orientation);
	}

	/**
	 *  Given the Image by Byte Array to track face action
	 *  @param colorImage Input image by byte
	 *  @param cvImageFormat Image format
	 *  @param imageWidth Image width
	 *  @param imageHeight Image height
	 *  @param imageStride Image stride
	 *  @param orientation Image orientation
	 *  @return CvFace action array, each one in array is Detected by SDK native API
	 * */
	public STMobileFaceAction[] trackFaceAction(byte[] colorImage, int cvImageFormat, int imageWidth, int imageHeight, int imageStride, int orientation) {
		if(DEBUG) {
			System.out.println("SampleTrackFaceAction-------->CvFaceMultiTrack--------->trackFaceAction2");
		}

		if(trackHandle == null) {
			return null;
		}

		long startTime = System.currentTimeMillis();
		int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_tracker_106_track_face_action(trackHandle, colorImage, cvImageFormat, imageWidth,
				imageHeight, imageStride, orientation, faceAction_ptrToArray, ptrToSize);
		long endTime = System.currentTimeMillis();
		if(DEBUG)Log.d("trackFaceAction", "multi track face action time: "+(endTime-startTime)+"ms");

		if(rst != ResultCode.ST_OK.getResultCode()) {
			throw new RuntimeException("Calling cv_face_action_multi_track() method failed! ResultCode=" + rst);
		}

		if(ptrToSize.getValue() == 0) {
			return new STMobileFaceAction[0];
		}

		st_mobile_face_action_t arrayRef = new st_mobile_face_action_t(faceAction_ptrToArray.getValue());
		arrayRef.read();
		st_mobile_face_action_t[] array = st_mobile_face_action_t.arrayCopy((st_mobile_face_action_t[]) arrayRef.toArray(ptrToSize.getValue()));

		STMobileFaceAction[] ret = new STMobileFaceAction[array.length];
		for(int i=0; i<array.length; i++) {
			ret[i] = new STMobileFaceAction(array[i]);
		}

		if(DEBUG)Log.d("STMobileMultiTrack106", "face action track ret = "+ ret);

		return ret;
	}

}
