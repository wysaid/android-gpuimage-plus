package org.wysaid.cgedemo;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;

import org.wysaid.library.CGENativeLibrary;

public class MainActivity extends ActionBarActivity {

    private Bitmap _bitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        CGENativeLibrary.globalInit();

        ImageView iv = (ImageView) findViewById(R.id.mainImageView);
        iv.setImageResource(R.drawable.bgview);

        BitmapDrawable a = (BitmapDrawable)  iv.getDrawable();

        _bitmap = a.getBitmap();
//        _bitmap = Bitmap.createBitmap(3000, 3000, Bitmap.Config.ARGB_8888);
        CGENativeLibrary.filterImage_MultipleEffectsWriteBack(_bitmap, "@adjust brightness 1.2", 1.0f);

        iv.setImageBitmap(_bitmap);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
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
