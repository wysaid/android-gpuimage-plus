package org.wysaid.nativePort;

import android.graphics.Bitmap;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.wysaid.test.GLTestBase;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class CGEImageHandlerTest extends GLTestBase {

    @Test
    public void initWithBitmapSucceeds() {
        CGEImageHandler handler = new CGEImageHandler();
        Bitmap src = createTestBitmap();
        try {
            assertTrue("initWithBitmap should succeed", handler.initWithBitmap(src));
        } finally {
            handler.release();
            src.recycle();
        }
    }

    @Test
    public void initWithSizeSucceeds() {
        CGEImageHandler handler = new CGEImageHandler();
        try {
            assertTrue("initWithSize should succeed", handler.initWithSize(64, 64));
        } finally {
            handler.release();
        }
    }

    @Test
    public void basicFilterPipelineReturnsValidBitmap() {
        CGEImageHandler handler = new CGEImageHandler();
        Bitmap src = createTestBitmap();
        Bitmap dst = null;
        try {
            assertTrue(handler.initWithBitmap(src));
            handler.setFilterWithConfig("@adjust brightness 0.5");
            handler.processFilters();
            dst = handler.getResultBitmap();

            assertNotNull("Result bitmap should not be null", dst);
            assertEquals(64, dst.getWidth());
            assertEquals(64, dst.getHeight());
            assertPixelChanged(src, dst, 32, 32, 2);
        } finally {
            handler.release();
            src.recycle();
            if (dst != null) {
                dst.recycle();
            }
        }
    }

    @Test
    public void compositeFilterConfigWorks() {
        CGEImageHandler handler = new CGEImageHandler();
        Bitmap src = createTestBitmap();
        Bitmap dst = null;
        try {
            assertTrue(handler.initWithBitmap(src));
            handler.setFilterWithConfig("@adjust contrast 1.2 @adjust saturation 0.8");
            handler.processFilters();
            dst = handler.getResultBitmap();
            assertNotNull("Result bitmap should not be null", dst);
            assertPixelChanged(src, dst, 32, 32, 2);
        } finally {
            handler.release();
            src.recycle();
            if (dst != null) {
                dst.recycle();
            }
        }
    }

    @Test
    public void releaseIsSafeWhenCalledTwice() {
        CGEImageHandler handler = new CGEImageHandler();
        Bitmap src = createTestBitmap();
        try {
            assertTrue(handler.initWithBitmap(src));
            handler.release();
            handler.release();
        } finally {
            src.recycle();
        }
    }
}
