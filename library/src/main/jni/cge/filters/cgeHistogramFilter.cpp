#include "cgeHistogramFilter.h"

static CGEConstString  s_fshHistogram = CGE_SHADER_STRING_PRECISION_M
(
     varying vec2 textureCoordinate;

     uniform sampler2D inputImageTexture;

     uniform vec2 center;//坐标系原点

     uniform float xValue;//宽度(x轴长度)

     uniform float yValue;//高度(y轴长度)

     uniform vec3 bgColor;//背景颜色

    void main()
    {
     /**
        在以center为原点，宽xValue，高yValue，背景色为bgColor的矩形框中绘制亮度直方图。如何在这个库的基础上实现？
     */
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    }
);

namespace CGE
{
	CGEConstString CGEHistogramFilter::paramCenter = "center";
	CGEConstString CGEHistogramFilter::paramXValue = "xValue";
	CGEConstString CGEHistogramFilter::paramYValue = "yValue";
	CGEConstString CGEHistogramFilter::paramColor = "bgColor";

	bool CGEHistogramFilter::init()
    {
        if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHistogram))
        {
			setCenter(0.5f, 0.5f);
			setXValue(0.4f);
			setYValue(0.3f);
			setColor(0.0f, 0.0f, 0.0f);
            return true;
        }
        return false;
    }

	void CGEHistogramFilter::setCenter(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramCenter, x, y);
	}

	void CGEHistogramFilter::setXValue(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramXValue, value);
	}

	void CGEHistogramFilter::setYValue(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramYValue, value);
	}

	void CGEHistogramFilter::setColor(float r, float b, float g)
	{
		m_program.bind();
		m_program.sendUniformf(paramColor, r, b, g);
	}
}