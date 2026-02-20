package org.wysaid.cgeDemo;

import android.content.ContentValues;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.wysaid.common.Common;
import org.wysaid.common.SharedContext;
import org.wysaid.myUtils.FileUtil;
import org.wysaid.myUtils.MsgUtil;
import org.wysaid.nativePort.CGEFFmpegNativeLibrary;
import org.wysaid.nativePort.CGEImageHandler;
import org.wysaid.nativePort.CGENativeLibrary;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class TestCaseActivity extends AppCompatActivity {

    public static final String LOG_TAG = Common.LOG_TAG;

    protected Thread mThread;
    protected boolean mShouldStopThread = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_case);
        FileUtil.init(this);
    }

    protected void threadSync() {

        if (mThread != null && mThread.isAlive()) {
            mShouldStopThread = true;

            try {
                mThread.join();
                mThread = null;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        mShouldStopThread = false;
    }

    protected void showMsg(final String msg) {

        TestCaseActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                MsgUtil.toastMsg(TestCaseActivity.this, msg, Toast.LENGTH_SHORT);
            }
        });
    }

    // ========== Gallery helpers ==========

    /**
     * Returns true on API 29+ devices where direct filesystem writes to public directories
     * require the MediaStore IS_PENDING + fd approach.
     */
    private boolean needsScopedStorageWrite() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q;
    }

    /**
     * Creates an output path for a video file in the public Movies/libCGE directory.
     *
     * <p>On API 29+, inserts a pending MediaStore entry and returns {@code /proc/self/fd/N}
     * so native FFmpeg code can write directly without Scoped Storage restrictions.
     * Call {@link #publishVideoToGallery(Uri, ParcelFileDescriptor)} when done.
     *
     * <p>On API &lt; 29, returns a direct filesystem path in the public Movies directory.
     *
     * @param outUri  receives the MediaStore URI (API 29+ only, else null)
     * @param outPfd  receives the open ParcelFileDescriptor (API 29+ only, else null)
     * @return path to pass to native code, or null on failure
     */
    private String createVideoOutputPath(String displayName,
                                         Uri[] outUri,
                                         ParcelFileDescriptor[] outPfd) {
        if (needsScopedStorageWrite()) {
            ContentValues values = new ContentValues();
            values.put(MediaStore.Video.Media.DISPLAY_NAME, displayName);
            values.put(MediaStore.Video.Media.MIME_TYPE, "video/mp4");
            values.put(MediaStore.Video.Media.RELATIVE_PATH,
                    Environment.DIRECTORY_MOVIES + "/libCGE");
            values.put(MediaStore.Video.Media.IS_PENDING, 1);

            Uri collection = MediaStore.Video.Media.getContentUri(
                    MediaStore.VOLUME_EXTERNAL_PRIMARY);
            Uri uri = getContentResolver().insert(collection, values);
            if (uri == null) {
                Log.e(LOG_TAG, "createVideoOutputPath: MediaStore insert failed");
                return null;
            }
            outUri[0] = uri;

            try {
                ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "rw");
                if (pfd == null) {
                    getContentResolver().delete(uri, null, null);
                    return null;
                }
                outPfd[0] = pfd;
                String path = "/proc/self/fd/" + pfd.getFd();
                Log.i(LOG_TAG, "createVideoOutputPath: " + path + " -> " + displayName);
                return path;
            } catch (Exception e) {
                Log.e(LOG_TAG, "createVideoOutputPath: " + e);
                getContentResolver().delete(uri, null, null);
                outUri[0] = null;
                return null;
            }

        } else {
            outUri[0] = null;
            outPfd[0] = null;
            File dir = new File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES),
                    "libCGE");
            if (!dir.exists() && !dir.mkdirs()) {
                Log.e(LOG_TAG, "createVideoOutputPath: mkdirs failed: " + dir);
                return null;
            }
            return new File(dir, displayName).getAbsolutePath();
        }
    }

    /** Finalises a video created by {@link #createVideoOutputPath}. */
    private void publishVideoToGallery(Uri mediaUri, ParcelFileDescriptor pfd, String legacyPath) {
        if (needsScopedStorageWrite() && mediaUri != null) {
            if (pfd != null) {
                try { pfd.close(); } catch (Exception ignored) {}
            }
            ContentValues values = new ContentValues();
            values.put(MediaStore.Video.Media.IS_PENDING, 0);
            getContentResolver().update(mediaUri, values, null, null);
            Log.i(LOG_TAG, "publishVideoToGallery: cleared IS_PENDING for " + mediaUri);
        } else if (legacyPath != null) {
            MediaScannerConnection.scanFile(this,
                    new String[]{legacyPath}, new String[]{"video/mp4"},
                    (p, uri) -> Log.i(LOG_TAG, "publishVideoToGallery: scan done " + p));
        }
    }

    /**
     * Saves a Bitmap to the public Pictures/libCGE directory and notifies the gallery.
     *
     * @return the display name (filename) of the saved image, or null on failure.
     */
    private String saveBitmapToGallery(Bitmap bmp, String displayName) {
        if (needsScopedStorageWrite()) {
            ContentValues values = new ContentValues();
            values.put(MediaStore.Images.Media.DISPLAY_NAME, displayName);
            values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
            values.put(MediaStore.Images.Media.RELATIVE_PATH,
                    Environment.DIRECTORY_PICTURES + "/libCGE");
            values.put(MediaStore.Images.Media.IS_PENDING, 1);

            Uri collection = MediaStore.Images.Media.getContentUri(
                    MediaStore.VOLUME_EXTERNAL_PRIMARY);
            Uri uri = getContentResolver().insert(collection, values);
            if (uri == null) {
                Log.e(LOG_TAG, "saveBitmapToGallery: MediaStore insert failed");
                return null;
            }

            try (OutputStream out = getContentResolver().openOutputStream(uri)) {
                if (out == null) throw new IOException("openOutputStream returned null");
                bmp.compress(Bitmap.CompressFormat.JPEG, 95, out);
            } catch (Exception e) {
                Log.e(LOG_TAG, "saveBitmapToGallery: write failed - " + e);
                getContentResolver().delete(uri, null, null);
                return null;
            }

            ContentValues pub = new ContentValues();
            pub.put(MediaStore.Images.Media.IS_PENDING, 0);
            getContentResolver().update(uri, pub, null, null);
            Log.i(LOG_TAG, "saveBitmapToGallery: published " + displayName);
            return displayName;

        } else {
            File dir = new File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES),
                    "libCGE");
            if (!dir.exists() && !dir.mkdirs()) {
                Log.e(LOG_TAG, "saveBitmapToGallery: mkdirs failed: " + dir);
                return null;
            }
            File file = new File(dir, displayName);
            try (java.io.FileOutputStream fos = new java.io.FileOutputStream(file)) {
                bmp.compress(Bitmap.CompressFormat.JPEG, 95, fos);
            } catch (Exception e) {
                Log.e(LOG_TAG, "saveBitmapToGallery: write failed - " + e);
                return null;
            }
            MediaScannerConnection.scanFile(this,
                    new String[]{file.getAbsolutePath()}, new String[]{"image/jpeg"},
                    (p, uri) -> Log.i(LOG_TAG, "saveBitmapToGallery: scan done " + p));
            return displayName;
        }
    }

    // ========== Test cases ==========

    public void testCaseOffscreenVideoRendering(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {

                Log.i(LOG_TAG, "Test case 1 clicked!\n");

                String inputFileName = FileUtil.getTextContent(CameraDemoActivity.lastVideoPathFileName);
                if (inputFileName == null) {
                    showMsg("No video is recorded, please record one in the 2nd case.");
                    return;
                }

                Bitmap bmp;
                try {
                    AssetManager am = getAssets();
                    InputStream is = am.open("logo.png");
                    bmp = BitmapFactory.decodeStream(is);
                } catch (IOException e) {
                    Log.e(LOG_TAG, "Can not open blend image file!");
                    bmp = null;
                }

                String displayName = "blendVideo_" + System.currentTimeMillis() + ".mp4";
                Uri[] outUri = {null};
                ParcelFileDescriptor[] outPfd = {null};
                String outputPath = createVideoOutputPath(displayName, outUri, outPfd);

                if (outputPath == null) {
                    showMsg("Failed to create output path!");
                    return;
                }

                CGEFFmpegNativeLibrary.generateVideoWithFilter(
                        outputPath, inputFileName,
                        "@adjust lut late_sunset.png", 1.0f,
                        bmp, CGENativeLibrary.TextureBlendMode.CGE_BLEND_ADDREV, 1.0f, false);

                String publicPath = needsScopedStorageWrite()
                        ? "Movies/libCGE/" + displayName
                        : outputPath;
                publishVideoToGallery(outUri[0], outPfd[0], outputPath);

                showMsg("Done! Saved to: " + publicPath);
                Log.i(LOG_TAG, "Done! Saved to: " + publicPath);
            }
        });

        mThread.start();
    }

    public void testCaseCustomFilter(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {

                Bitmap src = BitmapFactory.decodeResource(TestCaseActivity.this.getResources(), R.drawable.bgview);
                int maxIndex = CGENativeLibrary.cgeGetCustomFilterNum();
                SharedContext glContext = SharedContext.create();
                glContext.makeCurrent();
                for (int i = 0; i != maxIndex && !mShouldStopThread; ++i) {
                    Bitmap dst = CGENativeLibrary.cgeFilterImageWithCustomFilter(src, i, 1.0f, true, true);
                    String name = "custom_filter_" + i + "_" + System.currentTimeMillis() + ".jpg";
                    String saved = saveBitmapToGallery(dst, name);
                    dst.recycle();
                    showMsg(saved != null
                            ? "Filter applied! Saved: Pictures/libCGE/" + saved
                            : "Filter applied but save failed!");
                }

                glContext.release();
            }
        });

        mThread.start();
    }

    public void testCaseConfigStringFilters(View view) {

        threadSync();

        mThread = new Thread(new Runnable() {
            @Override
            public void run() {
                Bitmap src = BitmapFactory.decodeResource(TestCaseActivity.this.getResources(), R.drawable.bgview);
                int maxIndex = MainActivity.EFFECT_CONFIGS.length;

                SharedContext glContext = SharedContext.create();
                glContext.makeCurrent();

                CGEImageHandler handler = new CGEImageHandler();
                handler.initWithBitmap(src);

                for (int i = 0; i != maxIndex && !mShouldStopThread; ++i) {

                    final String filterConfig = MainActivity.EFFECT_CONFIGS[i];
                    handler.setFilterWithConfig(filterConfig);
                    handler.processFilters();

                    Bitmap dst = handler.getResultBitmap();
                    if (dst == null) {
                        Log.e(LOG_TAG, "getResultBitmap returns null!");
                        continue;
                    }

                    String name = "config_filter_" + i + "_" + System.currentTimeMillis() + ".jpg";
                    String saved = saveBitmapToGallery(dst, name);
                    dst.recycle();

                    showMsg(saved != null
                            ? "Config \"" + filterConfig + "\" applied! Saved: Pictures/libCGE/" + saved
                            : "Config \"" + filterConfig + "\" applied but save failed!");
                }

                glContext.release();
            }
        });

        mThread.start();
    }

    @Override
    protected void onDestroy() {
        threadSync();
        super.onDestroy();
    }
}
