
package org.wysaid.cgeDemo.view;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Message;
import android.view.View;

public class HistogramView extends View implements Handler.Callback {
    static final private String TAG = "HistogramView";

    private static final int HISTOGRAM = 0;

    private Paint paint;
    private Handler handler;
    //    private Converter converter;
    private Configuration config;

    private int width;
    private int height;
    private float max;

    private float[] histogram;
    private long count;

    // HistogramView
    public HistogramView(Context context) {
        super(context);
        paint = new Paint();
        handler = new Handler(this);
//        converter = new Converter(getContext());
    }

    // onSizeChanged
    @Override
    public void onSizeChanged(int w, int h, int oldw, int oldh) {
        width = w;
        height = h;

        config = getResources().getConfiguration();
    }

    // onDraw
    @Override
    public void onDraw(Canvas canvas) {
        if (histogram == null) return;

        canvas.drawColor(Color.BLACK);

        float xscale = (float) width / 256;
        float yscale = (float) height / 10000;

        paint.setStrokeWidth(xscale);
        paint.setColor(Color.WHITE);

        for (int x = 0; x < 256; x++) {
            float xpos = x * xscale;
            float ypos;
            if (histogram[x] < 100000) {
                ypos = histogram[x] * yscale;
            } else {
                ypos = 100000 * yscale;
            }
            canvas.drawLine(xpos, height, xpos + xscale, height - ypos, paint);
        }
    }

    public void onPreviewFrame(float[] data, VideoPlayerGLSurfaceView view) {
        if (data != null) {
//            if (count++ % 2 == 0) {
            Message message = handler.obtainMessage(HISTOGRAM, view.getViewWidth(), view.getViewheight(), data);
            message.sendToTarget();
//            }
        }
    }

    @Override
    public boolean handleMessage(Message message) {
        // process incoming messages here
        histogram = (float[]) message.obj;
//        int width = message.arg1;
//        int height = message.arg2;
//        byte[] data = (byte[]) message.obj;
//
//        byte[] pixels = converter.convertToRGB(data, width, height);
//        histogram =
//                converter.luminanceHistogram(data, width, height);
//                converter.histogram(pixels, width, height);
        invalidate();
        return true;
    }
}
