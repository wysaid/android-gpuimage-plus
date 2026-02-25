package org.wysaid.cgeDemo;

import android.os.Bundle;

import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.camera.CameraProviderFactory;
import org.wysaid.camera.ICameraProvider;
import org.wysaid.cgeDemo.demoUtils.FaceTrackingDemo;
import org.wysaid.view.CameraGLSurfaceView;
import org.wysaid.view.TrackingCameraGLSurfaceView;

public class FaceTrackingDemoActivity extends AppCompatActivity {

    private TrackingCameraGLSurfaceView mCameraView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_face_tracking_demo);

        mCameraView = (TrackingCameraGLSurfaceView) findViewById(R.id.myGLSurfaceView);

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

        mCameraView.presetCameraForward(false);
        mCameraView.presetRecordingSize(480, 640);
        mCameraView.setPictureSize(2048, 2048, true);
        mCameraView.setZOrderMediaOverlay(true);

        mCameraView.setOnCreateCallback(new CameraGLSurfaceView.OnCreateCallback() {
            @Override
            public void createOver() {
                FaceTrackingDemo demo = new FaceTrackingDemo();
                mCameraView.setTrackingProc(demo);
            }
        });
    }

    public void switchCamera(View view) {
        mCameraView.switchCamera();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCameraView.getCameraProvider().closeCamera();
        mCameraView.release(null);
        mCameraView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mCameraView.onResume();
    }


}
