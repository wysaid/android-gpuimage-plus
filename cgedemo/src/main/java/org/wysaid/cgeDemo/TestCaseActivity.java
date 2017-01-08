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
import org.wysaid.common.SharedContext;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
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

    public void testCaseOffscreenVideoRendering(View view) {

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
        CGEFFmpegNativeLibrary.generateVideoWithFilter(outputFilename, inputFileName, "@adjust lut late_sunset.png", 1.0f, bmp, CGENativeLibrary.TextureBlendMode.CGE_BLEND_ADDREV, 1.0f, false);

        MsgUtil.toastMsg(this, "Done! The file is generated at: " + outputFilename);
        Log.i(LOG_TAG, "Done! The file is generated at: " + outputFilename);
    }

    public void testCaseCustomFilter(View view) {
        Bitmap src = BitmapFactory.decodeResource(this.getResources(), R.drawable.bgview);
        int maxIndex = CGENativeLibrary.cgeGetCustomFilterNum();
        SharedContext glContext = SharedContext.create();
        glContext.makeCurrent();
        for(int i = 0; i != maxIndex; ++i)
        {
            //If a gl context is already binded, then pass true for the last arg, or false otherwise.
            //It's better to create a gl context manually, so that the cpp layer will not create it each time.
            Bitmap dst = CGENativeLibrary.cgeFilterImageWithCustomFilter(src, i, 1.0f, true);
            ImageUtil.saveBitmap(dst);
            dst.recycle();
        }
    }
}
