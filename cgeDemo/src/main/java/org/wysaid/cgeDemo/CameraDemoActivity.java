package org.wysaid.cgeDemo;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PointF;
import android.hardware.Camera;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.SeekBar;

import com.sensetime.stmobileapi.STMobileFaceAction;
import com.sensetime.stmobileapi.STMobileMultiTrack106;
import com.sensetime.stmobileapi.STUtils;

import org.wysaid.camera.CameraInstance;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.stmobile.Accelerometer;
import org.wysaid.view.CameraRecordGLSurfaceView;

public class CameraDemoActivity extends AppCompatActivity {

    public static String lastVideoPathFileName = FileUtil.getPath() + "/lastVideoPath.txt";

    private String mCurrentConfig;

    private CameraRecordGLSurfaceView mCameraView;

    public final static String LOG_TAG = CameraRecordGLSurfaceView.LOG_TAG;

    public static CameraDemoActivity mCurrentInstance = null;

    public static CameraDemoActivity getInstance() {
        return mCurrentInstance;
    }

    private void showText(final String s) {
        mCameraView.post(new Runnable() {
            @Override
            public void run() {
                MsgUtil.toastMsg(CameraDemoActivity.this, s);
            }
        });
    }

    public static class MyButtons extends Button {

        public String filterConfig;

        public MyButtons(Context context, String config) {
            super(context);
            filterConfig = config;
        }
    }

    class RecordListener implements View.OnClickListener {

        boolean isValid = true;
        String recordFilename;

        @Override
        public void onClick(View v) {
            Button btn = (Button) v;

            if (!isValid) {
                Log.e(LOG_TAG, "Please wait for the call...");
                return;
            }

            isValid = false;

            if (!mCameraView.isRecording()) {
                btn.setText("Recording");
                Log.i(LOG_TAG, "Start recording...");
                mCameraView.setClearColor(1.0f, 0.0f, 0.0f, 0.3f);
                recordFilename = ImageUtil.getPath() + "/rec_" + System.currentTimeMillis() + ".mp4";
//                recordFilename = ImageUtil.getPath(CameraDemoActivity.this, false) + "/rec_1.mp4";
                mCameraView.startRecording(recordFilename, new CameraRecordGLSurfaceView.StartRecordingCallback() {
                    @Override
                    public void startRecordingOver(boolean success) {
                        if (success) {
                            showText("Start recording OK");
                            FileUtil.saveTextContent(recordFilename, lastVideoPathFileName);
                        } else {
                            showText("Start recording failed");
                        }

                        isValid = true;
                    }
                });
            } else {
                showText("Recorded as: " + recordFilename);
                btn.setText("Recorded");
                Log.i(LOG_TAG, "End recording...");
                mCameraView.setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                mCameraView.endRecording(new CameraRecordGLSurfaceView.EndRecordingCallback() {
                    @Override
                    public void endRecordingOK() {
                        Log.i(LOG_TAG, "End recording OK");
                        isValid = true;
                    }
                });
            }

        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_demo);

//        lastVideoPathFileName = FileUtil.getPathInPackage(CameraDemoActivity.this, true) + "/lastVideoPath.txt";
        Button takePicBtn = (Button) findViewById(R.id.takePicBtn);
        Button takeShotBtn = (Button) findViewById(R.id.takeShotBtn);
        Button recordBtn = (Button) findViewById(R.id.recordBtn);
        mCameraView = (CameraRecordGLSurfaceView) findViewById(R.id.myGLSurfaceView);
        mCameraView.presetCameraForward(false);
        SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar);

        takePicBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showText("Taking Picture...");

                mCameraView.takePicture(new CameraRecordGLSurfaceView.TakePictureCallback() {
                    @Override
                    public void takePictureOK(Bitmap bmp) {
                        if (bmp != null) {
                            String s = ImageUtil.saveBitmap(bmp);
                            bmp.recycle();
                            showText("Take picture success!");
                            sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                        } else
                            showText("Take picture failed!");
                    }
                }, null, mCurrentConfig, 1.0f, true);
            }
        });

        takeShotBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                showText("Taking Shot...");

                mCameraView.takeShot(new CameraRecordGLSurfaceView.TakePictureCallback() {
                    @Override
                    public void takePictureOK(Bitmap bmp) {
                        if (bmp != null) {
                            String s = ImageUtil.saveBitmap(bmp);
                            bmp.recycle();
                            showText("Take Shot success!");
                            sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                        } else
                            showText("Take Shot failed!");
                    }
                }, false);
            }
        });

        recordBtn.setOnClickListener(new RecordListener());

        LinearLayout layout = (LinearLayout) findViewById(R.id.menuLinearLayout);

        for (int i = 0; i != MainActivity.EFFECT_CONFIGS.length; ++i) {
            MyButtons button = new MyButtons(this, MainActivity.EFFECT_CONFIGS[i]);
            button.setAllCaps(false);
            if (i == 0)
                button.setText("None");
            else
                button.setText("Filter" + i);
            button.setOnClickListener(mFilterSwitchListener);
            layout.addView(button);
        }

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float intensity = progress / 100.0f;
                mCameraView.setFilterIntensity(intensity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        mCurrentInstance = this;

        Button shapeBtn = (Button) findViewById(R.id.shapeBtn);
        shapeBtn.setOnClickListener(new View.OnClickListener() {
            private boolean mIsUsingShape = false;
            Bitmap mBmp;

            @Override
            public void onClick(View v) {
                mIsUsingShape = !mIsUsingShape;
                if (mIsUsingShape) {
                    if (mBmp == null) {
                        mBmp = BitmapFactory.decodeResource(getResources(), R.drawable.mask1);
                    }
                    if (mBmp != null)
                        mCameraView.setMaskBitmap(mBmp, false, new CameraRecordGLSurfaceView.SetMaskBitmapCallback() {
                            @Override
                            public void setMaskOK(CGEFrameRecorder recorder) {
                                //flip mask
                                if (mCameraView.isUsingMask())
                                    recorder.setMaskFlipScale(1.0f, -1.0f);
                            }
                        });
                } else {
                    mCameraView.setMaskBitmap(null, false);
                }

            }
        });

        Button switchBtn = (Button) findViewById(R.id.switchCameraBtn);
        switchBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mCameraView.switchCamera();
            }
        });

        Button flashBtn = (Button) findViewById(R.id.flashBtn);
        flashBtn.setOnClickListener(new View.OnClickListener() {
            int flashIndex = 0;
            String[] flashModes = {
                    Camera.Parameters.FLASH_MODE_AUTO,
                    Camera.Parameters.FLASH_MODE_ON,
                    Camera.Parameters.FLASH_MODE_OFF,
                    Camera.Parameters.FLASH_MODE_TORCH,
                    Camera.Parameters.FLASH_MODE_RED_EYE,
            };

            @Override
            public void onClick(View v) {
                mCameraView.setFlashLightMode(flashModes[flashIndex]);
                ++flashIndex;
                flashIndex %= flashModes.length;
            }
        });

        //Recording video size
        mCameraView.presetRecordingSize(480, 640);
//        mCameraView.presetRecordingSize(720, 1280);

        //Taking picture size.
        mCameraView.setPictureSize(2048, 2048, true); // > 4MP
        mCameraView.setZOrderOnTop(false);
        mCameraView.setZOrderMediaOverlay(true);

        mCameraView.setOnCreateCallback(new CameraRecordGLSurfaceView.OnCreateCallback() {
            @Override
            public void createOver(boolean success) {
                if (success) {
                    Log.i(LOG_TAG, "view create OK");
                } else {
                    Log.e(LOG_TAG, "view create failed!");
                }
            }
        });

        Button pauseBtn = (Button) findViewById(R.id.pauseBtn);
        Button resumeBtn = (Button) findViewById(R.id.resumeBtn);

        pauseBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraView.stopPreview();
            }
        });

        resumeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraView.resumePreview();
            }
        });

        Button fitViewBtn = (Button) findViewById(R.id.fitViewBtn);
        fitViewBtn.setOnClickListener(new View.OnClickListener() {
            boolean shouldFit = false;

            @Override
            public void onClick(View v) {
                shouldFit = !shouldFit;
                mCameraView.setFitFullView(shouldFit);
            }
        });

        mCameraView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, final MotionEvent event) {

                switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN: {
                        Log.i(LOG_TAG, String.format("Tap to focus: %g, %g", event.getX(), event.getY()));
                        final float focusX = event.getX() / mCameraView.getWidth();
                        final float focusY = event.getY() / mCameraView.getHeight();

                        mCameraView.focusAtPoint(focusX, focusY, new Camera.AutoFocusCallback() {
                            @Override
                            public void onAutoFocus(boolean success, Camera camera) {
                                if (success) {
                                    Log.e(LOG_TAG, String.format("Focus OK, pos: %g, %g", focusX, focusY));
                                } else {
                                    Log.e(LOG_TAG, String.format("Focus failed, pos: %g, %g", focusX, focusY));
                                    mCameraView.cameraInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                                }
                            }
                        });
                    }
                    break;
                    default:
                        break;
                }

                return true;
            }
        });

        mCameraView.setPictureSize(600, 800, true);

        Button faceDetectBtn = (Button) findViewById(R.id.face_detect_btn);
        faceDetectBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CameraInstance.getInstance().getCameraDevice().setPreviewCallback(new Camera.PreviewCallback() {
                    @Override
                    public void onPreviewFrame(byte[] data, Camera camera) {
                        if (mNV21Data == null) {
                            mNV21Data = new byte[CameraInstance.getInstance().previewHeight() * CameraInstance.getInstance().previewWidth() * 2];
                        }
                        synchronized (mNV21Data) {
                            System.arraycopy(data, 0, mNV21Data, 0, data.length);
                            mIsNV21ready = true;
                        }
                    }
                });
                mCameraView.setFilterWithConfig("@facedetect");
            }
        });
        mAcc = new Accelerometer(this);
        mAcc.start();
    }

    private byte[] mNV21Data;
    private boolean mIsFaceDetectThreadKilled = false;
    private Thread mFaceDetectThread;
    private boolean mIsNV21ready = false;
    private Accelerometer mAcc;
    private STMobileMultiTrack106 mTracker = null;
    private static final int ST_MOBILE_TRACKING_ENABLE_FACE_ACTION = 0x00000020;
    private byte[] tmp = null;

    private void stopFaceDetectThread() {
        mIsFaceDetectThreadKilled = true;
        if (mFaceDetectThread != null) {
            try {
                mFaceDetectThread.join(1000);
            } catch (InterruptedException e1) {
                e1.printStackTrace();
            }
        }
    }

    private void startFaceDetectThread() {
        mIsFaceDetectThreadKilled = false;

        mFaceDetectThread = new Thread() {
            @Override
            public void run() {
                while (!mIsFaceDetectThreadKilled) {
                    if (!mIsNV21ready) {
                        continue;
                    }
                    if (tmp == null) {
                        tmp = new byte[CameraInstance.getInstance().previewHeight() * CameraInstance.getInstance().previewWidth() * 2];
                    }
                    synchronized (mNV21Data) {
                        System.arraycopy(mNV21Data, 0, tmp, 0, mNV21Data.length);
                        mIsNV21ready = false;
                    }
                    boolean frontCamera = (CameraInstance.getInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_FRONT);
                    int dir = Accelerometer.getDirection();
                    /**
                     * 请注意前置摄像头与后置摄像头旋转定义不同
                     * 请注意不同手机摄像头旋转定义不同
                     */
                    if (frontCamera &&
                            ((CameraInstance.getInstance().getCameraInfo().orientation == 270 && (dir & 1) == 1) ||
                                    (CameraInstance.getInstance().getCameraInfo().orientation == 90 && (dir & 1) == 0))) {
                        dir = (dir ^ 2);
                    }
                    STMobileFaceAction[] faceActions = mTracker.trackFaceAction(tmp, dir, CameraInstance.getInstance().previewWidth(), CameraInstance.getInstance().previewHeight());
                    boolean rotate270 = CameraInstance.getInstance().getCameraInfo().orientation == 270;
                    Log.e("xuezi", "rotate270" + rotate270);
                    if (faceActions != null && faceActions.length > 0) {
                        STMobileFaceAction r = faceActions[0];
                        PointF[] points = r.getFace().getPointsArray();
                        float[] pointArray = new float[212];
                        for (int i = 0; i < points.length; i++) {
                            if (rotate270) {
                                points[i] = STUtils.RotateDeg270(points[i], CameraInstance.getInstance().previewWidth(), CameraInstance.getInstance().previewHeight(), frontCamera);
                            } else {
                                points[i] = STUtils.RotateDeg90(points[i], CameraInstance.getInstance().previewWidth(), CameraInstance.getInstance().previewHeight(), frontCamera);
                            }
                            pointArray[2*i] = points[i].x / CameraInstance.getInstance().previewHeight();
                            pointArray[2*i+1] = points[i].y / CameraInstance.getInstance().previewWidth();
                        }
                        mCameraView.setFaceDetectPoints(pointArray);
                    }
                    else {
                        mCameraView.clearFaceDetectPoints();
                    }
                }
            }
        };
        mFaceDetectThread.start();
    }

    private View.OnClickListener mFilterSwitchListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            CameraInstance.getInstance().getCameraDevice().setPreviewCallback(null);
            MyButtons btn = (MyButtons)v;
            mCameraView.setFilterWithConfig(btn.filterConfig);
            mCurrentConfig = btn.filterConfig;
        }
    };

    int customFilterIndex = 0;

    public void customFilterClicked(View view) {
        ++customFilterIndex;
        customFilterIndex %= CGENativeLibrary.cgeGetCustomFilterNum();
        mCameraView.queueEvent(new Runnable() {
            @Override
            public void run() {
                long customFilter = CGENativeLibrary.cgeCreateCustomNativeFilter(customFilterIndex, 1.0f);
                mCameraView.getRecorder().setNativeFilter(customFilter);
            }
        });
    }

    public void dynamicFilterClicked(View view) {

        mCameraView.setFilterWithConfig("#unpack @dynamic mf 10 0");

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_camera_demo, menu);
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onPause() {
        super.onPause();
        CameraInstance.getInstance().stopCamera();
        Log.i(LOG_TAG, "activity onPause...");
        mCameraView.release(null);
        mCameraView.onPause();
        mAcc.stop();
        stopFaceDetectThread();
        if (mTracker != null) {
            System.out.println("destroy tracker");
            mTracker.destory();
            mTracker = null;
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        mCameraView.onResume();
        mAcc.start();
        if (mTracker == null) {
//            long start_init = System.currentTimeMillis();
//			int config = 0; //default config
            int config = ST_MOBILE_TRACKING_ENABLE_FACE_ACTION;
            mTracker = new STMobileMultiTrack106(this, config);
            int max = 1;
            mTracker.setMaxDetectableFaces(max);
//            long end_init = System.currentTimeMillis();
//            Log.i("track106", "init cost "+(end_init - start_init) +" ms");
        }
        startFaceDetectThread();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
