package org.wysaid.cgeDemo;

import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.common.Common;
import org.wysaid.common.SharedContext;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFFmpegNativeLibrary;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.nativePort.CGENativeLibrary;

import java.io.IOException;
import java.io.InputStream;

public class TestCaseActivity extends AppCompatActivity {

    public static final String LOG_TAG = Common.LOG_TAG;

    protected Thread mThread;
    protected boolean mShouldStopThread = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_case);
    }

    protected void threadSync() {

        if (mThread != null && mThread.isAlive()) {
            mShouldStopThread = true;

            try {
                mThread.join();
                mThread = null;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        mShouldStopThread = false;
    }

    protected void showMsg(final String msg) {

        TestCaseActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                MsgUtil.toastMsg(TestCaseActivity.this, msg, Toast.LENGTH_SHORT);
            }
        });
    }

    public void testCaseOffscreenVideoRendering(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {

                Log.i(LOG_TAG, "Test case 1 clicked!\n");

                String outputFilename = FileUtil.getPath() + "/blendVideo.mp4";
                String inputFileName = FileUtil.getTextContent(CameraDemoActivity.lastVideoPathFileName);
                if (inputFileName == null) {
                    showMsg("No video is recorded, please record one in the 2nd case.");
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

                //bmp is used for watermark, (just pass null if you don't want that)
                //and ususally the blend mode is CGE_BLEND_ADDREV for watermarks.
                CGEFFmpegNativeLibrary.generateVideoWithFilter(outputFilename, inputFileName, "@adjust lut late_sunset.png", 1.0f, bmp, CGENativeLibrary.TextureBlendMode.CGE_BLEND_ADDREV, 1.0f, false);

                showMsg("Done! The file is generated at: " + outputFilename);
                Log.i(LOG_TAG, "Done! The file is generated at: " + outputFilename);
                sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + outputFilename)));
            }
        });

        mThread.start();
    }

    public void testCaseCustomFilter(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {

                Bitmap src = BitmapFactory.decodeResource(TestCaseActivity.this.getResources(), R.drawable.bgview);
                int maxIndex = CGENativeLibrary.cgeGetCustomFilterNum();
                SharedContext glContext = SharedContext.create();
                glContext.makeCurrent();
                for (int i = 0; i != maxIndex && !mShouldStopThread; ++i) {
                    //If a gl context is already binded, you should pass true for the last arg, or false otherwise.
                    //It's better to create a gl context manually, so that the cpp layer will not create it each time when the function is called.
                    //You can also use CGEImageHandler to do this. (Maybe faster)
                    Bitmap dst = CGENativeLibrary.cgeFilterImageWithCustomFilter(src, i, 1.0f, true, true);
                    String s = ImageUtil.saveBitmap(dst);
                    dst.recycle();

                    showMsg("The filter is applied! See it: /sdcard/libCGE/rec_*.jpg");
                    sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                }

                glContext.release();
            }
        });

        mThread.start();
    }

    public void testCaseConfigStringFilters(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {
                Bitmap src = BitmapFactory.decodeResource(TestCaseActivity.this.getResources(), R.drawable.bgview);
                int maxIndex = MainActivity.EFFECT_CONFIGS.length;

                SharedContext glContext = SharedContext.create();
                glContext.makeCurrent();

                //You can also use "NativeLibrary.filterImage_MultipleEffects" like the function "testCaseCustomFilter".
                //But when you use a CGEImageHandler, you can do the filter faster, because the handler will not be created for every filter.
                CGEImageHandler handler = new CGEImageHandler();
                handler.initWithBitmap(src);

                for (int i = 0; i != maxIndex && !mShouldStopThread; ++i) {

                    final String filterConfig = MainActivity.EFFECT_CONFIGS[i];
                    handler.setFilterWithConfig(filterConfig);
                    handler.processFilters();

                    //To accelerate this, you can add a Bitmap arg for "getResultBitmap",
                    // and reuse the Bitmap instead of recycle it every time.
                    Bitmap dst = handler.getResultBitmap();
                    if(dst == null) {
                        Log.e(LOG_TAG, "getResultBitmap returns null!");
                        continue;
                    }

                    String s = ImageUtil.saveBitmap(dst);
                    dst.recycle();  //Maybe reuse it will be better.

                    showMsg("The config " + filterConfig + "is applied! See it: /sdcard/libCGE/rec_*.jpg");
                    sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                }

                glContext.release();
            }
        });

        mThread.start();
    }

    @Override
    protected void onDestroy() {
        threadSync();
        super.onDestroy();
    }
}
