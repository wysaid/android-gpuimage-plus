package org.wysaid.cgeDemo

import android.graphics.BitmapFactory
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.doOnLayout
import org.wysaid.cgeDemo.view.MatrixMutualViewGroup

/**
 * @author PengHaiChen
 * @date 2024/9/30 21:32:29
 * @description
 */
class ImageDemoWithMatrixActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_image_demo_with_matrix)

        val demoView: MatrixMutualViewGroup = findViewById(R.id.demo_view)
        val mBitmap = BitmapFactory.decodeResource(resources, R.mipmap.test_image)
        demoView.doOnLayout {
            demoView.initBitmap(mBitmap)
            demoView.setFilter(MainActivity.EFFECT_CONFIGS[4])
        }

    }
}