package com.sensetime.stmobileapi;

import java.io.ByteArrayOutputStream;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicYuvToRGB;
import android.renderscript.Type;
import android.util.TimingLogger;

/**
 * CvFaceSDK: Utils
 * Created by Guangli W on 9/7/15.
 */
public class STUtils {
    /**
     * Get the pixels by byte[] of Bitmap
     * @param image
	 * @return pixels by byte[]
     */
    public static int[] getBGRAImageByte(Bitmap image) {
        int width = image.getWidth();
        int height = image.getHeight();

		if(image.getConfig().equals(Config.ARGB_8888)) {
	        int[] imgData = new int[width * height];
	        image.getPixels(imgData, 0, width, 0, 0, width, height);
	        return imgData;
	       
//	        byte[] imgPixels = new byte[width * height];
//	        for (int i = 0; i < imgData.length; ++i) {
//	        	int p = 0;
//	        	//p += ((imgData[i] >> 24) & 0xFF);
//	        	p += ((imgData[i] >> 16) & 0xFF);
//	        	p += ((imgData[i] >> 8) & 0xFF);
//	        	p += ((imgData[i] >> 0) & 0xFF);
//	            imgPixels[i] = (byte) (p/3);
//	        }
		} else {
			// TODO
		}

        return null;
    }
    
    
    static RenderScript mRS = null;
    
    static ScriptIntrinsicYuvToRGB mYuvToRgb = null;
    
    static Allocation ain = null;
    
    static Allocation aOut = null;
    
    static Bitmap bitmap = null;
    
    static final String TIMING_LOG_TAG = "CvUtils timing";
    
	@SuppressLint("NewApi")
	public static Bitmap NV21ToRGBABitmap(byte []nv21, int width, int height, Context context) {
		
		TimingLogger timings = new TimingLogger(TIMING_LOG_TAG, "NV21ToRGBABitmap");
		
		Rect rect = new Rect(0, 0, width, height);
		
		try {
			Class.forName("android.renderscript.Element$DataKind").getField("PIXEL_YUV");
			Class.forName("android.renderscript.ScriptIntrinsicYuvToRGB");
	    	byte[] imageData = nv21;
	    	if (mRS == null) {
	    		mRS = RenderScript.create(context);
	    		mYuvToRgb = ScriptIntrinsicYuvToRGB.create(mRS, Element.U8_4(mRS));
	    		Type.Builder tb = new Type.Builder(mRS, Element.createPixel(mRS, Element.DataType.UNSIGNED_8, Element.DataKind.PIXEL_YUV));
	    		tb.setX(width);
	    		tb.setY(height);
	    		tb.setMipmaps(false);
	    		tb.setYuvFormat(ImageFormat.NV21);
	    		ain = Allocation.createTyped(mRS, tb.create(), Allocation.USAGE_SCRIPT);
	    		timings.addSplit("Prepare for ain");
	    		Type.Builder tb2 = new Type.Builder(mRS, Element.RGBA_8888(mRS));
	    		tb2.setX(width);
	    		tb2.setY(height);
	    		tb2.setMipmaps(false);
	    		aOut = Allocation.createTyped(mRS, tb2.create(), Allocation.USAGE_SCRIPT & Allocation.USAGE_SHARED);
	    		timings.addSplit("Prepare for aOut");
	    		bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
	    		timings.addSplit("Create Bitmap");
			}
	    	ain.copyFrom(imageData);
			timings.addSplit("ain copyFrom");
			mYuvToRgb.setInput(ain);
			timings.addSplit("setInput ain");
			mYuvToRgb.forEach(aOut);
			timings.addSplit("NV21 to ARGB forEach");
			aOut.copyTo(bitmap);
			timings.addSplit("Allocation to Bitmap");
		} catch (Exception e) {
			YuvImage yuvImage = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
			timings.addSplit("NV21 bytes to YuvImage");
			
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
	        yuvImage.compressToJpeg(rect, 90, baos);
	        byte[] cur = baos.toByteArray();
	        timings.addSplit("YuvImage crop and compress to Jpeg Bytes");
	        
	        bitmap = BitmapFactory.decodeByteArray(cur, 0, cur.length);
	        timings.addSplit("Jpeg Bytes to Bitmap");
		}
		
    	timings.dumpToLog();
    	return bitmap;
	}
    
	/**
	 * 将Android的特殊格式转化成ARGB彩图
	 * 
	 * @param NV21
	 * 输入的图片数据，Android读图特殊格式
	 * 
	 * @param width
	 * 输入图的宽度
	 * 
	 * @param height
	 * 输入图的高度
	 * 
	 * @return
	 * ARGB彩图
	 */
	static public Bitmap NV21ToRGBABitmap(byte []nv21, int width, int height) {
		YuvImage yuvImage = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
        yuvImage.compressToJpeg(new Rect(0, 0, width, height), 100, baos);
        byte[] cur = baos.toByteArray();
        return BitmapFactory.decodeByteArray(cur, 0, cur.length);
	}

    /**
     * Draw the face rect in the Image
     * @param canvas
     * @param face
     * @param width
     * @param height
     * @param frontCamera
     */
	static public void drawFaceRect(Canvas canvas, Rect rect, int width, int height, boolean frontCamera) {

		if(canvas == null) return;

		Paint paint = new Paint(); 
		paint.setColor(Color.rgb(57, 138, 243));
		int strokeWidth = Math.max(width / 240, 2);
		paint.setStrokeWidth(strokeWidth);

		if(frontCamera) {
			int left = rect.left;
			rect.left = width - rect.right;
			rect.right = width - left;
		}

		paint.setStyle(Style.STROKE);
		canvas.drawRect(rect, paint);
	}

    /**
     * Draw points in the Image
     * @param canvas
     * @param points array
     * @param width
     * @param height
     * @param frontCamera
     */
	static public void drawPoints(Canvas canvas, PointF[] points, int width, int height, boolean frontCamera) {

		if(canvas == null) return;

		Paint paint = new Paint();
		paint.setColor(Color.rgb(57, 138, 243));
		int strokeWidth = Math.max(width / 240, 2);
		paint.setStrokeWidth(strokeWidth);
		paint.setStyle(Style.STROKE);

		for (PointF point : points) {
			PointF p = point;
			if(frontCamera) {
				p.x = width - p.x;
			}
			canvas.drawPoint(p.x, p.y, paint);
		}
	}

    /**
     * rotate the coordinate in the image
     * @param rect
     * @param width, origin width in the image
     * @param height, origin height in the image
	 * @return rotated rect
     */
	static public Rect RotateDeg90(Rect rect, int width, int height, boolean frontCamera) {
		int left = rect.left;
		rect.left	= height- rect.bottom;
		rect.bottom= rect.right;
		rect.right	= height- rect.top;
		rect.top	= left;
		if (frontCamera) {
			left = rect.left;
			rect.left = height - rect.right;
			rect.right = height - left;
		}
		return rect;
	}
	
	static public Rect RotateDeg270(Rect rect, int width, int height, boolean frontCamera) {
		int left = rect.left;
		rect.left = rect.top;
		rect.top = width - rect.right;
		rect.right = rect.bottom;
		rect.bottom = width - left;
		if (frontCamera) {
			left = rect.left;
			rect.left = height - rect.right;
			rect.right = height - left;
		}
		return rect;
	}

    /**
     * rotate the coordinate in the image
     * @param point
     * @param width, origin width in the image
     * @param height, origin height in the image
	 * @return rotated PointF
     */
	static public PointF RotateDeg90(PointF point, int width, int height, boolean frontCamera) {
		float x = point.x;
		point.x = height - point.y;
		point.y = x;
		if(frontCamera) {
			point.x = height - point.x;
		}
		return point;
	}
	
	static public PointF RotateDeg270(PointF point, int width, int height, boolean frontCamera) {
		float x = point.x;
		point.x = point.y;
		point.y = width - x;
		if(frontCamera) {
			point.x = height - point.x;
		}
		return point;
	}
	
	public static Bitmap getRotateBitmap(Bitmap bitmap, int rotation){
		if(null == bitmap || bitmap.isRecycled()){
			return null;
		}
		
		Matrix matrix = new Matrix();
		matrix.postRotate(rotation);
		Bitmap cropBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(),bitmap.getHeight(), matrix, false);
		return cropBitmap;
	}
	
	public static void recycleBitmap(Bitmap bitmap){
		if(bitmap == null || bitmap.isRecycled()){
			return;
		}
		bitmap.recycle();
		bitmap = null;
	}
}
