package org.wysaid.cgeDemo;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.media.FaceDetector;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.SeekBar;

import org.wysaid.common.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFaceTracker;
import org.wysaid.view.ImageGLSurfaceView;

import java.io.InputStream;

public class ImageDemoActivity extends AppCompatActivity {

    private Bitmap _bitmap;
    private ImageGLSurfaceView _imageView;
    private String _currentConfig;

    public static final int REQUEST_CODE_PICK_IMAGE = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_filter_demo);

        LinearLayout menuLayout = (LinearLayout) findViewById(R.id.menuLayout);
        menuLayout.setHorizontalScrollBarEnabled(true);

        for(int i = 0; i != MainActivity.effectConfigs.length; ++i)
        {
            Button btn = new Button(this);
            btn.setText("filter" + i);
            btn.setOnClickListener(buttonClickListener);
            menuLayout.addView(btn);
        }

        Button btn = (Button)findViewById(R.id.galleryBtn);
        btn.setOnClickListener(galleryBtnClickListener);

        _imageView = (ImageGLSurfaceView) findViewById(R.id.mainImageView);
        _bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.bgview);

        _imageView.setSurfaceCreatedCallback(new ImageGLSurfaceView.OnSurfaceCreatedCallback() {
            @Override
            public void surfaceCreated() {
                _imageView.setImageBitmap(_bitmap);
                _imageView.setFilterWithConfig(_currentConfig);
            }
        });

        Button saveBtn = (Button) findViewById(R.id.saveSingleFrameBtn);
        saveBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                _imageView.getResultBitmap(new ImageGLSurfaceView.QueryResultBitmapCallback() {
                    @Override
                    public void get(final Bitmap bmp) {
                        String s = ImageUtil.saveBitmap(bmp);
                        sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                    }
                });
            }
        });

        _imageView.setDisplayMode(ImageGLSurfaceView.DisplayMode.DISPLAY_ASPECT_FIT);

        SeekBar seekBar = (SeekBar)findViewById(R.id.seekBar);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float intensity = progress / 100.0f;
                _imageView.setFilterIntensity(intensity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

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

                        InputStream stream = getContentResolver().openInputStream(data.getData());
                        Bitmap bmp = BitmapFactory.decodeStream(stream);

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
                        MsgUtil.toastMsg(this, "Error: Can not open image");
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
                    _currentConfig = MainActivity.effectConfigs[index];
                    _imageView.setFilterWithConfig(_currentConfig);
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
        _imageView.release();
        _imageView.onPause();
    }

    @Override
    public void onResume() {
        Log.i(Common.LOG_TAG, "Filter Demo onResume...");
        super.onResume();
        _imageView.onResume();
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

    public void faceDetectTestCase(View view) {

        Log.i(Common.LOG_TAG, "Face detecting");

        _imageView.getResultBitmap(new ImageGLSurfaceView.QueryResultBitmapCallback() {
            @Override
            public void get(final Bitmap bmp) {

                _imageView.post(new Runnable() {
                    @Override
                    public void run() {
                        ImageUtil.FaceRects rects = ImageUtil.findFaceByBitmap(bmp, 8);

                        if (rects == null) {
                            MsgUtil.toastMsg(ImageDemoActivity.this, "unknown error");
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

                                content += String.format("Accuracy: %g, center %g, %g, eyeDis: %g\n", face.confidence(), pnt.x, pnt.y, eyeDis);
                                canvas.drawRect((int) (pnt.x - eyeDis * 1.5f), (int) (pnt.y - eyeDis * 1.5f), (int) (pnt.x + eyeDis * 1.5f), (int) (pnt.y + eyeDis * 1.5f), paint);

                                //眼睛中心
                                canvas.drawRect((int) (pnt.x - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x + 2.0f), (int) (pnt.y + 2.0f), paint);

                                //双眼
                                canvas.drawRect((int) (pnt.x - halfEyeDis - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x - halfEyeDis + 2.0f), (int) (pnt.y + 2.0f), paint);
                                canvas.drawRect((int) (pnt.x + halfEyeDis - 2.0f), (int) (pnt.y - 2.0f), (int) (pnt.x + halfEyeDis + 2.0f), (int) (pnt.y + 2.0f), paint);
                            }

                            MsgUtil.toastMsg(ImageDemoActivity.this, content);
                            _imageView.setImageBitmap(bmp);
                        } else {
                            Log.i(Common.LOG_TAG, "No face");
                            MsgUtil.toastMsg(ImageDemoActivity.this, "No face");
                        }
                    }
                });
            }
        });
    }

    static int displayIndex;

    public void switchDisplayMode(View view) {
        ImageGLSurfaceView.DisplayMode[] modes = {
                ImageGLSurfaceView.DisplayMode.DISPLAY_SCALE_TO_FILL,
                ImageGLSurfaceView.DisplayMode.DISPLAY_ASPECT_FILL,
                ImageGLSurfaceView.DisplayMode.DISPLAY_ASPECT_FIT,
        };

        _imageView.setDisplayMode(modes[++displayIndex % modes.length]);
    }

    public void faceTrackerTestCase(View view) {

        CGEFaceTracker tracker = CGEFaceTracker.createFaceTracker();

        if(tracker != null) {

            if(!_bitmap.isMutable()) {
                _bitmap = _bitmap.copy(_bitmap.getConfig(), true);
            }

            CGEFaceTracker.FaceResultSimple result = tracker.detectFaceWithSimpleResult(_bitmap, true);

            Log.i(Common.LOG_TAG, String.format("LeftEye: %g, %g, rightEye: %g, %g, nose: %g, %g, mouth: %g, %g, jaw: %g, %g", result.leftEyePos.x, result.leftEyePos.y, result.rightEyepos.x, result.rightEyepos.y, result.nosePos.x, result.nosePos.y, result.mouthPos.x, result.mouthPos.y, result.jawPos.x, result.jawPos.y));

            Canvas cvs = new Canvas(_bitmap);
            Paint paint = new Paint();
            paint.setStrokeWidth(3.0f);
            paint.setColor(0xffffffff);
            cvs.drawCircle(result.leftEyePos.x, result.leftEyePos.y, 5, paint);
            cvs.drawCircle(result.rightEyepos.x, result.rightEyepos.y, 5, paint);
            cvs.drawCircle(result.nosePos.x, result.nosePos.y, 5, paint);
            cvs.drawCircle(result.mouthPos.x, result.mouthPos.y, 5, paint);
            cvs.drawCircle(result.jawPos.x, result.jawPos.y, 5, paint);


            _imageView.setImageBitmap(_bitmap);
        }

        tracker.release();
    }
}
