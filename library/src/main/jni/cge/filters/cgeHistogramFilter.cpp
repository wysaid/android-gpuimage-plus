#include "cgeHistogramFilter.h"

#include <EGL/egl.h>

static CGEConstString s_vshHistogram = "#version 320 es\n" CGE_SHADER_STRING_PRECISION_H(
layout(location = 0) in vec2 position;
layout(location = 0) out vec2 textureCoordinate;
void main() {
	gl_Position = vec4(position, 0.0, 1.0);
	textureCoordinate = (position.xy + 1.0) / 2.0;
});

static CGEConstString s_fshHistogram = "#version 320 es\n" CGE_SHADER_STRING(
		precision highp float;
precision highp int;
// GLSL代码
layout(location = 0) in vec2 textureCoordinate;
layout(binding = 0) uniform sampler2D inputImageTexture;
layout (binding = 1, rgba32f) uniform writeonly highp image2D histogram; // 直方图
void main() {
	vec4 color = texture(inputImageTexture, textureCoordinate);

	int binIndex = int(clamp(color.r * 255.0f, 0.0f, 254.99999));

	imageAtomicAdd(histogram, ivec2(binIndex), 1);
});


namespace CGE
{
	CGEHistogramFilter::~CGEHistogramFilter(){}

	bool CGEHistogramFilter::init()
	{
		if (initShadersFromString(s_vshHistogram, s_fshHistogram))
		{
			m_program.bind();
			setFormPosition(0.1f, 0.1f);
			setFormSize(0.3f, 0.3f);
			setColor(0.0f, 0.0f, 0.0f, 0.5f);
			m_drawer.reset(TextureDrawer::create());
			m_drawer->setFlipScale(1.0f, -1.0f); // flip upside down, meet the gl coord.
			m_renderTarget = std::make_unique<FrameBufferWithTexture>();
			return true;
		}

		return false;
	}

	void CGEHistogramFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		auto&& sz = handler->getOutputFBOSize();
		if (sz.width != m_renderTarget->width() || m_renderTarget->texture() == 0)
		{
			m_renderTarget->bindTexture2D(256, sz.height);
		}

		m_renderTarget->bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glFinish();

		m_program.bind();
		/// 渲染不写入, 使用 imageStore 写入.
		glColorMask(false, false, false, false);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindImageTexture(1, m_renderTarget->texture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glColorMask(true, true, true, true);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		handler->setAsTarget();
		glViewport(m_position[0] * sz.width, m_position[1] * sz.height, m_size[0] * sz.width, m_size[1] * sz.height);
		m_drawer->drawTexture(m_renderTarget->texture());
	}

	void CGEHistogramFilter::setFormPosition(float left, float top)
	{
		m_position = { left, top };
	}

	void CGEHistogramFilter::setFormSize(float width, float height)
	{
		m_size = { width, height };
	}

	void CGEHistogramFilter::setColor(float r, float g, float b, float a)
	{
		m_color = { r, g, b, a };
	}
} // namespace CGE