package org.wysaid.cgeDemo;

import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.camera.CameraInstance;
import org.wysaid.cgeDemo.demoUtils.MultiInputDemo;
import org.wysaid.common.Common;

public class MultiInputActivity extends AppCompatActivity {

    private MultiInputDemo mCameraView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_multi_input);

        mCameraView = (MultiInputDemo) findViewById(R.id.myGLSurfaceView);
        mCameraView.presetCameraForward(false);

        //Recording video size
        mCameraView.presetRecordingSize(720, 1280);

        mCameraView.setZOrderOnTop(false);
        mCameraView.setZOrderMediaOverlay(true);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onPause() {
        super.onPause();
        CameraInstance.getInstance().stopCamera();
        Log.i(Common.LOG_TAG, "activity onPause...");
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

    public void demoClicked(View view) {
        mCameraView.triggerEffect();
    }
}
