package org.wysaid.cgeDemo;

import android.os.Bundle;
import android.util.Log;
import androidx.appcompat.app.AppCompatActivity;
import org.wysaid.nativePort.NativeLibraryLoader;

/**
 * Simple test activity to demonstrate 16KB page size detection
 */
public class PageSizeTestActivity extends AppCompatActivity {
    
    private static final String TAG = "PageSizeTest";
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Log.i(TAG, "Starting 16KB page size detection test...");
        
        // This will trigger JNI_OnLoad which runs our 16KB page size check
        NativeLibraryLoader.load();
        
        Log.i(TAG, "CGE library loaded - check logcat for 16KB page size status");
        
        // You can also check programmatically if needed  
        Log.i(TAG, "Test completed - see native logs above for 16KB page size status");
        
        finish(); // This is just a test activity
    }
}