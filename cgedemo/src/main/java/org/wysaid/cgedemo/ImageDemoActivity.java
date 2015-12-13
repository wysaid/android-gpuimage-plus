package org.wysaid.cgeDemo;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.drawable.BitmapDrawable;
import android.media.FaceDetector;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import org.wysaid.myUtils.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.nativePort.CGENativeLibrary;

public class ImageDemoActivity extends ActionBarActivity {

    private Bitmap _bitmap;
    private ImageView _imageView;

    public static final int REQUEST_CODE_PICK_IMAGE = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_filter_demo);

        LinearLayout menuLayout = (LinearLayout) findViewById(R.id.menuLayout);
        menuLayout.setHorizontalScrollBarEnabled(true);

        int layoutWidth = 0;
        for(int i = 0; i != MainActivity.effectConfigs.length; ++i)
        {
            Button btn = new Button(this);
            layoutWidth += btn.getWidth();
            btn.setText("filter" + i);
            btn.setOnClickListener(buttonClickListener);
            menuLayout.addView(btn);
        }

        Button btn = (Button)findViewById(R.id.galleryBtn);
        btn.setOnClickListener(galleryBtnClickListener);

//        CGENativeLibrary.globalInit();

        _imageView = (ImageView) findViewById(R.id.mainImageView);
        BitmapDrawable a = (BitmapDrawable)_imageView.getDrawable();
        _bitmap = a.getBitmap();

        Button saveBtn = (Button) findViewById(R.id.saveBtn);
        saveBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BitmapDrawable a = (BitmapDrawable)_imageView.getDrawable();
                Bitmap bmp = a.getBitmap();
                ImageUtil.saveBitmap(bmp);
            }
        });

        Button faceDetectionBtn = (Button)findViewById(R.id.faceDetectionBtn);
        faceDetectionBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(Common.LOG_TAG, "人脸检测中");
                BitmapDrawable a = (BitmapDrawable)_imageView.getDrawable();
                Bitmap bmp = a.getBitmap().copy(Bitmap.Config.RGB_565, true);

                ImageUtil.FaceRects rects = ImageUtil.findFaceByBitmap(bmp);

                if (rects == null) {
                    Toast.makeText(ImageDemoActivity.this, "未知错误", Toast.LENGTH_LONG);
                    return;
                }

                if (rects.numOfFaces > 0) {
                    String content = "";

                    Canvas canvas = new Canvas(bmp);
                    Paint paint = new Paint();
                    paint.setColor(Color.RED);
                    paint.setStyle(Paint.Style.STROKE);
                    paint.setStrokeWidth(4);

                    for (int i = 0; i != rects.numOfFaces; ++i) {
                        FaceDetector.Face face = rects.faces[i];
                        PointF pnt = new PointF();
                        face.getMidPoint(pnt);

                        float eyeDis = face.eyesDistance();
                        float halfEyeDis = eyeDis / 2.0f;

                        content += String.format("准确率: %g, 人脸中心 %g, %g, 眼间距: %g\n", face.confidence(), pnt.x, pnt.y, eyeDis);
                        canvas.drawRect((int) (pnt.x - eyeDis * 1.5f), (int) (pnt.y - eyeDis * 1.5f), (int) (pnt.x + eyeDis * 1.5f), (int) (pnt.y + eyeDis * 1.5f), paint);

                        //眼睛中心
                        canvas.drawRect((int) (pnt.x - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x + 2.0f), (int) (pnt.y + 2.0f), paint);

                        //双眼
                        canvas.drawRect((int) (pnt.x - halfEyeDis - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x - halfEyeDis + 2.0f), (int) (pnt.y + 2.0f), paint);
                        canvas.drawRect((int) (pnt.x + halfEyeDis - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x + halfEyeDis + 2.0f), (int) (pnt.y + 2.0f), paint);
                    }

                    Toast.makeText(ImageDemoActivity.this, content, Toast.LENGTH_SHORT).show();
                    _imageView.setImageBitmap(bmp);
                } else {
                    Log.i(Common.LOG_TAG, "未发现人脸");
                    Toast.makeText(ImageDemoActivity.this, "未发现人脸", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }

    android.view.View.OnClickListener galleryBtnClickListener = new android.view.View.OnClickListener(){

        @Override
        public  void onClick(final android.view.View view) {
            Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
            photoPickerIntent.setType("image/*");
            startActivityForResult(photoPickerIntent, REQUEST_CODE_PICK_IMAGE);
        }
    };

    public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        switch (requestCode)
        {
            case REQUEST_CODE_PICK_IMAGE:
                if(resultCode == RESULT_OK)
                {
                    try {
                        _imageView.setImageURI(data.getData());
                        Bitmap bmp = ((BitmapDrawable)_imageView.getDrawable()).getBitmap();

                        int w = bmp.getWidth();
                        int h = bmp.getHeight();
                        float s = Math.max(w / 2048.0f, h / 2048.0f);

                        if(s > 1.0f) {
                            w /= s;
                            h /= s;
                            _bitmap = Bitmap.createScaledBitmap(bmp, w, h, false);
                        } else {
                            _bitmap = bmp;
                        }

                        _imageView.setImageBitmap(_bitmap);

                    } catch (Exception e) {
                        e.printStackTrace();
                        Toast.makeText(this, "Error: 未能打开图片", Toast.LENGTH_LONG).show();
                    }

                }
            default:break;
        }
    }

    android.view.View.OnClickListener buttonClickListener = new android.view.View.OnClickListener() {
        @Override
        public void onClick(final android.view.View view) {

            _imageView.post(new Runnable() {
                @Override
                public void run() {
                    Button btn = (Button)view;
                    String str = btn.getText().toString();
                    int index = Integer.parseInt(str.substring(6));
                    String config = MainActivity.effectConfigs[index];
                    Bitmap bmp = CGENativeLibrary.filterImage_MultipleEffects(_bitmap, config, 1.0f);

                    _imageView.setImageBitmap(bmp);
//                    bmp.recycle();
                }
            });
        }
    };

    @Override
    public void onDestroy() {
        Log.i(Common.LOG_TAG, "Filter Demo onDestroy...");
        super.onDestroy();
    }

    @Override
    public void onPause() {
        Log.i(Common.LOG_TAG, "Filter Demo onPause...");
        super.onPause();
    }

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
