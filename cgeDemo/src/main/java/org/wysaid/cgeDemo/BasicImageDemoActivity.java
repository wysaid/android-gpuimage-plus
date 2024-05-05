package org.wysaid.cgeDemo;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;


import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.SeekBar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatButton;

import org.wysaid.common.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.view.ImageGLSurfaceView;

import java.io.InputStream;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Data: 2017/7/7
 * Description:
 */

// For general photo editing scene.

public class BasicImageDemoActivity extends AppCompatActivity {

    protected static final String BASIC_FILTER_CONFIG = "@adjust brightness 0 @adjust contrast 1 @adjust saturation 1 @adjust sharpen 0";
    protected static final String BASIC_FILTER_NAMES[] = {
            "brightness",
            "contrast",
            "saturation",
            "sharpen"
    };

    private Bitmap mBitmap;
    private ImageGLSurfaceView mImageView;
    private SeekBar mSeekBar;

    class AdjustConfig {
        public int index;
        public float intensity, slierIntensity = 0.5f;
        public float minValue, originValue, maxValue;

        public AdjustConfig(int _index, float _minValue, float _originValue, float _maxValue) {
            index = _index;
            minValue = _minValue;
            originValue = _originValue;
            maxValue = _maxValue;
            intensity = _originValue;
        }

        protected float calcIntensity(float _intensity) {
            float result;
            if (_intensity <= 0.0f) {
                result = minValue;
            } else if (_intensity >= 1.0f) {
                result = maxValue;
            } else if (_intensity <= 0.5f) {
                result = minValue + (originValue - minValue) * _intensity * 2.0f;
            } else {
                result = maxValue + (originValue - maxValue) * (1.0f - _intensity) * 2.0f;
            }
            return result;
        }

        //_intensity range: [0.0, 1.0], 0.5 for the origin.
        public void setIntensity(float _intensity, boolean shouldProcess) {
            if (mImageView != null) {
                slierIntensity = _intensity;
                intensity = calcIntensity(_intensity);
                mImageView.setFilterIntensityForIndex(intensity, index, shouldProcess);
            }
        }
    }

    AdjustConfig mActiveConfig = null;

    public void setActiveConfig(AdjustConfig config) {
        mActiveConfig = config;
        mSeekBar.setProgress((int) (config.slierIntensity * mSeekBar.getMax()));
    }

    AdjustConfig mAdjustConfigs[] = {
            new AdjustConfig(0, -1.0f, 0.0f, 1.0f), //brightness
            new AdjustConfig(1, 0.1f, 1.0f, 3.0f), //contrast
            new AdjustConfig(2, 0.0f, 1.0f, 3.0f), //saturation
            new AdjustConfig(3, -1.0f, 0.0f, 10.0f) //sharpen
    };

    public static final int REQUEST_CODE_PICK_IMAGE = 1;

    public static class MyButton extends AppCompatButton implements View.OnClickListener {

        AdjustConfig mConfig;
        BasicImageDemoActivity mImageDemoActivity;

        public MyButton(BasicImageDemoActivity context, AdjustConfig config) {
            super(context);
            mImageDemoActivity = context;
            mConfig = config;
            setOnClickListener(this);
            setAllCaps(false);
        }

        @Override
        public void onClick(View v) {
            mImageDemoActivity.setActiveConfig(mConfig);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_basic_filter_demo);

        mImageView = (ImageGLSurfaceView) findViewById(R.id.mainImageView);
        mBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.bgview);

        mImageView.setSurfaceCreatedCallback(new ImageGLSurfaceView.OnSurfaceCreatedCallback() {
            @Override
            public void surfaceCreated() {
                mImageView.setImageBitmap(mBitmap);
                mImageView.setFilterWithConfig(BASIC_FILTER_CONFIG);
            }
        });

        mImageView.setDisplayMode(ImageGLSurfaceView.DisplayMode.DISPLAY_ASPECT_FIT);

        mSeekBar = (SeekBar) findViewById(R.id.globalRestoreSeekBar);
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mActiveConfig != null) {
                    float intensity = progress / (float) seekBar.getMax();
                    mActiveConfig.setIntensity(intensity, true);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        LinearLayout menuLayout = (LinearLayout) findViewById(R.id.menuLayout);

        if (BASIC_FILTER_NAMES.length == mAdjustConfigs.length) {

            for (int i = 0; i != BASIC_FILTER_NAMES.length; ++i) {
                MyButton btn = new MyButton(this, mAdjustConfigs[i]);
                btn.setText(BASIC_FILTER_NAMES[i]);
                menuLayout.addView(btn);
            }
        } else {
            Log.e(Common.LOG_TAG, "Invalid config num!");
        }
    }

    public void galleryBtnClicked(View view) {
        Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
        photoPickerIntent.setType("image/*");
        startActivityForResult(photoPickerIntent, REQUEST_CODE_PICK_IMAGE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REQUEST_CODE_PICK_IMAGE:
                if (resultCode == RESULT_OK) {
                    try {

                        InputStream stream = getContentResolver().openInputStream(data.getData());
                        Bitmap bmp = BitmapFactory.decodeStream(stream);

                        int w = bmp.getWidth();
                        int h = bmp.getHeight();
                        float s = Math.max(w / 2048.0f, h / 2048.0f);

                        if (s > 1.0f) {
                            w /= s;
                            h /= s;
                            mBitmap = Bitmap.createScaledBitmap(bmp, w, h, false);
                        } else {
                            mBitmap = bmp;
                        }

                        mImageView.setImageBitmap(mBitmap);

                    } catch (Exception e) {
                        e.printStackTrace();
                        MsgUtil.toastMsg(this, "Error: Can not open image");
                    }
                }
            default:
                break;
        }
    }

    public void saveImageBtnClicked(View view) {
        mImageView.getResultBitmap(new ImageGLSurfaceView.QueryResultBitmapCallback() {
            @Override
            public void get(Bitmap bmp) {
                if(bmp == null) {
                    MsgUtil.toastMsg(BasicImageDemoActivity.this, "Get bitmap failed!");
                    return;
                }
                final String s = ImageUtil.saveBitmap(bmp);
                sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                mImageView.post(new Runnable() {
                    @Override
                    public void run() {
                        MsgUtil.toastMsg(BasicImageDemoActivity.this, "Image saved: " + s);
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

        mImageView.setDisplayMode(modes[++displayIndex % modes.length]);
    }

    public void resetBtnClicked(View view) {

        mImageView.queueEvent(new Runnable() {
            @Override
            public void run() {

                CGEImageHandler handler = mImageView.getImageHandler();
                for (AdjustConfig config : mAdjustConfigs) {

                    handler.setFilterIntensityAtIndex(config.originValue, config.index, false);
                }

                handler.revertImage();
                handler.processFilters();
                mImageView.requestRender();
            }
        });
    }

    @Override
    public void onDestroy() {
        Log.i(Common.LOG_TAG, "Filter Demo2 onDestroy...");
        super.onDestroy();
    }

    @Override
    public void onPause() {
        Log.i(Common.LOG_TAG, "Filter Demo2 onPause...");
        super.onPause();
        mImageView.release();
        mImageView.onPause();
    }

    @Override
    public void onResume() {
        Log.i(Common.LOG_TAG, "Filter Demo2 onResume...");
        super.onResume();
        mImageView.onResume();
    }
}
