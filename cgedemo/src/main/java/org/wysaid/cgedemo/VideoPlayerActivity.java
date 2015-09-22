package org.wysaid.cgedemo;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaPlayer;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import org.wysaid.camera.CameraInstance;
import org.wysaid.myUtils.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.texUtils.TextureRendererMask;
import org.wysaid.view.VideoPlayerGLSurfaceView;

public class VideoPlayerActivity extends AppCompatActivity {

    VideoPlayerGLSurfaceView mPlayerView;
    Button mShapeBtn;
    Button mTakeshotBtn;
    Button mGalleryBtn;

    public static final int REQUEST_CODE_PICK_VIDEO = 1;

    static class MyButton extends Button implements View.OnClickListener {

        Uri videoUri;
        VideoPlayerGLSurfaceView videoView;
        String buttonText;

        public MyButton(Context context) {
            super(context);
        }

        @Override
        public void onClick(View v) {

            buttonText = getText().toString();
            setText("加载中");

            videoView.setVideoUri(videoUri, new VideoPlayerGLSurfaceView.PlayPreparedCallback() {
                @Override
                public void playPrepared(MediaPlayer player) {
                    Log.i(Common.LOG_TAG, "The video is prepared to play");
                    player.start();
                    setText(buttonText);
                }
            }, new VideoPlayerGLSurfaceView.PlayCompletionCallback() {
                @Override
                public void playComplete(MediaPlayer player) {
                    Log.i(Common.LOG_TAG, "The video playing is over, restart...");
                    player.start();
                }
            });
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_player);
        mPlayerView = (VideoPlayerGLSurfaceView)findViewById(R.id.videoGLSurfaceView);
        mShapeBtn = (Button)findViewById(R.id.switchShapeBtn);

        mShapeBtn.setOnClickListener(new View.OnClickListener() {

            private boolean useMask = false;
            Bitmap bmp;

            @Override
            public void onClick(View v) {
                useMask = !useMask;
                if(useMask) {
                    if(bmp == null)
                        bmp = BitmapFactory.decodeResource(getResources(), R.drawable.mask1);

                    if(bmp != null) {
                        mPlayerView.setMaskBitmap(bmp, false, new VideoPlayerGLSurfaceView.SetMaskBitmapCallback() {
                            @Override
                            public void setMaskOK(TextureRendererMask renderer) {
                                if(mPlayerView.isUsingMask()) {
                                    renderer.setMaskFlipscale(1.0f, -1.0f);
                                }
                            }

                            @Override
                            public void unsetMaskOK(TextureRenderer renderer) {

                            }
                        });
                    }
                } else {
                    mPlayerView.setMaskBitmap(null, false);
                }

            }
        });

        mTakeshotBtn = (Button)findViewById(R.id.takeShotBtn);

        mTakeshotBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mPlayerView.takeShot(new VideoPlayerGLSurfaceView.TakeShotCallback() {
                    @Override
                    public void takeShotOK(Bitmap bmp) {
                        if(bmp != null)
                            ImageUtil.saveBitmap(bmp);
                        else
                            Log.e(Common.LOG_TAG, "take shot failed!");
                    }
                });
            }
        });

        LinearLayout menuLayout = (LinearLayout)findViewById(R.id.menuLinearLayout);

        String[] filePaths = {
                "android.resource://" + getPackageName() + "/" + R.raw.test,
                "http://wge.wysaid.org/res/video/1.mp4",
                "http://wysaid.org/p/test.mp4",
        };

        for(int i = 0; i != filePaths.length; ++i) {
            MyButton btn = new MyButton(this);
            btn.setText("视频" + i);
            btn.videoUri = Uri.parse(filePaths[i]);
            btn.videoView = mPlayerView;
            btn.setOnClickListener(btn);
            menuLayout.addView(btn);

            if(i == 0) {
                btn.onClick(btn);
            }
        }

        mGalleryBtn = (Button)findViewById(R.id.galleryBtn);
        mGalleryBtn.setOnClickListener(galleryBtnClickListener);

        Button fitViewBtn = (Button)findViewById(R.id.fitViewBtn);
        fitViewBtn.setOnClickListener(new View.OnClickListener() {
            boolean shouldFit = false;
            @Override
            public void onClick(View v) {
                shouldFit = !shouldFit;
                mPlayerView.setFitFullView(shouldFit);
            }
        });
    }

    android.view.View.OnClickListener galleryBtnClickListener = new android.view.View.OnClickListener(){

        @Override
        public  void onClick(final android.view.View view) {
            Intent videoPickerIntent = new Intent(Intent.ACTION_GET_CONTENT);
            videoPickerIntent.setType("video/*");
            startActivityForResult(videoPickerIntent, REQUEST_CODE_PICK_VIDEO);
        }
    };

    public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        switch (requestCode)
        {
            case REQUEST_CODE_PICK_VIDEO:
                if(resultCode == RESULT_OK)
                {

                    mPlayerView.setVideoUri(data.getData(), new VideoPlayerGLSurfaceView.PlayPreparedCallback() {
                        @Override
                        public void playPrepared(MediaPlayer player) {
                            Log.i(Common.LOG_TAG, "The video is prepared to play");
                            player.start();
                        }
                    }, new VideoPlayerGLSurfaceView.PlayCompletionCallback() {
                        @Override
                        public void playComplete(MediaPlayer player) {
                            Log.i(Common.LOG_TAG, "The video playing is over, restart...");
                            player.start();
                        }
                    });
                }
            default: break;
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        CameraInstance.getInstance().stopCamera();
        Log.i(VideoPlayerGLSurfaceView.LOG_TAG, "activity onPause...");
        mPlayerView.release();
        mPlayerView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();

        mPlayerView.onResume();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_video_player, menu);
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
