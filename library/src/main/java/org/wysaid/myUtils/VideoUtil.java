package org.wysaid.myUtils;

import android.app.Activity;
import android.content.ContentValues;
import android.hardware.Camera;
import android.os.Build;
import android.view.Surface;

import java.util.Comparator;
import java.util.List;

/**
 * Created by wangyang on 15/7/27.
 */

public class VideoUtil {

    public static ContentValues videoContentValues = null;

    public static int determineDisplayOrientation(Activity activity, int defaultCameraId) {
        int displayOrientation = 0;
        if(Build.VERSION.SDK_INT >  Build.VERSION_CODES.FROYO)
        {
            Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
            Camera.getCameraInfo(defaultCameraId, cameraInfo);

            int degrees  = getRotationAngle(activity);


            if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                displayOrientation = (cameraInfo.orientation + degrees) % 360;
                displayOrientation = (360 - displayOrientation) % 360;
            } else {
                displayOrientation = (cameraInfo.orientation - degrees + 360) % 360;
            }
        }
        return displayOrientation;
    }

    public static int getRotationAngle(Activity activity)
    {
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees  = 0;

        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;

            case Surface.ROTATION_90:
                degrees = 90;
                break;

            case Surface.ROTATION_180:
                degrees = 180;
                break;

            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }
        return degrees;
    }
//
//    public static String createImagePath(Context context){
//        long dateTaken = System.currentTimeMillis();
//        String title = CONSTANTS.FILE_START_NAME + dateTaken;
//        String filename = title + CONSTANTS.IMAGE_EXTENSION;
//
//        String dirPath = Environment.getExternalStorageDirectory()+"/Android/data/" + context.getPackageName()+"/video";
//        File file = new File(dirPath);
//        if(!file.exists() || !file.isDirectory())
//            file.mkdirs();
//        String filePath = dirPath + "/" + filename;
//        return filePath;
//    }
//
//    public static String createFinalPath(Context context)
//    {
//        long dateTaken = System.currentTimeMillis();
//        String title = CONSTANTS.FILE_START_NAME + dateTaken;
//        String filename = title + CONSTANTS.VIDEO_EXTENSION;
//        String filePath = genrateFilePath(context,String.valueOf(dateTaken), true, null);
//
//        ContentValues values = new ContentValues(7);
//        values.put(MediaStore.Video.Media.TITLE, title);
//        values.put(MediaStore.Video.Media.DISPLAY_NAME, filename);
//        values.put(MediaStore.Video.Media.DATE_TAKEN, dateTaken);
//        values.put(MediaStore.Video.Media.MIME_TYPE, "video/3gpp");
//        values.put(MediaStore.Video.Media.DATA, filePath);
//        videoContentValues = values;
//
//        return filePath;
//    }
//
//    private static String genrateFilePath(Context context,String uniqueId, boolean isFinalPath, File tempFolderPath)
//    {
//        String fileName = CONSTANTS.FILE_START_NAME + uniqueId + CONSTANTS.VIDEO_EXTENSION;
//        String dirPath = Environment.getExternalStorageDirectory()+"/Android/data/" + context.getPackageName()+"/video";
//        if(isFinalPath)
//        {
//            File file = new File(dirPath);
//            if(!file.exists() || !file.isDirectory())
//                file.mkdirs();
//        }
//        else
//            dirPath = tempFolderPath.getAbsolutePath();
//        String filePath = dirPath + "/" + fileName;
//        return filePath;
//    }
//    public static String createTempPath(Context context,File tempFolderPath )
//    {
//        long dateTaken = System.currentTimeMillis();
//        String filePath = genrateFilePath(context,String.valueOf(dateTaken), false, tempFolderPath);
//        return filePath;
//    }

    public static List<Camera.Size> getResolutionList(Camera camera)
    {
        Camera.Parameters parameters = camera.getParameters();


        return parameters.getSupportedPreviewSizes();
    }

//    public static RecorderParameters getRecorderParameter(int currentResolution)
//    {
//        RecorderParameters parameters = new RecorderParameters();
//        if(currentResolution ==  CONSTANTS.RESOLUTION_HIGH_VALUE)
//        {
//            parameters.setAudioBitrate(128000);
//            parameters.setVideoQuality(0);
//        }
//        else if(currentResolution ==  CONSTANTS.RESOLUTION_MEDIUM_VALUE)
//        {
//            parameters.setAudioBitrate(128000);
//            parameters.setVideoQuality(5);
//        }
//        else if(currentResolution == CONSTANTS.RESOLUTION_LOW_VALUE)
//        {
//            parameters.setAudioBitrate(96000);
//            parameters.setVideoQuality(20);
//        }
//        return parameters;
//    }

    public static class ResolutionComparator implements Comparator<Camera.Size> {
        @Override
        public int compare(Camera.Size size1, Camera.Size size2) {
            if(size1.height != size2.height)
                return size1.height -size2.height;
            else
                return size1.width - size2.width;
        }
    }

    public static int getTimeStampInNsFromSampleCounted(int paramInt)
    {
        return (int)(paramInt / 0.0441D);
    }
}
