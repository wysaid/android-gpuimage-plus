package org.wysaid.cgedemo;

import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import org.wysaid.myUtils.Common;
import org.wysaid.nativePort.CGENativeLibrary;

import java.io.IOException;
import java.io.InputStream;


public class MainActivity extends ActionBarActivity {

    public static final String LOG_TAG = "wysaid";

    CGENativeLibrary.LoadImageCallback loadImageCallback = new CGENativeLibrary.LoadImageCallback() {

        //注意， 这里回传的name不包含任何路径名， 仅为具体的图片文件名如 1.jpg
        @Override
        public Bitmap loadImage(String name, Object arg) {

            Log.i(Common.LOG_TAG, "正在加载图片: " + name);
            AssetManager am = getAssets();
            InputStream is;
            try {
                is = am.open(name);
            } catch (IOException e) {
                Log.e(Common.LOG_TAG, "Can not open file " + name);
                return null;
            }

            Bitmap bmp = BitmapFactory.decodeStream(is);
            return bmp;
        }

        @Override
        public void loadImageOK(Bitmap bmp, Object arg) {
            Log.i(Common.LOG_TAG, "加载图片完毕， 可以自行选择 recycle or cache");

            //loadImage结束之后可以马上recycle
            //唯一不需要马上recycle的应用场景为 多个不同的滤镜都使用到相同的bitmap
            //那么可以选择缓存起来。
            bmp.recycle();
        }
    };

    public static class DemoClassDescription {
        String activityName;
        String title;
//        DemoClassDescription() {
//            activityName = null;
//            title = null;
//        }

        DemoClassDescription(String _name, String _title) {
            activityName = _name;
            title = _title;
        }
    }

    private static final DemoClassDescription mDemos[] = new DemoClassDescription[]{
            new DemoClassDescription("FilterDemoActivity", "filterDemo"),
            new DemoClassDescription("CameraDemoActivity", "cameraDemo"),
            new DemoClassDescription("VideoPlayerActivity", "playerDemo")
    };

    private LinearLayout mLayout;

    public class MyButton extends Button implements View.OnClickListener {
        private DemoClassDescription mDemo;
        public void setDemo(DemoClassDescription demo) {
            mDemo = demo;
            setText(mDemo.title);
            setOnClickListener(this);
        }
        MyButton(Context context) {
            super(context);
        }

        @Override
        public void onClick(final View v) {
            Log.i(LOG_TAG, String.format("%s is clicked!", mDemo.title));
            Class cls = null;
            try {
                cls = Class.forName("org.wysaid.cgedemo." + mDemo.activityName);
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
                return ;
            }

            if(cls != null)
                startActivity(new Intent(MainActivity.this, cls));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mLayout = (LinearLayout)findViewById(R.id.buttonLayout);

        for(DemoClassDescription demo : mDemos) {
            MyButton btn = new MyButton(this);
            btn.setDemo(demo);
            mLayout.addView(btn);
        }

        //第二个参数根据自身需要设置， 将作为 loadImage 第二个参数回传
        CGENativeLibrary.setLoadImageCallback(loadImageCallback, null);
    }

//    @Override
//    public void onDestroy() {
//        super.onDestroy();
//        System.exit(0);
//    }
//
//    @Override
//    public void onPause() {
//        super.onPause();
//        System.exit(0);
//    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
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
