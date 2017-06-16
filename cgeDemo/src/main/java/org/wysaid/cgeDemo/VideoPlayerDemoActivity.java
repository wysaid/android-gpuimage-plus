package org.wysaid.cgeDemo;

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
import android.widget.SeekBar;

import org.wysaid.common.Common;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFrameRenderer;
import org.wysaid.view.VideoPlayerGLSurfaceView;

public class VideoPlayerDemoActivity extends AppCompatActivity {

    VideoPlayerGLSurfaceView mPlayerView;
    Button mShapeBtn;
    Button mTakeshotBtn;
    Button mGalleryBtn;

    String mCurrentConfig;

    public static final int REQUEST_CODE_PICK_VIDEO = 1;

    private VideoPlayerGLSurfaceView.PlayCompletionCallback playCompletionCallback = new VideoPlayerGLSurfaceView.PlayCompletionCallback() {
        @Override
        public void playComplete(MediaPlayer player) {
            Log.i(Common.LOG_TAG, "The video playing is over, restart...");
            player.start();
        }

        @Override
        public boolean playFailed(MediaPlayer player, final int what, final int extra)
        {
            MsgUtil.toastMsg(VideoPlayerDemoActivity.this, String.format("Error occured! Stop playing, Err code: %d, %d", what, extra));
            return true;
        }
    };

    class MyVideoButton extends Button implements View.OnClickListener {

        Uri videoUri;
        VideoPlayerGLSurfaceView videoView;

        public MyVideoButton(Context context) {
            super(context);
        }

        @Override
        public void onClick(View v) {

            MsgUtil.toastMsg(VideoPlayerDemoActivity.this, "loading video: " + videoUri.getHost() + videoUri.getPath() + " If the video is from the internet, you may wait a while...");

            videoView.setVideoUri(videoUri, new VideoPlayerGLSurfaceView.PlayPreparedCallback() {
                @Override
                public void playPrepared(MediaPlayer player) {
                    mPlayerView.post(new Runnable() {
                        @Override
                        public void run() {
                            MsgUtil.toastMsg(VideoPlayerDemoActivity.this, "Start playing " + videoUri.getHost() + videoUri.getPath());
                        }
                    });

                    player.start();
                }
            }, playCompletionCallback);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_player_demo);
        mPlayerView = (VideoPlayerGLSurfaceView)findViewById(R.id.videoGLSurfaceView);
        mPlayerView.setZOrderOnTop(false);
        mPlayerView.setZOrderMediaOverlay(true);

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
                            public void setMaskOK(CGEFrameRenderer renderer) {
//                                if(mPlayerView.isUsingMask()) {
//                                    renderer.setMaskFlipScale(1.0f, -1.0f);
//                                }
                                Log.i(Common.LOG_TAG, "enable mask!");
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
                        if(bmp != null) {
                            String s = ImageUtil.saveBitmap(bmp);
                            VideoPlayerDemoActivity.this.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                        } else {
                            Log.e(Common.LOG_TAG, "take shot failed!");
                        }
                    }
                });
            }
        });

        LinearLayout menuLayout = (LinearLayout)findViewById(R.id.menuLinearLayout);

        {
            Button btn = new Button(this);
            menuLayout.addView(btn);
            btn.setAllCaps(false);
            btn.setText("Last Recorded Video");
            btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                    String lastVideoFileName = FileUtil.getTextContent(CameraDemoActivity.lastVideoPathFileName);
                    if(lastVideoFileName == null) {
                        MsgUtil.toastMsg(VideoPlayerDemoActivity.this, "No video is recorded, please record one in the 2nd case.");
                        return;
                    }

                    Uri lastVideoUri = Uri.parse(lastVideoFileName);
                    mPlayerView.setVideoUri(lastVideoUri, new VideoPlayerGLSurfaceView.PlayPreparedCallback() {
                        @Override
                        public void playPrepared(MediaPlayer player) {
                            Log.i(Common.LOG_TAG, "The video is prepared to play");
                            player.start();
                        }
                    }, playCompletionCallback);
                }
            });
        }

        String[] filePaths = {
                "android.resource://" + getPackageName() + "/" + R.raw.test,
                "http://wge.wysaid.org/res/video/1.mp4",
                "http://wysaid.org/p/test.mp4",
        };

        for(int i = 0; i != filePaths.length; ++i) {
            MyVideoButton btn = new MyVideoButton(this);
            btn.setText("Video" + i);
            btn.videoUri = Uri.parse(filePaths[i]);
            btn.videoView = mPlayerView;
            btn.setOnClickListener(btn);
            menuLayout.addView(btn);

            if(i == 0) {
                btn.onClick(btn);
            }
        }

        for(int i = 0; i != MainActivity.effectConfigs.length; ++i) {
            CameraDemoActivity.MyButtons button = new CameraDemoActivity.MyButtons(this, MainActivity.effectConfigs[i]);
            button.setText("filter" + i);
            button.setOnClickListener(mFilterSwitchListener);
            menuLayout.addView(button);
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

        mPlayerView.setPlayerInitializeCallback(new VideoPlayerGLSurfaceView.PlayerInitializeCallback() {
            @Override
            public void initPlayer(final MediaPlayer player) {
                //针对网络视频进行进度检查
                player.setOnBufferingUpdateListener(new MediaPlayer.OnBufferingUpdateListener() {
                    @Override
                    public void onBufferingUpdate(MediaPlayer mp, int percent) {
                        Log.i(Common.LOG_TAG, "Buffer update: " + percent);
                        if(percent == 100) {
                            Log.i(Common.LOG_TAG, "缓冲完毕!");
                            player.setOnBufferingUpdateListener(null);
                        }
                    }
                });
            }
        });

        SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar);

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float intensity = progress / 100.0f;
                mPlayerView.setFilterIntensity(intensity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }

    private View.OnClickListener mFilterSwitchListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            CameraDemoActivity.MyButtons btn = (CameraDemoActivity.MyButtons)v;
            mPlayerView.setFilterWithConfig(btn.filterConfig);
            mCurrentConfig = btn.filterConfig;
        }
    };

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
                    }, playCompletionCallback);
                }
            default: break;
        }
    }

    @Override
    public void onPause() {
        super.onPause();
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
