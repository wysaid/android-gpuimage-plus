package org.wysaid.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.net.Uri;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;

import org.wysaid.common.Common;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.texUtils.TextureRendererDrawOrigin;
import org.wysaid.texUtils.TextureRendererMask;

import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/8/20.
 */
public class SimplePlayerGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final String LOG_TAG = Common.LOG_TAG;

    private SurfaceTexture mSurfaceTexture;
    private int mVideoTextureID;
    private TextureRenderer mDrawer;

    //setTextureRenderer 必须在OpenGL 线程调用!
    public void setTextureRenderer(TextureRenderer drawer) {
        if (mDrawer == null) {
            Log.e(LOG_TAG, "Invalid Drawer!");
            return;
        }

        if (mDrawer != drawer) {
            mDrawer.release();
            mDrawer = drawer;
            calcViewport();
        }
    }

    private TextureRenderer.Viewport mRenderViewport = new TextureRenderer.Viewport();
    private float[] mTransformMatrix = new float[16];
    private boolean mIsUsingMask = false;

    public boolean isUsingMask() {
        return mIsUsingMask;
    }

    private float mMaskAspectRatio = 1.0f;
    private float mDrawerFlipScaleX = 1.0f;
    private float mDrawerFlipScaleY = 1.0f;

    private int mViewWidth = 1000;
    private int mViewHeight = 1000;

    public int getViewWidth() {
        return mViewWidth;
    }

    public int getViewheight() {
        return mViewHeight;
    }

    private int mVideoWidth = 1000;
    private int mVideoHeight = 1000;

    private boolean mFitFullView = false;

    public void setFitFullView(boolean fit) {
        mFitFullView = fit;
        if (mDrawer != null)
            calcViewport();
    }

    private MediaPlayer mPlayer;

    private Uri mVideoUri;

    public interface PlayerInitializeCallback {

        //对player 进行初始化设置， 设置未默认启动的listener， 比如 bufferupdateListener.
        void initPlayer(MediaPlayer player);
    }

    public void setPlayerInitializeCallback(PlayerInitializeCallback callback) {
        mPlayerInitCallback = callback;
    }

    PlayerInitializeCallback mPlayerInitCallback;

    public interface PlayPreparedCallback {
        void playPrepared(MediaPlayer player);
    }

    PlayPreparedCallback mPreparedCallback;

    public interface PlayCompletionCallback {
        void playComplete(MediaPlayer player);


        /*

        what 取值: MEDIA_ERROR_UNKNOWN,
                  MEDIA_ERROR_SERVER_DIED

        extra 取值 MEDIA_ERROR_IO
                  MEDIA_ERROR_MALFORMED
                  MEDIA_ERROR_UNSUPPORTED
                  MEDIA_ERROR_TIMED_OUT

        returning false would cause the 'playComplete' to be called
        */
        boolean playFailed(MediaPlayer mp, int what, int extra);
    }

    PlayCompletionCallback mPlayCompletionCallback;

    public synchronized void setVideoUri(final Uri uri, final PlayPreparedCallback preparedCallback, final PlayCompletionCallback completionCallback) {

        mVideoUri = uri;
        mPreparedCallback = preparedCallback;
        mPlayCompletionCallback = completionCallback;

        if (mDrawer != null) {

            queueEvent(new Runnable() {
                @Override
                public void run() {
                    Log.i(LOG_TAG, "setVideoUri...");

                    if (mSurfaceTexture == null || mVideoTextureID == 0) {
                        mVideoTextureID = Common.genSurfaceTextureID();
                        mSurfaceTexture = new SurfaceTexture(mVideoTextureID);
                        mSurfaceTexture.setOnFrameAvailableListener(SimplePlayerGLSurfaceView.this);
                    }
                    _useUri();
                }
            });
        }
    }

    //根据传入bmp回调不同
    //若设置之后使用mask， 则调用 setMaskOK
    //否则调用 unsetMaskOK
    public interface SetMaskBitmapCallback {
        void setMaskOK(TextureRendererMask renderer);

        void unsetMaskOK(TextureRenderer renderer);
    }

    public void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle) {
        setMaskBitmap(bmp, shouldRecycle, null);
    }

    public synchronized void setMaskBitmap(final Bitmap bmp, final boolean shouldRecycle, final SetMaskBitmapCallback callback) {

        if (mDrawer == null) {
            Log.e(LOG_TAG, "setMaskBitmap after release!");
            return;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (bmp == null) {
                    Log.i(LOG_TAG, "Cancel Mask Bitmap!");

                    setMaskTexture(0, 1.0f);

                    if (callback != null) {
                        callback.unsetMaskOK(mDrawer);
                    }

                    return;
                }

                Log.i(LOG_TAG, "Use Mask Bitmap!");

                int texID[] = {0};
                GLES20.glGenTextures(1, texID, 0);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texID[0]);
                GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bmp, 0);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

                setMaskTexture(texID[0], bmp.getWidth() / (float) bmp.getHeight());

                if (callback != null && mDrawer instanceof TextureRendererMask) {
                    callback.setMaskOK((TextureRendererMask) mDrawer);
                }
                if (shouldRecycle)
                    bmp.recycle();

            }
        });
    }

    public synchronized void setMaskTexture(int texID, float aspectRatio) {
        Log.i(LOG_TAG, "setMaskTexture... ");

        if (texID == 0) {
            if (mDrawer instanceof TextureRendererMask) {
                mDrawer.release();
                mDrawer = TextureRendererDrawOrigin.create(true);
            }
            mIsUsingMask = false;
        } else {
            if (!(mDrawer instanceof TextureRendererMask)) {
                mDrawer.release();
                TextureRendererMask drawer = TextureRendererMask.create(true);
                assert drawer != null : "Drawer Create Failed!";
                drawer.setMaskTexture(texID);
                mDrawer = drawer;
            }
            mIsUsingMask = true;
        }

        mMaskAspectRatio = aspectRatio;
        calcViewport();
    }

    public synchronized MediaPlayer getPlayer() {
        if (mPlayer == null) {
            Log.e(LOG_TAG, "Player is not initialized!");
        }
        return mPlayer;
    }

    public interface OnCreateCallback {
        void createOK();
    }

    private OnCreateCallback mOnCreateCallback;

    //定制一些初始化操作
    public void setOnCreateCallback(final OnCreateCallback callback) {

        assert callback != null : "无意义操作!";

        if (mDrawer == null) {
            mOnCreateCallback = callback;
        } else {
            // 已经创建完毕， 直接执行
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    callback.createOK();
                }
            });
        }
    }

    public SimplePlayerGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        Log.i(LOG_TAG, "MyGLSurfaceView Construct...");

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        setZOrderOnTop(true);

        Log.i(LOG_TAG, "MyGLSurfaceView Construct OK...");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        Log.i(LOG_TAG, "video player onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);

        mDrawer = TextureRendererDrawOrigin.create(true);
        if (mDrawer == null) {
            Log.e(LOG_TAG, "Create Drawer Failed!");
            return;
        }
        if (mOnCreateCallback != null) {
            mOnCreateCallback.createOK();
        }

        if (mVideoUri != null && (mSurfaceTexture == null || mVideoTextureID == 0)) {
            mVideoTextureID = Common.genSurfaceTextureID();
            mSurfaceTexture = new SurfaceTexture(mVideoTextureID);
            mSurfaceTexture.setOnFrameAvailableListener(SimplePlayerGLSurfaceView.this);
            _useUri();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        mViewWidth = width;
        mViewHeight = height;

        calcViewport();
    }

    //must be in the OpenGL thread!
    public void release() {

        Log.i(LOG_TAG, "Video player view release...");

        if (mPlayer != null) {
            queueEvent(new Runnable() {
                @Override
                public void run() {

                    Log.i(LOG_TAG, "Video player view release run...");

                    if (mPlayer != null) {

                        mPlayer.setSurface(null);
                        if (mPlayer.isPlaying())
                            mPlayer.stop();
                        mPlayer.release();
                        mPlayer = null;
                    }

                    if (mDrawer != null) {
                        mDrawer.release();
                        mDrawer = null;
                    }

                    if (mSurfaceTexture != null) {
                        mSurfaceTexture.release();
                        mSurfaceTexture = null;
                    }

                    if (mVideoTextureID != 0) {
                        GLES20.glDeleteTextures(1, new int[]{mVideoTextureID}, 0);
                        mVideoTextureID = 0;
                    }

                    mIsUsingMask = false;
                    mPreparedCallback = null;
                    mPlayCompletionCallback = null;

                    Log.i(LOG_TAG, "Video player view release OK");
                }
            });
        }
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "surfaceview onPause ...");

        super.onPause();
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if (mSurfaceTexture == null) {
            return;
        }

        mSurfaceTexture.updateTexImage();

        if (!mPlayer.isPlaying()) {
            return;
        }

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glViewport(0, 0, mViewWidth, mViewHeight);

        mSurfaceTexture.getTransformMatrix(mTransformMatrix);
        mDrawer.setTransform(mTransformMatrix);

        mDrawer.renderTexture(mVideoTextureID, mRenderViewport);

    }

    private long mTimeCount2 = 0;
    private long mFramesCount2 = 0;
    private long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        requestRender();

        if (mLastTimestamp2 == 0)
            mLastTimestamp2 = System.currentTimeMillis();

        long currentTimestamp = System.currentTimeMillis();

        ++mFramesCount2;
        mTimeCount2 += currentTimestamp - mLastTimestamp2;
        mLastTimestamp2 = currentTimestamp;
        if (mTimeCount2 >= 1e3) {
            Log.i(LOG_TAG, String.format("播放帧率: %d", mFramesCount2));
            mTimeCount2 -= 1e3;
            mFramesCount2 = 0;
        }
    }

    private void calcViewport() {
        float scaling;

        if (mIsUsingMask) {
            flushMaskAspectRatio();
            scaling = mMaskAspectRatio;
        } else {
            mDrawer.setFlipscale(mDrawerFlipScaleX, mDrawerFlipScaleY);
            scaling = mVideoWidth / (float) mVideoHeight;
        }

        float viewRatio = mViewWidth / (float) mViewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if (mFitFullView) {
            //撑满全部view(内容大于view)
            if (s > 1.0) {
                w = (int) (mViewHeight * scaling);
                h = mViewHeight;
            } else {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            }
        } else {
            //显示全部内容(内容小于view)
            if (s > 1.0) {
                w = mViewWidth;
                h = (int) (mViewWidth / scaling);
            } else {
                h = mViewHeight;
                w = (int) (mViewHeight * scaling);
            }
        }

        mRenderViewport.width = w;
        mRenderViewport.height = h;
        mRenderViewport.x = (mViewWidth - mRenderViewport.width) / 2;
        mRenderViewport.y = (mViewHeight - mRenderViewport.height) / 2;
        Log.i(LOG_TAG, String.format("View port: %d, %d, %d, %d", mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height));
    }

    private void _useUri() {

        if (mPlayer != null) {

            mPlayer.stop();
            mPlayer.reset();

        } else {
            mPlayer = new MediaPlayer();
        }

        try {
            mPlayer.setDataSource(getContext(), mVideoUri);

//            mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);

            mPlayer.setSurface(new Surface(mSurfaceTexture));

        } catch (Exception e) {
            e.printStackTrace();
            Log.e(LOG_TAG, "useUri failed");

            if (mPlayCompletionCallback != null) {
                this.post(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayCompletionCallback != null) {
                            if (!mPlayCompletionCallback.playFailed(mPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, MediaPlayer.MEDIA_ERROR_UNSUPPORTED))
                                mPlayCompletionCallback.playComplete(mPlayer);
                        }
                    }
                });
            }
            return;
        }

        if (mPlayerInitCallback != null) {
            mPlayerInitCallback.initPlayer(mPlayer);
        }

        mPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                if (mPlayCompletionCallback != null) {
                    mPlayCompletionCallback.playComplete(mPlayer);
                }
                Log.i(LOG_TAG, "Video Play Over");
            }
        });

        mPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mVideoWidth = mp.getVideoWidth();
                mVideoHeight = mp.getVideoHeight();

                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        calcViewport();
                    }
                });

                if (mPreparedCallback != null) {
                    mPreparedCallback.playPrepared(mPlayer);
                } else {
                    mp.start();
                }
//                requestRender(); //可能导致第一帧过快渲染 (先于surface texture 准备完成
                Log.i(LOG_TAG, String.format("Video resolution 1: %d x %d", mVideoWidth, mVideoHeight));
            }
        });

        mPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(MediaPlayer mp, int what, int extra) {

                if (mPlayCompletionCallback != null)
                    return mPlayCompletionCallback.playFailed(mp, what, extra);
                return false;
            }
        });

        try {
            mPlayer.prepareAsync();
        } catch (Exception e) {
            Log.i(LOG_TAG, String.format("Error handled: %s, play failure handler would be called!", e.toString()));
            if (mPlayCompletionCallback != null) {
                this.post(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayCompletionCallback != null) {
                            if (!mPlayCompletionCallback.playFailed(mPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, MediaPlayer.MEDIA_ERROR_UNSUPPORTED))
                                mPlayCompletionCallback.playComplete(mPlayer);
                        }
                    }
                });
            }
        }

    }

    private void flushMaskAspectRatio() {

        float dstRatio = mVideoWidth / (float) mVideoHeight;

        float s = dstRatio / mMaskAspectRatio;

        if (s > 1.0f) {
            mDrawer.setFlipscale(mDrawerFlipScaleX / s, mDrawerFlipScaleY);
        } else {
            mDrawer.setFlipscale(mDrawerFlipScaleX, s * mDrawerFlipScaleY);
        }
    }

    public interface TakeShotCallback {
        //You can recycle the bmp.
        void takeShotOK(Bitmap bmp);
    }

    public synchronized void takeShot(final TakeShotCallback callback) {
        assert callback != null : "callback must not be null!";

        if (mDrawer == null) {
            Log.e(LOG_TAG, "Drawer not initialized!");
            callback.takeShotOK(null);
            return;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                IntBuffer buffer = IntBuffer.allocate(mRenderViewport.width * mRenderViewport.height);

                GLES20.glReadPixels(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
                Bitmap bmp = Bitmap.createBitmap(mRenderViewport.width, mRenderViewport.height, Bitmap.Config.ARGB_8888);
                bmp.copyPixelsFromBuffer(buffer);

                Bitmap bmp2 = Bitmap.createBitmap(mRenderViewport.width, mRenderViewport.height, Bitmap.Config.ARGB_8888);

                Canvas canvas = new Canvas(bmp2);
                Matrix mat = new Matrix();
                mat.setTranslate(0.0f, -mRenderViewport.height / 2.0f);
                mat.postScale(1.0f, -1.0f);
                mat.postTranslate(0.0f, mRenderViewport.height / 2.0f);

                canvas.drawBitmap(bmp, mat, null);
                bmp.recycle();

                callback.takeShotOK(bmp2);
            }
        });

    }
}
