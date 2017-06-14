package org.wysaid.myUtils;

import android.graphics.Bitmap;
import android.media.FaceDetector;
import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by wangyang on 15/7/27.
 */

public class ImageUtil extends FileUtil {


    public static String saveBitmap(Bitmap bmp) {
        String path = getPath();
        long currentTime = System.currentTimeMillis();
        String filename = path + "/" + currentTime + ".jpg";
        return saveBitmap(bmp, filename);
    }

    public static String saveBitmap(Bitmap bmp, String filename) {

        Log.i(LOG_TAG, "saving Bitmap : " + filename);

        try {
            FileOutputStream fileout = new FileOutputStream(filename);
            BufferedOutputStream bufferOutStream = new BufferedOutputStream(fileout);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, bufferOutStream);
            bufferOutStream.flush();
            bufferOutStream.close();
        } catch (IOException e) {
            Log.e(LOG_TAG, "Err when saving bitmap...");
            e.printStackTrace();
            return null;
        }

        Log.i(LOG_TAG, "Bitmap " + filename + " saved!");
        return filename;
    }

    public static class FaceRects {
        public int numOfFaces; // 实际检测出的人脸数
        public FaceDetector.Face[] faces; // faces.length >= numOfFaces
    }

    public static FaceRects findFaceByBitmap(Bitmap bmp) {
        return findFaceByBitmap(bmp, 1);
    }

    public static FaceRects findFaceByBitmap(Bitmap bmp, int maxFaces) {

        if(bmp == null) {
            Log.e(LOG_TAG, "Invalid Bitmap for Face Detection!");
            return null;
        }

        Bitmap newBitmap = bmp;

        //人脸检测API 仅支持 RGB_565 格式当图像. (for now)
        if(newBitmap.getConfig() != Bitmap.Config.RGB_565) {
            newBitmap = newBitmap.copy(Bitmap.Config.RGB_565, false);
        }

        FaceRects rects = new FaceRects();
        rects.faces = new FaceDetector.Face[maxFaces];

        try {
            FaceDetector detector = new FaceDetector(newBitmap.getWidth(), newBitmap.getHeight(), maxFaces);
            rects.numOfFaces = detector.findFaces(newBitmap, rects.faces);
        } catch (Exception e) {
            Log.e(LOG_TAG, "findFaceByBitmap error: " + e.getMessage());
            return null;
        }


        if(newBitmap != bmp) {
            newBitmap.recycle();
        }
        return rects;
    }



}

