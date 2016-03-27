package org.wysaid.geometryUtils;

import android.opengl.GLES20;

import org.wysaid.common.ProgramObject;

/**
 * Created by wysaid on 16/3/1.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 * Tips: GeometryRenderer needs no resource(such as textures)
 */
public class GeometryRenderer {

    protected static final String vshDrawDefault = "" +
            "attribute vec2 vPosition;\n" +
            "uniform vec2 canvasSize;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4((vPosition / canvasSize) * 2.0 - 1.0, 0.0, 1.0);\n" +
            "}";

    private static final String fshDrawOrigin = "" +
            "precision mediump float;\n" +
            "uniform vec4 color;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_FragColor = color;\n" +
            "}";

    protected static final String POSITION_NAME = "vPosition";
    protected static final String COLOR_NAME = "color";
    protected static final String CANVAS_SIZE = "canvasSize";
//    protected int mColorLoc;

    protected ProgramObject mProgram;
    protected int mVertexBuffer;

    protected float mCanvasWidth;
    protected float mCanvasHeight;

    //////////////////////////////////////////////////////

    GeometryRenderer() {

    }

    protected boolean init() {
        mProgram = new ProgramObject();
        mProgram.bindAttribLocation(POSITION_NAME, 0);
        if(!mProgram.init(vshDrawDefault, fshDrawOrigin)) {
            release();
            return false;
        }
        setColor(1.0f, 1.0f, 1.0f, 1.0f);
        setCanvasSize(1.0f, 1.0f);
        return true;
    }

    public void release() {
        if(mProgram != null) {
            mProgram.release();
            mProgram = null;
        }

        if(mVertexBuffer != 0) {
            GLES20.glDeleteBuffers(1, new int[]{mVertexBuffer}, 0);
            mVertexBuffer = 0;
        }
    }

    public static GeometryRenderer create() {
        GeometryRenderer renderer = new GeometryRenderer();
        if(!renderer.init()) {
            renderer.release();
            renderer = null;
        }
        return renderer;
    }

    public void setColor(float r, float g, float b, float a) {
        mProgram.bind();
        mProgram.sendUniformf(COLOR_NAME, r, g, b, a);
    }

    public int getVertexBuffer() {
        return mVertexBuffer;
    }

    public void setVertexBuffer(int buffer) {
        mVertexBuffer = buffer;
    }

    public void setCanvasSize(float w, float h) {
        mCanvasWidth = w;
        mCanvasHeight = h;
        mProgram.bind();
        mProgram.sendUniformf(CANVAS_SIZE, w, h);
    }

    public ProgramObject getProgram() {
        return mProgram;
    }

    public void bindBufferAttrib() {
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(0);
        GLES20.glVertexAttribPointer(0, 2, GLES20.GL_FLOAT, false, 0, 0);
    }

    public void render(int mode, int first, int count) {

        bindBufferAttrib();
        mProgram.bind();
        GLES20.glDrawArrays(mode, first, count);
    }
}
