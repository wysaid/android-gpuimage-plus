package org.wysaid.test;

import android.graphics.Bitmap;
import android.graphics.Color;

import androidx.annotation.NonNull;

import org.junit.After;
import org.junit.Before;
import org.wysaid.common.SharedContext;
import org.wysaid.nativePort.NativeLibraryLoader;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public abstract class GLTestBase {

    protected SharedContext sharedContext;

    @Before
    public void setUpGLContext() {
        NativeLibraryLoader.load();
        sharedContext = SharedContext.create();
        assertNotNull("SharedContext should be created", sharedContext);
        sharedContext.makeCurrent();
    }

    @After
    public void tearDownGLContext() {
        if (sharedContext != null) {
            sharedContext.release();
            sharedContext = null;
        }
    }

    @NonNull
    protected Bitmap createTestBitmap() {
        Bitmap bitmap = Bitmap.createBitmap(64, 64, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.argb(255, 64, 128, 192));
        return bitmap;
    }

    protected void assertPixelChanged(Bitmap src, Bitmap dst, int x, int y, int minDelta) {
        int srcPixel = src.getPixel(x, y);
        int dstPixel = dst.getPixel(x, y);

        int deltaA = Math.abs(Color.alpha(srcPixel) - Color.alpha(dstPixel));
        int deltaR = Math.abs(Color.red(srcPixel) - Color.red(dstPixel));
        int deltaG = Math.abs(Color.green(srcPixel) - Color.green(dstPixel));
        int deltaB = Math.abs(Color.blue(srcPixel) - Color.blue(dstPixel));
        int totalDelta = deltaA + deltaR + deltaG + deltaB;

        assertTrue("Filtered pixel should change (total delta=" + totalDelta + ")", totalDelta >= minDelta);
    }
}
