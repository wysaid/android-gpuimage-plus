package org.wysaid.cgeDemo

import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.SeekBar
import android.widget.Spinner
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.doOnLayout
import org.wysaid.cgeDemo.view.MatrixMutualViewGroup

private const val TAG = "ImageDemoWithMatrixActi"

/**
 * @author PengHaiChen
 * @date 2024/9/30 21:32:29
 * @description
 */
class ImageDemoWithMatrixActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_image_demo_with_matrix)

        initView()
        initEvent()

    }

    private fun initEvent() {
        val demoView: MatrixMutualViewGroup = findViewById(R.id.demo_view)
        val intensity: SeekBar = findViewById(R.id.intensity)
        val spinner: Spinner = findViewById(R.id.filter_item)
        intensity.progress = intensity.max
        intensity.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(p0: SeekBar, p1: Int, p2: Boolean) {
                val progress = (p1.toFloat() / p0.max).coerceIn(0F, 1F)
                Log.i(TAG, "onProgressChanged: $progress")
                demoView.setIntensity(progress)
            }

            override fun onStartTrackingTouch(p0: SeekBar?) {
            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
            }

        })
        spinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(p0: AdapterView<*>?, p1: View?, p2: Int, p3: Long) {
                demoView.setFilter(MainActivity.EFFECT_CONFIGS[p2])
            }

            override fun onNothingSelected(p0: AdapterView<*>?) {

            }

        }
    }

    private fun initView() {
        val spinner: Spinner = findViewById(R.id.filter_item)
        val demoView: MatrixMutualViewGroup = findViewById(R.id.demo_view)
        val mBitmap = BitmapFactory.decodeResource(resources, R.mipmap.test_image)
        demoView.doOnLayout {
            demoView.initBitmap(mBitmap)
        }
        val effectConfigs: List<String> = MainActivity.EFFECT_CONFIGS.toList().map {
            it.ifBlank { "Select A Filter Apply To Image" }
        }
        val adapter = ArrayAdapter(this, android.R.layout.simple_spinner_item, effectConfigs)
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        spinner.adapter = adapter

    }
}