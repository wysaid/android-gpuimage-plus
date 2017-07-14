package com.sensetime.stmobileapi;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.sensetime.stmobileapi.STMobileApiBridge.ResultCode;
import com.sensetime.stmobileapi.STMobileApiBridge.st_mobile_106_t;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

public class STMobileFaceDetection {
	private Pointer detectHandle;
	private Context mContext;
	private static boolean DEBUG = true;// false;
	private String TAG = "FaceDetection";
	private static final String DETECTION_MODEL_NAME = "track_compose.model";
	public static int ST_MOBILE_DETECT_DEFAULT_CONFIG = 0x00000000;  ///< 默认选项
	public static int ST_MOBILE_DETECT_FAST = 0x00000001;  ///< resize图像为长边320的图像之后再检测，结果处理为原图像对应结果
	public static int ST_MOBILE_DETECT_BALANCED = 0x00000002;  ///< resize图像为长边640的图像之后再检测，结果处理为原图像对应结果
	public static int ST_MOBILE_DETECT_ACCURATE = 0x00000004;
	
    PointerByReference ptrToArray = new PointerByReference();
    IntByReference ptrToSize = new IntByReference();

    public STMobileFaceDetection(Context context, int config) {
        PointerByReference handlerPointer = new PointerByReference();
		mContext = context;
		synchronized(this.getClass())
		{
		   copyModelIfNeed(DETECTION_MODEL_NAME);
		}
		String modulePath = getModelPath(DETECTION_MODEL_NAME);
    	int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_create(modulePath, config, handlerPointer);
    	if(rst != ResultCode.ST_OK.getResultCode())
    	{
    		return;
    	}
    	detectHandle = handlerPointer.getValue();
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
						Log.e(TAG, "the src module is not existed");
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
	
	public void destory()
	{
    	long start_destroy = System.currentTimeMillis();
    	if(detectHandle != null) {
    		STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_destroy(detectHandle);
    		detectHandle = null;
    	}
        long end_destroy = System.currentTimeMillis();
        Log.i(TAG, "destroy cost "+(end_destroy - start_destroy)+" ms");
	}
	
    /**
     * Given the Image by Bitmap to detect face
     * @param image Input image by Bitmap
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] detect(Bitmap image, int orientation) {
    	if(DEBUG) 
    		Log.d(TAG, "detect bitmap");
    	
        int[] colorImage = STUtils.getBGRAImageByte(image);
        return detect(colorImage, STImageFormat.ST_PIX_FMT_BGRA8888,image.getWidth(), image.getHeight(), image.getWidth(), orientation);
    }
    
    /**
     * Given the Image by Byte Array to detect face
     * @param colorImage Input image by int
     * @param cvImageFormat Image format
     * @param imageWidth Image width
     * @param imageHeight Image height
     * @param imageStride Image stride
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] detect(int[] colorImage,int cvImageFormat, int imageWidth, int imageHeight, int imageStride, int orientation) {
    	if(DEBUG)
    		Log.d(TAG, "detect int array");
    	
    	if(detectHandle == null){
    		return null;
    	}
        long startTime = System.currentTimeMillis();

        int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_detect(detectHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
        long endTime = System.currentTimeMillis();
        
        if(DEBUG)Log.d(TAG, "detect time: "+(endTime-startTime)+"ms");
        
        if (rst != ResultCode.ST_OK.getResultCode()) {
            throw new RuntimeException("Calling st_mobile_face_detection_detect() method failed! ResultCode=" + rst);
        }

        if (ptrToSize.getValue() == 0) {
        	if(DEBUG)Log.d(TAG, "ptrToSize.getValue() == 0");
            return new STMobile106[0];
        }

        st_mobile_106_t arrayRef = new st_mobile_106_t(ptrToArray.getValue());
        arrayRef.read();
        st_mobile_106_t[] array = st_mobile_106_t.arrayCopy((st_mobile_106_t[]) arrayRef.toArray(ptrToSize.getValue()));
        STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_release_result(ptrToArray.getValue(), ptrToSize.getValue());
        
        STMobile106[] ret = new STMobile106[array.length]; 
        for (int i = 0; i < array.length; i++) {
        	ret[i] = new STMobile106(array[i]);
        }
        
        if(DEBUG)Log.d(TAG, "track : "+ ret);
        
        return ret;
    }
    
    /**
     * Given the Image by Byte to detect face
     * @param image Input image by byte
     * @param orientation Image orientation
     * @param width Image width
     * @param height Image height
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] detect(byte[] image, int orientation,int width,int height) {
    	if(DEBUG){
    		Log.d(TAG, "detect byte array");
    	}
    	
        return detect(image, STImageFormat.ST_PIX_FMT_NV21,width, height, width, orientation);
    }

    /**
     * Given the Image by Byte Array to detect face
     * @param colorImage Input image by byte
     * @param cvImageFormat Image format
     * @param imageWidth Image width
     * @param imageHeight Image height
     * @param imageStride Image stride
     * @param orientation Image orientation
     * @return CvFace array, each one in array is Detected by SDK native API
     */
    public STMobile106[] detect(byte[] colorImage,int cvImageFormat, int imageWidth, int imageHeight, int imageStride, int orientation) {
    	if(DEBUG){
    		Log.d(TAG, "detect 111");
    	}
    	
    	if(detectHandle == null){
    		return null;
    	}
        long startTime = System.currentTimeMillis();

        int rst = STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_detect(detectHandle, colorImage, cvImageFormat,imageWidth,
                imageHeight, imageStride, orientation, ptrToArray, ptrToSize);
        long endTime = System.currentTimeMillis();
        
        if(DEBUG)Log.d(TAG, "detect time: "+(endTime-startTime)+"ms");
        
        if (rst != ResultCode.ST_OK.getResultCode()) {
            throw new RuntimeException("Calling st_mobile_face_detection_detect() method failed! ResultCode=" + rst);
        }

        if (ptrToSize.getValue() == 0) {
            return new STMobile106[0];
        }

        st_mobile_106_t arrayRef = new st_mobile_106_t(ptrToArray.getValue());
        arrayRef.read();
        st_mobile_106_t[] array = st_mobile_106_t.arrayCopy((st_mobile_106_t[]) arrayRef.toArray(ptrToSize.getValue()));
        STMobileApiBridge.FACESDK_INSTANCE.st_mobile_face_detection_release_result(ptrToArray.getValue(), ptrToSize.getValue());
        
        STMobile106[] ret = new STMobile106[array.length]; 
        for (int i = 0; i < array.length; i++) {
        	ret[i] = new STMobile106(array[i]);
        }
        
        if(DEBUG)Log.d(TAG, "track : "+ ret);
        
        return ret;
    }
}
