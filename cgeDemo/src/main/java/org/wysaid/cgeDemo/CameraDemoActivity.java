package org.wysaid.cgeDemo;

import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.provider.MediaStore;

import android.util.Log;

import java.io.File;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.SeekBar;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.camera.CameraInstance;
import org.wysaid.camera.CameraProviderFactory;
import org.wysaid.camera.ICameraProvider;
import org.wysaid.library.BuildConfig;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.myUtils.PermissionUtil;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.view.CameraRecordGLSurfaceView;

public class CameraDemoActivity extends AppCompatActivity {

    // Lazy-initialized in onCreate() so that FileUtil has a valid Context.
    public static String lastVideoPathFileName;

    /** MediaStore URI for the current in-progress recording (API 29+ only). */
    private Uri mRecordingMediaUri = null;

    /**
     * Keeps the MediaStore file descriptor open during recording (API 29+ only).
     * Must stay alive until the recording is finished, otherwise the OS may revoke write access.
     */
    private ParcelFileDescriptor mRecordingPfd = null;

    /**
     * The actual public file path of the current recording (e.g. /sdcard/Movies/libCGE/rec_xxx.mp4).
     * On API 29+, this differs from the fd path passed to native code.
     * Saved to lastVideoPath.txt so TestCaseActivity can find the recorded file.
     */
    private String mRecordingPublicPath = null;

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

    public static class MyButtons extends androidx.appcompat.widget.AppCompatButton {

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

            if (!BuildConfig.CGE_USE_VIDEO_MODULE) {
                MsgUtil.toastMsg(CameraDemoActivity.this, "gradle.ext.disableVideoModule is defined to true, video module disabled!!");
                return;
            }

            Button btn = (Button) v;

            if (!isValid) {
                Log.e(LOG_TAG, "Please wait for the call...");
                return;
            }

            isValid = false;

            if (!mCameraView.isRecording()) {
                btn.setText("Recording");
                Log.i(LOG_TAG, "Start recording...");
                recordFilename = createVideoOutputPath();
                mCameraView.startRecording(recordFilename, new CameraRecordGLSurfaceView.StartRecordingCallback() {
                    @Override
                    public void startRecordingOver(boolean success) {
                        if (success) {
                            showText("Start recording OK");
                            // Save the real public path (not the /proc/self/fd/N path) so
                            // TestCaseActivity can locate the file after the fd is closed.
                            String pathToSave = (mRecordingPublicPath != null)
                                    ? mRecordingPublicPath : recordFilename;
                            FileUtil.saveTextContent(pathToSave, lastVideoPathFileName);
                        } else {
                            showText("Start recording failed");
                        }

                        isValid = true;
                    }
                });
            } else {
                String displayPath = (mRecordingPublicPath != null) ? mRecordingPublicPath : recordFilename;
                showText("Recorded as: " + displayPath);
                btn.setText("Recorded");
                Log.i(LOG_TAG, "End recording...");
                mCameraView.endRecording(new CameraRecordGLSurfaceView.EndRecordingCallback() {
                    @Override
                    public void endRecordingOK() {
                        Log.i(LOG_TAG, "End recording OK");
                        isValid = true;
                        publishRecordedVideo(recordFilename);
                    }
                });
            }

        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_demo);
        PermissionUtil.verifyStoragePermissions(this);

        FileUtil.init(this);
        if (lastVideoPathFileName == null) {
            lastVideoPathFileName = FileUtil.getPath(this) + "/lastVideoPath.txt";
        }
        Button takePicBtn = (Button) findViewById(R.id.takePicBtn);
        Button takeShotBtn = (Button) findViewById(R.id.takeShotBtn);
        Button recordBtn = (Button) findViewById(R.id.recordBtn);
        mCameraView = (CameraRecordGLSurfaceView) findViewById(R.id.myGLSurfaceView);

        // --- Set up camera provider based on user selection ---
        String cameraApi = getIntent().getStringExtra(MainActivity.EXTRA_CAMERA_API);
        if (cameraApi == null) {
            cameraApi = MainActivity.CAMERA_API_CAMERA1;
        }
        CameraProviderFactory.CameraAPI api = MainActivity.CAMERA_API_CAMERAX.equals(cameraApi)
                ? CameraProviderFactory.CameraAPI.CAMERAX
                : CameraProviderFactory.CameraAPI.CAMERA1;
        ICameraProvider provider = CameraProviderFactory.create(api, this);
        if (api == CameraProviderFactory.CameraAPI.CAMERAX) {
            provider.attachLifecycleOwner(this);
        }
        mCameraView.setCameraProvider(provider);
        Log.i(LOG_TAG, "Using camera API: " + api.name());

        mCameraView.presetCameraForward(false);
        SeekBar seekBar = (SeekBar) findViewById(R.id.globalRestoreSeekBar);

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
                });
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
            ICameraProvider.FlashMode[] flashModes = {
                    ICameraProvider.FlashMode.AUTO,
                    ICameraProvider.FlashMode.ON,
                    ICameraProvider.FlashMode.OFF,
                    ICameraProvider.FlashMode.TORCH,
                    ICameraProvider.FlashMode.RED_EYE,
            };

            @Override
            public void onClick(View v) {
                mCameraView.setFlashMode(flashModes[flashIndex]);
                ++flashIndex;
                flashIndex %= flashModes.length;
            }
        });

        //Recording video size
        // mCameraView.presetRecordingSize(480, 640);
       mCameraView.presetRecordingSize(1080, 1920);

        //Taking picture size.
        mCameraView.setPictureSize(2048, 2048, true); // > 4MP
        mCameraView.setZOrderOnTop(false);
        mCameraView.setZOrderMediaOverlay(true);

        mCameraView.setOnCreateCallback(new CameraRecordGLSurfaceView.OnCreateCallback() {
            @Override
            public void createOver() {
                Log.i(LOG_TAG, "view onCreate");
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

                        mCameraView.focusAtPoint(focusX, focusY, new ICameraProvider.AutoFocusCallback() {
                            @Override
                            public void onAutoFocus(boolean success) {
                                if (success) {
                                    Log.e(LOG_TAG, String.format("Focus OK, pos: %g, %g", focusX, focusY));
                                } else {
                                    Log.e(LOG_TAG, String.format("Focus failed, pos: %g, %g", focusX, focusY));
                                    mCameraView.getCameraProvider().setFocusMode("continuous-video");
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

    }

    /**
     * Creates an output path for a new video recording and saves it in a gallery-visible location.
     *
     * <p>On API 29+ devices, uses MediaStore IS_PENDING + {@code /proc/self/fd/N} so the file
     * lands in the public {@code Movies/libCGE/} directory and native FFmpeg can write directly.
     * The gallery picks it up immediately once IS_PENDING is cleared in
     * {@link #publishRecordedVideo}. This path is correct for all targetSdk values and requires
     * no changes when targetSdk is upgraded in the future.
     *
     * <p>On API &lt; 29 devices, writes directly to the public Movies directory.
     *
     * @return absolute file path that native code can write to, or {@code null} on failure.
     */
    private String createVideoOutputPath() {
        String displayName = "rec_" + System.currentTimeMillis() + ".mp4";

        if (needsScopedStorageWrite()) {
            // API 29+ device: use MediaStore IS_PENDING so the file ends up in Movies/libCGE/.
            // Open an fd via ContentResolver and pass /proc/self/fd/N to FFmpeg —
            // the standard way to give native code write access on modern Android.
            ContentValues values = new ContentValues();
            values.put(MediaStore.Video.Media.DISPLAY_NAME, displayName);
            values.put(MediaStore.Video.Media.MIME_TYPE, "video/mp4");
            values.put(MediaStore.Video.Media.RELATIVE_PATH,
                    Environment.DIRECTORY_MOVIES + "/libCGE");
            values.put(MediaStore.Video.Media.IS_PENDING, 1);

            Uri collection = MediaStore.Video.Media.getContentUri(
                    MediaStore.VOLUME_EXTERNAL_PRIMARY);
            mRecordingMediaUri = getContentResolver().insert(collection, values);

            if (mRecordingMediaUri == null) {
                Log.e(LOG_TAG, "createVideoOutputPath: MediaStore insert failed");
                mRecordingPublicPath = null;
                return null;
            }

            mRecordingPublicPath = Environment
                    .getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES)
                    .getAbsolutePath() + "/libCGE/" + displayName;

            try {
                mRecordingPfd = getContentResolver()
                        .openFileDescriptor(mRecordingMediaUri, "rw");
                if (mRecordingPfd == null) {
                    Log.e(LOG_TAG, "createVideoOutputPath: openFileDescriptor returned null");
                    getContentResolver().delete(mRecordingMediaUri, null, null);
                    mRecordingMediaUri = null;
                    mRecordingPublicPath = null;
                    return null;
                }
                int fd = mRecordingPfd.getFd();
                String path = "/proc/self/fd/" + fd;
                Log.i(LOG_TAG, "createVideoOutputPath: fd=" + fd
                        + " publicPath=" + mRecordingPublicPath);
                return path;
            } catch (Exception e) {
                Log.e(LOG_TAG, "createVideoOutputPath: openFileDescriptor failed - " + e);
                getContentResolver().delete(mRecordingMediaUri, null, null);
                mRecordingMediaUri = null;
                mRecordingPublicPath = null;
                return null;
            }

        } else {
            // API < 29 device: write directly to the public Movies directory.
            // No MediaStore pending entry or fd trick needed on these older OS versions.
            mRecordingMediaUri = null;
            mRecordingPfd = null;
            File dir = new File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES),
                    "libCGE");
            if (!dir.exists() && !dir.mkdirs()) {
                Log.e(LOG_TAG, "createVideoOutputPath: failed to create dir " + dir);
                mRecordingPublicPath = ImageUtil.getPath(this) + "/" + displayName;
                return mRecordingPublicPath;
            }
            mRecordingPublicPath = new File(dir, displayName).getAbsolutePath();
            return mRecordingPublicPath;
        }
    }

    /**
     * Returns true when the current device requires the MediaStore IS_PENDING + fd approach
     * for writing video files to the public Movies directory.
     *
     * <p>Based purely on the device's OS version (API 29 / Android 10 introduced Scoped Storage).
     * This makes the recording logic forward-compatible: upgrading targetSdkVersion in the future
     * requires no changes here, because modern devices already use the correct code path.
     */
    private boolean needsScopedStorageWrite() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q;
    }

    /**
     * Finalises the recorded video so it appears in the system gallery.
     *
     * <p>On API 29+, clears the {@code IS_PENDING} flag on the MediaStore entry created in
     * {@link #createVideoOutputPath()}. The gallery displays the video immediately.
     *
     * <p>On older versions, uses {@link MediaScannerConnection} to add the file to MediaStore.
     */
    private void publishRecordedVideo(String path) {
        if (needsScopedStorageWrite() && mRecordingMediaUri != null) {
            // Close the file descriptor first so the OS can finalise the file.
            if (mRecordingPfd != null) {
                try { mRecordingPfd.close(); } catch (Exception ignored) {}
                mRecordingPfd = null;
            }
            ContentValues values = new ContentValues();
            values.put(MediaStore.Video.Media.IS_PENDING, 0);
            getContentResolver().update(mRecordingMediaUri, values, null, null);
            Log.i(LOG_TAG, "publishRecordedVideo: cleared IS_PENDING for " + mRecordingMediaUri);
            mRecordingMediaUri = null;
        } else if (path != null) {
            MediaScannerConnection.scanFile(
                    this,
                    new String[]{path},
                    new String[]{"video/mp4"},
                    (p, uri) -> Log.i(LOG_TAG, "publishRecordedVideo: scan done " + p + " -> " + uri));
        }
    }

    private View.OnClickListener mFilterSwitchListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            MyButtons btn = (MyButtons) v;
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
                long customFilter = CGENativeLibrary.cgeCreateCustomNativeFilter(customFilterIndex, 1.0f, true);
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
        mCameraView.getCameraProvider().closeCamera();
        Log.i(LOG_TAG, "activity onPause...");
        mCameraView.release(null);
        mCameraView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();

        mCameraView.onResume();
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
