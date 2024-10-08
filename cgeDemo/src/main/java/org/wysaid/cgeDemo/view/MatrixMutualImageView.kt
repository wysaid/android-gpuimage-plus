package org.wysaid.cgeDemo.view

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Matrix
import android.graphics.SurfaceTexture
import android.opengl.GLES20
import android.util.Log
import org.wysaid.cgeDemo.GLTextureView
import org.wysaid.nativePort.CGEImageHandler
import org.wysaid.texUtils.TextureRenderer
import org.wysaid.view.ImageGLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

private const val TAG = "MatrixMutualImageView"

/**
 * @author PengHaiChen
 * @date 2024/5/31 21:53:55
 * @description 可以使用矩阵交互,控制缩放和位移的demo
 */
class MatrixMutualImageView(context: Context) : GLTextureView(context), GLTextureView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    private var mImageHandler: CGEImageHandler? = null
    private var mRenderViewport: TextureRenderer.Viewport = TextureRenderer.Viewport()
    private var mDisplayMode: ImageGLSurfaceView.DisplayMode? = null
    private var mSettingIntensityLock: Any? = null
    private var mSettingIntensityCount: Int = 0
    private var mFilterIntensity: Float = 1.0f

    init {
        this.mDisplayMode = ImageGLSurfaceView.DisplayMode.DISPLAY_SCALE_TO_FILL
        this.mSettingIntensityLock = Any()
        this.mSettingIntensityCount = 1
        this.setEGLContextClientVersion(2)
        this.setEGLConfigChooser(8, 8, 8, 8, 8, 0)
        this.setRenderer(this)
        this.renderMode = 0
        Log.i("libCGE_java", "ImageGLSurfaceView Construct...")
    }

    private var mImageWidth: Int = 0
    private var mImageHeight: Int = 0
    private var originImage: Bitmap? = null
    private var scaleImageMatrix: Matrix = Matrix()


    var image: Bitmap? = null
        set(value) {
            field = value
            if (value == null) {
                return
            }
            originImage = value
            scaleImageMatrix.setScale(value.width / width.toFloat(), value.height / height.toFloat())
            setImageBitmap(value)
            applyFilter(cacheFilterData)
        }
    var syncMatrix: Matrix = Matrix()
        set(value) {
            field = value
            val finalMatrix = Matrix(value)
            finalMatrix.preConcat(scaleImageMatrix)
            setTransform(finalMatrix)
            invalidate()
        }

    var isParallel: Boolean = false
        set(value) {
            Log.i(TAG, "isParallel: 对比模式${value}")
            field = value
            if (value) {
                applyFilter(null)
            } else {
                applyFilter(cacheFilterData)
            }
        }


    private var cacheFilterData: String? = null

    var filterAdjust: Float = 0.6F
        set(value) {
            field = value
            setFilterIntensity(value.coerceIn(0F, 1F))
        }


    fun applyFilter(filter: String?) {
        if (filter != null) {
            cacheFilterData = filter
            val glsl = "@adjust lut $filter"
            Log.i(TAG, "applyFilter: 滤镜着色器为${glsl}")
            setFilterWithConfig(glsl)
            setFilterIntensity(filterAdjust.coerceIn(0F, 1F))
        } else {
            Log.i(TAG, "applyFilter: 空滤镜着色器")
            setFilterWithConfig("")
        }
    }

    @Synchronized
    fun setFilterWithConfig(config: String?) {
        queueEvent {
            if (mImageHandler != null) {
                mImageHandler?.setFilterWithConfig(config)
                requestRender()
            } else {
                Log.e(TAG, "setFilterWithConfig after release!!")
            }
        }
    }

    private fun setFilterIntensity(intensity: Float) {
        if (this.mImageHandler != null) {
            this.mFilterIntensity = intensity
            synchronized(mSettingIntensityLock!!) {
                if (this.mSettingIntensityCount <= 0) {
                    Log.i("libCGE_java", "Too fast, skipping...")
                    return
                }
                --this.mSettingIntensityCount
            }

            this.queueEvent {
                if (this.mImageHandler == null) {
                    Log.e("libCGE_java", "set intensity after release!!")
                } else {
                    this.mImageHandler?.setFilterIntensity(this.mFilterIntensity, true)
                    this.requestRender()
                }
                synchronized(this.mSettingIntensityLock!!) {
                    ++this.mSettingIntensityCount
                }
            }
        }
    }

    private fun setImageBitmap(bmp: Bitmap?) {
        if (bmp != null) {
            if (this.mImageHandler == null) {
                Log.e("libCGE_java", "Handler not initialized!")
            } else {
                this.mImageWidth = bmp.width
                this.mImageHeight = bmp.height
                queueEvent {
                    if (this.mImageHandler == null) {
                        Log.e("libCGE_java", "set image after release!!")
                    } else {
                        if (this.mImageHandler?.initWithBitmap(bmp) == true) {
                            this.calcViewport()
                            this.requestRender()
                        } else {
                            Log.e("libCGE_java", "setImageBitmap: init handler failed!")
                        }
                    }
                }
            }
        }
    }

    private fun calcViewport() {
        this.mRenderViewport.x = 0
        this.mRenderViewport.y = 0
        this.mRenderViewport.width = width
        this.mRenderViewport.height = height
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

        Log.i("libCGE_java", "ImageGLSurfaceView onSurfaceCreated...")
        GLES20.glDisable(2929)
        GLES20.glDisable(2960)
        this.mImageHandler = CGEImageHandler()
        this.mImageHandler?.setDrawerFlipScale(1.0f, -1.0f)
        image = image
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        Log.i(TAG, "onSurfaceChanged: $width $height")
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f)
        image?.let {
            scaleImageMatrix.setScale(it.width / width.toFloat(), it.height / height.toFloat())
        }
        this.calcViewport()
    }


    override fun onDrawFrame(gl: GL10?) {
        GLES20.glBindFramebuffer(36160, 0)
        GLES20.glClear(16384)
        if (this.mImageHandler != null) {
            GLES20.glViewport(mRenderViewport.x, mRenderViewport.y, mRenderViewport.width, mRenderViewport.height)
            mImageHandler!!.drawResult()
        }
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {
    }

}