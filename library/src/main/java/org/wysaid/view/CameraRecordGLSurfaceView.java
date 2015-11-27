package org.wysaid.view;

/**
 * Created by wangyang on 15/7/27.
 */


import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.AttributeSet;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

/**
 * Created by wangyang on 15/7/17.
 */
public class CameraRecordGLSurfaceView extends CameraGLSurfaceView {

    public CameraRecordGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    //Not provided by now.
}
