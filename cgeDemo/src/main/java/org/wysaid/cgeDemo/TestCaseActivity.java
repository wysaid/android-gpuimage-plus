package org.wysaid.cgeDemo;

import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
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
