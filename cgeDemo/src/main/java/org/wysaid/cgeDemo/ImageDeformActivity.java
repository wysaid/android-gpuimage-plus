package org.wysaid.cgeDemo;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.common.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEDeformFilterWrapper;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.texUtils.TextureRenderer;
import org.wysaid.view.ImageGLSurfaceView;

import java.io.InputStream;

public class ImageDeformActivity extends AppCompatActivity {

    public static final int REQUEST_CODE_PICK_IMAGE = 1;
    private Bitmap mBitmap;
    private ImageGLSurfaceView mImageView;
    private CGEDeformFilterWrapper mDeformWrapper;
    private SeekBar mSeekbar;
    private float mTouchRadius = 200.0f;
    private float mTouchIntensaity = 0.1f;

    enum DeformMode {
        Restore,
        Forward,
        Bloat,
        Wrinkle
    }

    DeformMode mDeformMode = DeformMode.Forward;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_deform);

        mImageView = (ImageGLSurfaceView) findViewById(R.id.mainImageView);
        mBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.bgview);

        mImageView.setSurfaceCreatedCallback(new ImageGLSurfaceView.OnSurfaceCreatedCallback() {
            @Override
            public void surfaceCreated() {
                mImageView.setImageBitmap(mBitmap);
                mImageView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        int w = mBitmap.getWidth(), h = mBitmap.getHeight();
                        float scaling = Math.min(1280.0f / w, 1280.0f / h);
                        if (scaling < 1.0f) {
                            w *= scaling;
                            h *= scaling;
                        }
                        mDeformWrapper = CGEDeformFilterWrapper.create(w, h, 10.0f);
                        mDeformWrapper.setUndoSteps(200); // set max undo steps to 200.
                        if (mDeformWrapper != null) {
                            CGEImageHandler handler = mImageView.getImageHandler();
                            handler.setFilterWithAddres(mDeformWrapper.getNativeAddress());
                            handler.processFilters();
                        }
                    }
                });
            }
        });

        mImageView.setDisplayMode(ImageGLSurfaceView.DisplayMode.DISPLAY_ASPECT_FIT);

        mSeekbar = (SeekBar) findViewById(R.id.globalRestoreSeekBar);
        mSeekbar.setOnSeekBarChangeListener(mSeekListener);
        mImageView.setOnTouchListener(mTouchListener);
    }

    SeekBar.OnSeekBarChangeListener mSeekListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, final int progress, boolean fromUser) {
            if (mDeformWrapper != null) {
                mImageView.lazyFlush(true, new Runnable() {
                    @Override
                    public void run() {
                        if (mDeformWrapper != null) {
                            float intensity = progress / 100.0f;
                            mDeformWrapper.restoreWithIntensity(intensity);
                        }
                    }
                });
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    };

    View.OnTouchListener mTouchListener = new View.OnTouchListener() {

        float mLastX, mLastY;
        boolean mIsMoving = false;
        boolean mHasMotion = false;

        @Override
        public boolean onTouch(View v, final MotionEvent event) {

            if (mDeformWrapper == null) {
                return false;
            }

            TextureRenderer.Viewport viewport = mImageView.getRenderViewport();
            final float w = viewport.width;
            final float h = viewport.height;
            final float x = (event.getX() - viewport.x);
            final float y = (event.getY() - viewport.y);

            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    mIsMoving = true;
                    mLastX = x;
                    mLastY = y;
                    if (!mDeformWrapper.canUndo()) {
                        mDeformWrapper.pushDeformStep();
                    }
                    return true;
                case MotionEvent.ACTION_MOVE:
                    break;
                case MotionEvent.ACTION_CANCEL:
                case MotionEvent.ACTION_UP:
                    mIsMoving = false;
                    if (mHasMotion) {
                        mImageView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                if (mImageView != null) {
                                    mDeformWrapper.pushDeformStep();
                                    Log.i(Common.LOG_TAG, "Init undo status...");
                                }
                            }
                        });
                    }
                default:
                    return true;
            }

            if (mSeekbar.getProgress() != 0) {
                mSeekbar.setOnSeekBarChangeListener(null);
                mSeekbar.setProgress(0);
                mSeekbar.setOnSeekBarChangeListener(mSeekListener);
            }

            final float lastX = mLastX;
            final float lastY = mLastY;

            mImageView.lazyFlush(true, new Runnable() {
                @Override
                public void run() {
                    if (mDeformWrapper == null)
                        return;
                    switch (mDeformMode) {
                        case Restore:
                            mDeformWrapper.restoreWithPoint(x, y, w, h, mTouchRadius, mTouchIntensaity);
                            break;
                        case Forward:
                            mDeformWrapper.forwardDeform(lastX, lastY, x, y, w, h, mTouchRadius, mTouchIntensaity);
                            break;
                        case Bloat:
                            mDeformWrapper.bloatDeform(x, y, w, h, mTouchRadius, mTouchIntensaity);
                            break;
                        case Wrinkle:
                            mDeformWrapper.wrinkleDeform(x, y, w, h, mTouchRadius, mTouchIntensaity);
                            break;
                    }

                    mHasMotion = true;
                }
            });

            mLastX = x;
            mLastY = y;
            return true;
        }
    };

    @Override
    protected void onPause() {
        super.onPause();

        if (mDeformWrapper != null) {
            mDeformWrapper.release(false);
            mDeformWrapper = null;
        }
        mImageView.release();
        mImageView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mImageView.onResume();
    }

    public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
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

    public void galleryBtnClicked(View view) {
        Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
        photoPickerIntent.setType("image/*");
        startActivityForResult(photoPickerIntent, REQUEST_CODE_PICK_IMAGE);
    }

    public void saveImageBtnClicked(View view) {
        mImageView.getResultBitmap(new ImageGLSurfaceView.QueryResultBitmapCallback() {
            @Override
            public void get(final Bitmap bmp) {
                if (bmp == null) {
                    MsgUtil.toastMsg(ImageDeformActivity.this, "Get bitmap failed!");
                    return;
                }
                String s = ImageUtil.saveBitmap(bmp);
                sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + s)));
            }
        });
    }

    public void restoreBtnClicked(View view) {
        mImageView.flush(true, new Runnable() {
            @Override
            public void run() {
                if (mDeformWrapper != null) {
                    mDeformWrapper.restore();
                    mImageView.requestRender();
                }
            }
        });
    }

    public void forwardModeBtnClicked(View view) {
        mDeformMode = DeformMode.Forward;
        MsgUtil.toastMsg(this, "Forward mode! Please touch the image view");
    }

    public void restoreModeBtnClicked(View view) {
        mDeformMode = DeformMode.Restore;
        MsgUtil.toastMsg(this, "Restore Mode! Please touch the image view");
    }

    public void bloatModeBtnClicked(View view) {
        mDeformMode = DeformMode.Bloat;
        MsgUtil.toastMsg(this, "Bloat mode! Please touch the image view");
    }

    public void wrinkleModeBtnClicked(View view) {
        mDeformMode = DeformMode.Wrinkle;
        MsgUtil.toastMsg(this, "Wrinkle Mode! Please touch the image view");
    }

    void checkRadius() {
        if (mTouchRadius < 10.0f) {
            mTouchRadius = 10.0f;
        } else if (mTouchRadius > 400.0f) {
            mTouchRadius = 400.0f;
        }
    }

    public void radiusIncClicked(View view) {
        mTouchRadius += 10.0;
        checkRadius();
        MsgUtil.toastMsg(this, "Radius increased to " + mTouchRadius);
    }

    public void radiusDecClicked(View view) {
        mTouchRadius -= 10.0f;
        checkRadius();

        MsgUtil.toastMsg(this, "Radius decreased to " + mTouchRadius);
    }

    void checkIntensity() {
        if (mTouchIntensaity < 0.02f) {
            mTouchIntensaity = 0.02f;
        } else if (mTouchIntensaity > 0.9f) {
            mTouchIntensaity = 0.9f;
        }
    }

    public void intensityIncClicked(View view) {
        mTouchIntensaity += 0.05;
        checkIntensity();
        MsgUtil.toastMsg(this, "intensity increased to " + mTouchIntensaity);
    }

    public void intensityDecClicked(View view) {
        mTouchIntensaity -= 0.05;
        checkIntensity();
        MsgUtil.toastMsg(this, "intensity decreased to " + mTouchIntensaity);
    }

    public void undoBtnClicked(View view) {
        mImageView.flush(true, new Runnable() {
            @Override
            public void run() {
                if (mDeformWrapper != null) {
                    if (!mDeformWrapper.undo()) {
                        mImageView.post(new Runnable() {
                            @Override
                            public void run() {
                                MsgUtil.toastMsg(ImageDeformActivity.this, "Nothing to undo!");
                            }
                        });
                    } else {
                        mImageView.requestRender();
                    }
                }
            }
        });
    }

    public void redoBtnClicked(View view) {
        mImageView.flush(true, new Runnable() {
            @Override
            public void run() {
                if (mDeformWrapper != null) {
                    if (!mDeformWrapper.redo()) {
                        mImageView.post(new Runnable() {
                            @Override
                            public void run() {
                                MsgUtil.toastMsg(ImageDeformActivity.this, "Nothing to redo!");
                            }
                        });
                    } else {
                        mImageView.requestRender();
                    }
                }
            }
        });
    }

    boolean mShowMesh = false;

    public void showMeshBtnClicked(View view) {
        mImageView.flush(true, new Runnable() {
            @Override
            public void run() {
                if (mDeformWrapper != null) {
                    mShowMesh = !mShowMesh;
                    mDeformWrapper.showMesh(mShowMesh);
                    mImageView.requestRender();
                }
            }
        });
    }
}
