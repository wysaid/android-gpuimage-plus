package org.wysaid.cgeDemo;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import org.wysaid.common.Common;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFFmpegNativeLibrary;
import org.wysaid.nativePort.CGENativeLibrary;

import java.io.IOException;
import java.io.InputStream;

public class TestCaseActivity extends AppCompatActivity {

    public static final String LOG_TAG = Common.LOG_TAG;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_case);
    }

    public void testCase1(View view) {

        Log.i(LOG_TAG, "Test case 1 clicked!\n");

        String outputFilename = FileUtil.getPath() + "/blendVideo.mp4";
        String inputFileName = FileUtil.getTextContent(CameraDemoActivity.lastVideoPathFileName);
        if(inputFileName == null) {
            Toast.makeText(TestCaseActivity.this, "No video is recorded, please record one in the 2nd case.", Toast.LENGTH_LONG).show();
            return;
        }

        Bitmap bmp;

        try {
            AssetManager am = getAssets();
            InputStream is;

            is = am.open("logo.png");

            bmp = BitmapFactory.decodeStream(is);

        } catch (IOException e) {
            Log.e(LOG_TAG, "Can not open blend image file!");
            bmp = null;
        }

        //bmp is used for watermark,
        //and ususally the blend mode is CGE_BLEND_ADDREV for watermarks.
        CGEFFmpegNativeLibrary.generateVideoWithFilter(outputFilename, inputFileName, "@adjust lut late_sunset.png", 1.0f, bmp, CGENativeLibrary.TextureBlendMode.CGE_BLEND_ADDREV, 1.0f);

        MsgUtil.toastMsg(this, "Done! The file is generated at: " + outputFilename);
        Log.i(LOG_TAG, "Done! The file is generated at: " + outputFilename);
    }
}
