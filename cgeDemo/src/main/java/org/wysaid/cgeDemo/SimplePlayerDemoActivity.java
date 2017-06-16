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

import org.wysaid.common.Common;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.texUtils.TextureRendererDrawOrigin;
import org.wysaid.texUtils.TextureRendererEdge;
import org.wysaid.texUtils.TextureRendererEmboss;
import org.wysaid.texUtils.TextureRendererLerpBlur;
import org.wysaid.texUtils.TextureRendererMask;
import org.wysaid.texUtils.TextureRendererWave;
import org.wysaid.view.SimplePlayerGLSurfaceView;

public class SimplePlayerDemoActivity extends AppCompatActivity {

    SimplePlayerGLSurfaceView mPlayerView;
    Button mShapeBtn;
    Button mTakeshotBtn;
    Button mGalleryBtn;

    public static final int REQUEST_CODE_PICK_VIDEO = 1;

    private SimplePlayerGLSurfaceView.PlayCompletionCallback playCompletionCallback = new SimplePlayerGLSurfaceView.PlayCompletionCallback() {
        @Override
        public void playComplete(MediaPlayer player) {
            Log.i(Common.LOG_TAG, "The video playing is over, restart...");
            player.start();
        }

        @Override
        public boolean playFailed(MediaPlayer player, final int what, final int extra)
        {
            MsgUtil.toastMsg(SimplePlayerDemoActivity.this, String.format("Error occured! Stop playing, Err code: %d, %d", what, extra));
            return true;
        }
    };

    class MyVideoButton extends Button implements View.OnClickListener {

        Uri videoUri;
        SimplePlayerGLSurfaceView videoView;

        public MyVideoButton(Context context) {
            super(context);
        }

        @Override
        public void onClick(View v) {

            MsgUtil.toastMsg(SimplePlayerDemoActivity.this, "Preparing for " + videoUri.getHost() + videoUri.getPath() + " If the video is from the internet, you may wait a while...");

            videoView.setVideoUri(videoUri, new SimplePlayerGLSurfaceView.PlayPreparedCallback() {
                @Override
                public void playPrepared(MediaPlayer player) {
                    mPlayerView.post(new Runnable() {
                        @Override
                        public void run() {
                            String msg = "Start playing ";
                            if(videoUri.getHost() != null)
                                msg += videoUri.getHost();
                            if(videoUri.getPath() != null)
                                msg += videoUri.getPath();

                            MsgUtil.toastMsg(SimplePlayerDemoActivity.this, msg);
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
        setContentView(R.layout.activity_simple_player_demo);
        mPlayerView = (SimplePlayerGLSurfaceView)findViewById(R.id.videoGLSurfaceView);
//        mPlayerView.setZOrderOnTop(false);

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
                        mPlayerView.setMaskBitmap(bmp, false, new SimplePlayerGLSurfaceView.SetMaskBitmapCallback() {
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
                mPlayerView.takeShot(new SimplePlayerGLSurfaceView.TakeShotCallback() {
                    @Override
                    public void takeShotOK(Bitmap bmp) {
                        if(bmp != null) {
                            final String s = ImageUtil.saveBitmap(bmp);
                            sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
                            mPlayerView.post(new Runnable() {
                                @Override
                                public void run() {
                                    MsgUtil.toastMsg(SimplePlayerDemoActivity.this, "file " + s + " saved!");
                                }
                            });
                        }
                        else {
                            Log.e(Common.LOG_TAG, "take shot failed!");
                        }
                    }
                });
            }
        });

        LinearLayout menuLayout = (LinearLayout)findViewById(R.id.menuLinearLayout);

        String[] filePaths = {
                "android.resource://" + getPackageName() + "/" + R.raw.test,
                "http://wge.wysaid.org/res/video/1.mp4",  //video from internet
                "http://wysaid.org/p/test.mp4",   //video from internet
        };

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
                        MsgUtil.toastMsg(SimplePlayerDemoActivity.this, "No video is recorded, please record one in the 2nd case.");
                        return;
                    }

                    Uri lastVideoUri = Uri.parse(lastVideoFileName);
                    mPlayerView.setVideoUri(lastVideoUri, new SimplePlayerGLSurfaceView.PlayPreparedCallback() {
                        @Override
                        public void playPrepared(MediaPlayer player) {
                            Log.i(Common.LOG_TAG, "The video is prepared to play");
                            player.start();
                        }
                    }, playCompletionCallback);
                }
            });
        }

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

        Button filterButton = new Button(this);
        menuLayout.addView(filterButton);
        filterButton.setText("switch filter");
        filterButton.setOnClickListener(new View.OnClickListener() {
            private int filterIndex;

            @Override
            public void onClick(View v) {
                mPlayerView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        TextureRenderer drawer = null;
                        ++filterIndex;
                        filterIndex %= 5;
                        switch (filterIndex) {
                            case 0: //LerpBlur
                            {
                                TextureRendererLerpBlur lerpBlurDrawer = TextureRendererLerpBlur.create(true);
                                if(lerpBlurDrawer != null) {
                                    lerpBlurDrawer.setIntensity(16);
                                    drawer = lerpBlurDrawer;
                                }
                            }
                            break;

                            case 1:
                            {
                                TextureRendererEdge edgeDrawer = TextureRendererEdge.create(true);
                                if(edgeDrawer != null) {
                                    drawer = edgeDrawer;
                                }
                            }
                            break;

                            case 2:
                            {
                                TextureRendererEmboss embossDrawer = TextureRendererEmboss.create(true);
                                if(embossDrawer != null) {
                                    drawer = embossDrawer;
                                }
                            }
                            break;

                            case 3:
                            {
                                TextureRendererWave waveDrawer = TextureRendererWave.create(true);
                                if(waveDrawer != null) {
                                    waveDrawer.setAutoMotion(0.4f);
                                    drawer = waveDrawer;
                                }
                            }
                            break;

                            case 4:
                            {
                                TextureRendererDrawOrigin originDrawer = TextureRendererDrawOrigin.create(true);
                                if(originDrawer != null) {
                                    drawer = originDrawer;
                                }
                            }
                            break;

                            default:break;
                        }

                        mPlayerView.setTextureRenderer(drawer);
                    }
                });
            }
        });

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

        mPlayerView.setPlayerInitializeCallback(new SimplePlayerGLSurfaceView.PlayerInitializeCallback() {
            @Override
            public void initPlayer(final MediaPlayer player) {
                //Check for remote video
                player.setOnBufferingUpdateListener(new MediaPlayer.OnBufferingUpdateListener() {
                    @Override
                    public void onBufferingUpdate(MediaPlayer mp, int percent) {
                        Log.i(Common.LOG_TAG, "Buffer update: " + percent);
                        if(percent == 100) {
                            Log.i(Common.LOG_TAG, "video is loaded!");
                            player.setOnBufferingUpdateListener(null);
                        }
                    }
                });
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

                    mPlayerView.setVideoUri(data.getData(), new SimplePlayerGLSurfaceView.PlayPreparedCallback() {
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
        Log.i(SimplePlayerGLSurfaceView.LOG_TAG, "activity onPause...");
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
