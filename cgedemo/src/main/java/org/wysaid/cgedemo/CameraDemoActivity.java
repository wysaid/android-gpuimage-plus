package org.wysaid.cgeDemo;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.RectF;
import android.hardware.Camera;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Toast;

import org.wysaid.camera.CameraInstance;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.nativePort.CGEFrameRecorder;
import org.wysaid.view.CameraGLSurfaceView;

public class CameraDemoActivity extends ActionBarActivity {

    String mCurrentConfig;

    private CameraGLSurfaceView mGLSurfaceView;
    private ImageView mThunbnailView;

    public final static String LOG_TAG = CameraGLSurfaceView.LOG_TAG;

    public static CameraDemoActivity mCurrentInstance = null;
    public static CameraDemoActivity getInstance() {
        return mCurrentInstance;
    }

    private void showText(final String s) {
        mGLSurfaceView.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(CameraDemoActivity.this, s, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public class MyButtons extends Button {

        public String filterConfig;

        public MyButtons(Context context, String config) {
            super(context);
            filterConfig = config;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_demo);

        Button takePicBtn = (Button) findViewById(R.id.takePicBtn);
        Button takeShotBtn = (Button) findViewById(R.id.takeShotBtn);
        mGLSurfaceView = (CameraGLSurfaceView)findViewById(R.id.myGLSurfaceView);
        mGLSurfaceView.presetCameraForward(false);
        SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar);
        mThunbnailView = (ImageView)findViewById(R.id.imagePreview);

        takePicBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showText("Taking Picture...");

                mGLSurfaceView.takePicture(new CameraGLSurfaceView.TakePictureCallback() {
                    @Override
                    public void takePictureOK(Bitmap bmp) {
                        if (bmp != null) {
                            ImageUtil.saveBitmap(bmp);
                            bmp.recycle();
                            showText("Take picture success!");
                        } else
                            showText("Take picture failed!");
                    }
                }, null, mCurrentConfig, 1.0f, true);
            }
        });

        takeShotBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                showText("Taking Shot...");

                mGLSurfaceView.takeShot(new CameraGLSurfaceView.TakePictureCallback() {
                    @Override
                    public void takePictureOK(Bitmap bmp) {
                        if (bmp != null) {
                            ImageUtil.saveBitmap(bmp);
                            bmp.recycle();
                            showText("Take Shot success!");
                        } else
                            showText("Take Shot failed!");
                    }
                }, false);
            }
        });

        LinearLayout layout = (LinearLayout) findViewById(R.id.menuLinearLayout);

        for(int i = 0; i != MainActivity.effectConfigs.length; ++i) {
            MyButtons button = new MyButtons(this, MainActivity.effectConfigs[i]);
            button.setText("特效" + i);
            button.setOnClickListener(mFilterSwitchListener);
            layout.addView(button);
        }

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float intensity = progress / (100.0f / 3.0f) - 1.0f;
                mGLSurfaceView.setFilterIntensity(intensity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        mCurrentInstance = this;

        Button shapeBtn = (Button)findViewById(R.id.shapeBtn);
        shapeBtn.setOnClickListener(new View.OnClickListener() {
            private boolean mIsUsingShape = false;
            Bitmap mBmp;

            @Override
            public void onClick(View v) {
                mIsUsingShape = !mIsUsingShape;
                if (mIsUsingShape) {
                    if (mBmp == null) {
                        mBmp = BitmapFactory.decodeResource(getResources(), R.drawable.mask1);
                    }
                    if (mBmp != null)
                        mGLSurfaceView.setMaskBitmap(mBmp, false, new CameraGLSurfaceView.SetMaskBitmapCallback() {
                            @Override
                            public void setMaskOK(CGEFrameRecorder recorder) {
                                //使mask上下翻转
                                if(mGLSurfaceView.isUsingMask())
                                    recorder.setMaskFlipScale(1.0f, -1.0f);
                            }
                        });
                } else {
                    mGLSurfaceView.setMaskBitmap(null, false);
                }

            }
        });

        Button switchBtn = (Button)findViewById(R.id.switchCameraBtn);
        switchBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mGLSurfaceView.switchCamera();
            }
        });

        Button flashBtn = (Button) findViewById(R.id.flashBtn);
        flashBtn.setOnClickListener(new View.OnClickListener() {
            int flashIndex = 0;
            String[] flashModes = {
                    Camera.Parameters.FLASH_MODE_AUTO,
                    Camera.Parameters.FLASH_MODE_ON,
                    Camera.Parameters.FLASH_MODE_OFF,
                    Camera.Parameters.FLASH_MODE_TORCH,
                    Camera.Parameters.FLASH_MODE_RED_EYE,
            };

            @Override
            public void onClick(View v) {
                mGLSurfaceView.setFlashLightMode(flashModes[flashIndex]);
                ++flashIndex;
                flashIndex %= flashModes.length;
            }
        });

        mGLSurfaceView.presetRecordingSize(480, 640);
        mGLSurfaceView.setZOrderOnTop(false);
        mGLSurfaceView.setZOrderMediaOverlay(true);

        mGLSurfaceView.setOnCreateCallback(new CameraGLSurfaceView.OnCreateCallback() {
            @Override
            public void createOver(boolean success) {
                if (success) {
                    Log.i(LOG_TAG, "view 创建成功");
                } else {
                    Log.e(LOG_TAG, "view 创建失败!");
                }
            }
        });

        Button pauseBtn = (Button)findViewById(R.id.pauseBtn);
        Button resumeBtn = (Button)findViewById(R.id.resumeBtn);

        pauseBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGLSurfaceView.stopPreview();
            }
        });

        resumeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGLSurfaceView.resumePreview();
            }
        });

        Button thunbnailBtn = (Button) findViewById(R.id.thunbnailBtn);

        thunbnailBtn.setOnClickListener(new View.OnClickListener() {
            boolean showThunbnailWindow = false;

            @Override
            public void onClick(View v) {
                showThunbnailWindow = !showThunbnailWindow;
                if (showThunbnailWindow) {
                    float showLeft = 0.3f, showTop = 0.35f;
                    float showRight = 0.4f + showLeft, showBottom = 0.3f + showTop;
                    mGLSurfaceView.startThunbnailCliping(150, 150, new RectF(showLeft, showTop, showRight, showBottom), new CameraGLSurfaceView.TakeThunbnailCallback() {

                        public boolean isUsing = false;

                        @Override
                        public boolean isUsingBitmap() {
                            return isUsing;
                        }

                        @Override
                        public void takeThunbnailOK(Bitmap bmp) {
                            isUsing = true;
                            mThunbnailView.setImageBitmap(bmp);
                            mThunbnailView.setVisibility(View.VISIBLE);
                            isUsing = false;
                        }
                    });
                } else {
                    mGLSurfaceView.stopThunbnailCliping();
                    mThunbnailView.setVisibility(View.INVISIBLE);
                }
            }
        });

        mThunbnailView.setVisibility(View.INVISIBLE);

        Button bgBtn = (Button)findViewById(R.id.backgroundBtn);
        bgBtn.setOnClickListener(new View.OnClickListener() {
            boolean useBackground = false;
            Bitmap backgroundBmp;
            @Override
            public void onClick(View v) {
                useBackground = !useBackground;
                if(useBackground) {
                    if(backgroundBmp == null)
                        backgroundBmp = BitmapFactory.decodeResource(getResources(), R.drawable.bgview);
                    mGLSurfaceView.setBackgroundImage(backgroundBmp, false, new CameraGLSurfaceView.SetBackgroundImageCallback() {
                        @Override
                        public void setBackgroundImageOK() {
                            Log.i(LOG_TAG, "Set Background Image OK!");
                        }
                    });
                } else {
                    mGLSurfaceView.setBackgroundImage(null, false, new CameraGLSurfaceView.SetBackgroundImageCallback() {
                        @Override
                        public void setBackgroundImageOK() {
                            Log.i(LOG_TAG, "Cancel Background Image OK!");
                        }
                    });
                }
            }
        });

        Button fitViewBtn = (Button)findViewById(R.id.fitViewBtn);
        fitViewBtn.setOnClickListener(new View.OnClickListener() {
            boolean shouldFit = false;
            @Override
            public void onClick(View v) {
                shouldFit = !shouldFit;
                mGLSurfaceView.setFitFullView(shouldFit);
            }
        });

        mGLSurfaceView.setOnCreateCallback(new CameraGLSurfaceView.OnCreateCallback() {
            @Override
            public void createOver(boolean success) {
                if(success) {
                    Log.i(LOG_TAG, "设备启动成功!");
                } else {
                    Log.e(LOG_TAG, "设备启动失败! 请检查app权限， 允许使用相机");
                }
            }
        });

        mGLSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, final MotionEvent event) {

                switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN: {
                        Log.i(LOG_TAG, String.format("Tap to focus: %g, %g", event.getX(), event.getY()));
                        final float focusX = event.getX() / mGLSurfaceView.getWidth();
                        final float focusY = event.getY() / mGLSurfaceView.getHeight();

                        mGLSurfaceView.focusAtPoint(focusX, focusY, new Camera.AutoFocusCallback() {
                            @Override
                            public void onAutoFocus(boolean success, Camera camera) {
                                if (success) {
                                    Log.e(LOG_TAG, String.format("手动对焦成功， 位置: %g, %g", focusX, focusY));
                                } else {
                                    Log.e(LOG_TAG, String.format("手动对焦失败， 位置: %g, %g", focusX, focusY));
                                    mGLSurfaceView.cameraInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                                }
                            }
                        });
                    }
                    break;
                    default:
                        break;
                }

                return true;
            }
        });

        mGLSurfaceView.setPictureSize(600, 800, true);
    }

    private View.OnClickListener mFilterSwitchListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            MyButtons btn = (MyButtons)v;
            mGLSurfaceView.setFilterWithConfig(btn.filterConfig);
            mCurrentConfig = btn.filterConfig;
        }
    };

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_camera_demo, menu);
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onPause() {
        super.onPause();
        CameraInstance.getInstance().stopCamera();
        Log.i(LOG_TAG, "activity onPause...");
        mGLSurfaceView.release(null);
        mGLSurfaceView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();

        mGLSurfaceView.onResume();
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
