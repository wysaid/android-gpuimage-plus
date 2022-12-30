package org.wysaid.cgeDemo;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

public class ChartGLSurfaceView extends GLSurfaceView {

    //线程标志位
    private boolean isUpdating = false;
    //渲染Renderer
    private ChartGLRenderer mRenderer;
    //y坐标数组
    private float[] datapoints = new float[ChartWaveForm.BUFFER_SIZE];

    public ChartGLSurfaceView(Context context) {
        super(context);
        //设置EGL配置选择
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        //设置处于屏幕最前边
        this.setZOrderOnTop(true); //necessary
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        // Set the Renderer for drawing on the GLSurfaceView
        mRenderer = new ChartGLRenderer(context);
        setRenderer(mRenderer);
        //初始化
        for (int i = 0; i < datapoints.length; i++) {
            datapoints[i] = 0;
        }
        setChartData(datapoints);
        // Render the view only when there is a change in the drawing data，有变化时绘制
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        new Thread(new Task()).start();
    }

    /**
     * 设置数据源
     * @param datapoints
     */
    public void setChartData(float[] datapoints) {
//        L.d("执行到这","decodeThread---02");
        if (datapoints != null && datapoints.length > 0) {
            isUpdating = true;
            this.datapoints = datapoints.clone();
//            float gMaxValue = getMax(datapoints);
//            float gMinValue = getMin(datapoints);
//            for (int i = 0; i < this.datapoints.length; i++) {
//                this.datapoints[i] = (((datapoints[i] - gMinValue) * (1.0f - (-1.0f)) / (gMaxValue - gMinValue)) + (-1));
////                L.d("执行到这","this.datapoints[i]=="+String.valueOf(this.datapoints[i]));
//            }
            isUpdating = false;
//            L.d("执行到这","gMaxValue==="+gMaxValue+"---gMinValue==="+gMinValue);
        }
    }

    /**
     * 获取数组最大值
     * @param array
     * @return
     */
    private float getMax(float[] array) {
        if(array != null && array.length > 0){
            float max = array[0];
            for (int i = 1; i < array.length; i++) {
                if (array[i] > max) {
                    max = array[i];
                }
            }
            return max;
        } else {
            return 0f;
        }
    }

    /**
     * 获取数组最小值
     * @param array
     * @return
     */
    private float getMin(float[] array) {
        if(array != null && array.length > 0){
            float min = array[0];
            for (int i = 1; i < array.length; i++) {
                if (array[i] < min) {
                    min = array[i];
                }
            }
            return min;
        } else {
            return 0f;
        }
    }

    class Task implements Runnable {
        @Override
        public void run() {
            while (true) {
                if (!isUpdating) {
                    mRenderer.chartData = datapoints;
                    requestRender();
                }
//                try {
//                    Thread.sleep(30);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
            }
        }
    }

}
