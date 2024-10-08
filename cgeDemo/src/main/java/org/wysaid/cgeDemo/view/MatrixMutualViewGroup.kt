package org.wysaid.cgeDemo.view

import android.animation.TypeEvaluator
import android.animation.ValueAnimator
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Matrix
import android.graphics.Rect
import android.graphics.RectF
import android.os.Build
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.widget.FrameLayout
import androidx.core.graphics.toRectF
import org.wysaid.cgeDemo.demoUtils.MoveGestureDetector

private const val TAG = "MatrixMutualViewGroup"

/**
 * @author PengHaiChen
 * @date 2024/9/30 21:10:58
 * @description
 */
class MatrixMutualViewGroup : FrameLayout {
    constructor(context: Context) : super(context)
    constructor(context: Context, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)


    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh) //判断图片当前是否合法
        val bitmap = if (currBitmap == null) return else currBitmap!!
        val srcMatrix = Matrix(syncTotalMatrix)
        val dstMatrix = Matrix(syncTotalMatrix)
        getFixMatrix(bitmap, srcMatrix, dstMatrix)
        syncTotalMatrix = dstMatrix
        syncMatrix2Child()
    }


    private var matrixMutualImageView = MatrixMutualImageView(this.context)

    init {
        addView(matrixMutualImageView)
    }

    private var syncTotalMatrix = Matrix()


    override fun onTouchEvent(event: MotionEvent?): Boolean {
        if (event == null) return true
        moveGestureDetector.onTouchEvent(event)
        scaleGestureDetector.onTouchEvent(event)
        when (event.action and MotionEvent.ACTION_MASK) {

            MotionEvent.ACTION_UP -> {
                animFixSyncMatrix()
            }

            MotionEvent.ACTION_CANCEL -> {
            }

            else -> {}
        }
        return true
    }

    private var moveGestureDetector = MoveGestureDetector(context, object : MoveGestureDetector.SimpleOnMoveGestureListener() {
        override fun onMove(detector: MoveGestureDetector?): Boolean {
            detector?.let {
                val d = detector.focusDelta
                syncTotalMatrix.postTranslate(d.x, d.y)
                syncMatrix2Child()
            }
            return true
        }

    })

    private var scaleGestureDetector = ScaleGestureDetector(context, object : ScaleGestureDetector.SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
            Log.i(TAG, "onScale: ${detector.scaleFactor}")
            syncTotalMatrix.postScale(detector.scaleFactor, detector.scaleFactor, detector.focusX, detector.focusY)
            syncMatrix2Child()
            return true
        }

    }).apply {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            isQuickScaleEnabled = false
        }
    }


    /**
     * 动画修复矩阵位置
     */
    private fun animFixSyncMatrix() {

        val bitmap = if (currBitmap == null) return else currBitmap!!
        val srcMatrix = Matrix(syncTotalMatrix)
        val dstMatrix = Matrix(syncTotalMatrix)

        getFixMatrix(bitmap, srcMatrix, dstMatrix)

        val animator = ValueAnimator.ofObject(MatrixEvaluator(), srcMatrix, dstMatrix)
        animator.duration = 300
        animator.addUpdateListener {
            val mat = it.animatedValue as Matrix
            syncTotalMatrix = mat
            syncMatrix2Child()
        }
        animator.start()
    }

    private fun getFixMatrix(bitmap: Bitmap, srcMatrix: Matrix, dstMatrix: Matrix) {
        val maxScale = 8F
        val minScale = 0.9F

        val imageOriginRect = Rect(0, 0, bitmap.width, bitmap.height).toRectF()
        val imageFixDstRect = Rect(0, 0, bitmap.width, bitmap.height).toRectF()
        val scale2target = Rect(0, 0, width, height).scale2dst(bitmap.width, bitmap.height)
        val xOffset = (width - (bitmap.width * scale2target)) / 2F
        val yOffset = (height - (bitmap.height * scale2target)) / 2F
        imageFixDstRect.set(xOffset, yOffset, xOffset + (bitmap.width * scale2target), yOffset + (bitmap.height * scale2target))

        val scaleFitImageRect = RectF()
        srcMatrix.mapRect(scaleFitImageRect, imageOriginRect)

        //判断放大越界
        if (scaleFitImageRect.width() > imageFixDstRect.width() * maxScale) {
            val scale = (imageFixDstRect.width() * maxScale) / scaleFitImageRect.width()
            Log.i(TAG, "animFixSyncMatrix: 过大${scale}")
            dstMatrix.postScale(scale, scale, width / 2F, height / 2F)
        } //判断缩小越界
        if (scaleFitImageRect.width() < imageFixDstRect.width() * minScale) {
            val scale = (imageFixDstRect.width() * minScale) / scaleFitImageRect.width()
            Log.i(TAG, "animFixSyncMatrix: 过小${scale}")
            dstMatrix.postScale(scale, scale, width / 2F, height / 2F)
        }

        val transFitImageRect = RectF()
        dstMatrix.mapRect(transFitImageRect, imageOriginRect)
        val limitTransW = imageFixDstRect.width() * minScale
        val limitTransH = imageFixDstRect.height() * minScale
        val limitTransXOffset = (width - limitTransW) / 2F
        val limitTransYOffset = (height - limitTransH) / 2F
        val imageFixMinDstRect = RectF(limitTransXOffset, limitTransYOffset, limitTransXOffset + limitTransW, limitTransYOffset + limitTransH)
        val dx = when {
            transFitImageRect.left > imageFixMinDstRect.left -> imageFixMinDstRect.left - transFitImageRect.left
            transFitImageRect.right < imageFixMinDstRect.right -> imageFixMinDstRect.right - transFitImageRect.right
            else -> 0F
        }

        val dy = when {
            transFitImageRect.top > imageFixMinDstRect.top -> imageFixMinDstRect.top - transFitImageRect.top
            transFitImageRect.bottom < imageFixMinDstRect.bottom -> imageFixMinDstRect.bottom - transFitImageRect.bottom
            else -> 0F
        }

        if (dx != 0F || dy != 0F) {
            dstMatrix.postTranslate(dx, dy)
            Log.i(TAG, "animFixSyncMatrix: dx = $dx, dy = $dy")
        }
    }

    private var currBitmap: Bitmap? = null
    fun initBitmap(bitmap: Bitmap) {
        val isEmptyMatrix = syncTotalMatrix.isIdentity
        if (isEmptyMatrix) syncTotalMatrix.reset()
        setBitmap(bitmap)
        if (isEmptyMatrix) {
            val scale2target = Rect(0, 0, width, height).scale2dst(bitmap.width, bitmap.height)
            syncTotalMatrix.postScale(scale2target, scale2target)
            val xOffset = (width - (bitmap.width * scale2target)) / 2F
            val yOffset = (height - (bitmap.height * scale2target)) / 2F
            syncTotalMatrix.postTranslate(xOffset, yOffset)
            syncMatrix2Child()
        }
    }

    private fun setBitmap(bitmap: Bitmap) {
        currBitmap = bitmap
        matrixMutualImageView.image = bitmap
    }


    fun setFilter(filterData: String?) = matrixMutualImageView.applyFilter(filterData)
    fun setIntensity(intensity: Float) {
        matrixMutualImageView.filterAdjust = intensity
    }

    private fun syncMatrix2Child() {
        matrixMutualImageView.syncMatrix = syncTotalMatrix
    }

    private fun Rect.scale2dst(width: Int, height: Int): Float {
        val widthScale = this.width() / width.toFloat()
        val heightScale = this.height() / height.toFloat()
        return minOf(widthScale, heightScale)
    }

    class MatrixEvaluator : TypeEvaluator<Matrix> {
        private val tempStartValues = FloatArray(9)
        private val tempEndValues = FloatArray(9)
        private val tempMatrix: Matrix = Matrix()
        override fun evaluate(fraction: Float, startValue: Matrix, endValue: Matrix): Matrix {
            startValue.getValues(tempStartValues)
            endValue.getValues(tempEndValues)
            for (i in 0..8) {
                val diff = tempEndValues[i] - tempStartValues[i]
                tempEndValues[i] = tempStartValues[i] + fraction * diff
            }
            tempMatrix.setValues(tempEndValues)
            return tempMatrix
        }
    }

}