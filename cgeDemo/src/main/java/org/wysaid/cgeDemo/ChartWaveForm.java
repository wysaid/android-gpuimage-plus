package org.wysaid.cgeDemo.view;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL10;

public class ChartWaveForm {

    //缓冲区长度，点数
    public final static int BUFFER_SIZE = 1570464;//VideoPlayerGLSurfaceView.mViewWidth*VideoPlayerGLSurfaceView.mViewHeight=984*1596=1570464
    //缓冲区数组
    private float[] mChartDatas = new float[BUFFER_SIZE];
    //缓冲区缓存
    private FloatBuffer vertexBuffer;
    //顶点坐标数组
    private float[] vertices = new float[BUFFER_SIZE * 3];
    //绘图区域
    private int width, height;

    public ChartWaveForm() {

    }

    /**
     * 根据缓冲区数组 封装 顶点坐标数组
     */
    private void drawRealtimeChart() {
        //坐标系xyz，屏幕中心为原点坐标（0,0,0），左上角为（-1,1,0），右下角（1,-1,0）,坐标值有问题？好窄
        float span = 20.0f / 984;//VideoPlayerGLSurfaceView.mViewWidth=984
        //vertices的0,3,6……位置放x坐标值
        //vertices的1,4,7……位置放y坐标值
        //vertices的2,5,8……位置放z坐标值，平面图，默认为0
        for (int i = 0; i < BUFFER_SIZE; i++) {

            vertices[i * 3] = -10 + (i % 984) * span;//VideoPlayerGLSurfaceView.mViewWidth=984
//            vertices[i * 3] = -10 + i * span;
//            L.d("执行到这", "X轴坐标==" + vertices[i * 3]);

            vertices[i * 3 + 1] = mChartDatas[i];

            vertices[i * 3 + 2] = 0.0f;
        }
    }

    /**
     * 开辟对应的缓冲区存放顶点坐标数组
     */
    private void vertexGenerate() {
        //一个浮点数在内存中占4个字节，在内存中开辟指定长度的缓冲区，用来存放顶点坐标数组
        ByteBuffer vertexByteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);
        //设置字节处理规则，大端模式或者小端模式，设置为默认
        vertexByteBuffer.order(ByteOrder.nativeOrder());
        //将内存中分配的字节缓冲区转换成浮点数缓冲区
        vertexBuffer = vertexByteBuffer.asFloatBuffer();
        //存放顶点坐标数组
        vertexBuffer.put(vertices);
        //复位缓冲区，position指向0，第一个数据
        vertexBuffer.position(0);
    }

    public void setResolution(int width, int height) {
        this.width = width;
        this.height = height;
    }

    public void setChartData(float[] chartData) {
        this.mChartDatas = chartData;
        drawRealtimeChart();
        vertexGenerate();
    }

    public void draw(GL10 gl) {
        gl.glViewport(0, 0, width, height);
        // bind the previously generated texture 顶点绘图
        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        // set the color for the triangle 设置绘图颜色
        gl.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        // Point to our vertex buffer 设置顶点数据，3代表XYZ坐标系
        gl.glVertexPointer(3, GL10.GL_FLOAT, 0, vertexBuffer);
        // Line width
        gl.glPointSize(2.5f);
        // Draw the vertices as triangle strip，顶点之间的连接模式
        gl.glDrawArrays(GL10.GL_POINTS, 0, vertices.length / 3);
        //Disable the client state before leaving 关闭顶点设置
        gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
    }
}
