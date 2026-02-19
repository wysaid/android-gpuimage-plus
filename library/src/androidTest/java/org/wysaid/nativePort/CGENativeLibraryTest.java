package org.wysaid.nativePort;

import android.graphics.Bitmap;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.wysaid.test.GLTestBase;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class CGENativeLibraryTest extends GLTestBase {

    @Test
    public void filterImageMultipleEffectsReturnsNonNull() {
        Bitmap src = createTestBitmap();
        Bitmap dst = null;
        try {
            dst = CGENativeLibrary.filterImage_MultipleEffects(src, "@adjust brightness 0.5", 1.0f);
            assertNotNull("Filtered bitmap should not be null", dst);
            assertPixelChanged(src, dst, 32, 32, 2);
        } finally {
            src.recycle();
            if (dst != null && dst != src) {
                dst.recycle();
            }
        }
    }

    @Test
    public void createAndDeleteFilterWithConfig() {
        long filterAddress = CGENativeLibrary.cgeCreateFilterWithConfig("@adjust saturation 0.5", 1.0f);
        assertTrue("Filter address should be non-zero", filterAddress != 0L);
        CGENativeLibrary.cgeDeleteFilterWithAddress(filterAddress);
    }

    @Test
    public void emptyConfigHandledGracefully() {
        Bitmap src = createTestBitmap();
        try {
            Bitmap result = CGENativeLibrary.filterImage_MultipleEffects(src, "", 1.0f);
            assertSame("Empty config should return original bitmap", src, result);
        } finally {
            src.recycle();
        }
    }
}
