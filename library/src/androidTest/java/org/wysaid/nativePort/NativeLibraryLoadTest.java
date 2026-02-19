package org.wysaid.nativePort;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class NativeLibraryLoadTest {

    @Test
    public void libraryLoadsWithoutException() {
        NativeLibraryLoader.load();
    }

    @Test
    public void customFilterCountIsValid() {
        NativeLibraryLoader.load();
        assertTrue("Custom filter count should be non-negative", CGENativeLibrary.cgeGetCustomFilterNum() >= 0);
    }
}
