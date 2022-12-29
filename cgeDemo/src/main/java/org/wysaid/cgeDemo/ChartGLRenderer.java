package org.wysaid.cgeDemo.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChartGLRenderer implements GLSurfaceView.Renderer {

    public volatile float[] chartData = new float[ChartWaveForm.BUFFER_SIZE];
    private int width;
    private int height;
    private Context context;
    private ChartWaveForm lineChart;

    public ChartGLRenderer(Context context) {
        this.context = context;
        lineChart = new ChartWaveForm();
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        //lineChart = new LineChart();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        this.width = width;
        this.height = height;
        //Prevent a divide by 0 by making height =1
        if (height == 0) {
            height = 1;
        }
        Log.d("执行到这", "ChartSurfaceView---Width =="+width+"----Height =="+height);
        //Reset current viewport
        gl.glViewport(0, 0, width, height);
        //Select Projection Matrix
        gl.glMatrixMode(GL10.GL_PROJECTION);
        //Reset Projection Matrix
        gl.glLoadIdentity();
        //Calculate The Aspect Ratio Of The Window
        //Log.d("Chart Ratio2 "," width " +width + " H " + height);
        //void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
        //fovy是眼睛上下睁开的幅度，角度值，值越小，视野范围越狭小（眯眼），值越大，视野范围越宽阔（睁开铜铃般的大眼）；
        //zNear表示近裁剪面到眼睛的距离，zFar表示远裁剪面到眼睛的距离，注意zNear和zFar不能设置设置为负值（你怎么看到眼睛后面的东西）。
        //aspect表示裁剪面的宽w高h比，这个影响到视野的截面有多大。
        GLU.gluPerspective(gl, 50.0f, (float) height * 2.0f / (float) width, 0.1f, 100.0f);
        gl.glMatrixMode(GL10.GL_MODELVIEW);     //Select The Modelview Matrix
        gl.glLoadIdentity();                    //Reset The Modelview Matrix
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // clear Screen and Depth Buffer
        gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        // Reset the Modelview Matrix
        gl.glLoadIdentity();
        //Move 5 units into the screen is the same as moving the camera 5 units away
        gl.glTranslatef(0.0f, 0.0f, -3.0f);
        this.lineChart.setResolution(width, height);
        this.lineChart.setChartData(chartData);
        lineChart.draw(gl);
    }
}
