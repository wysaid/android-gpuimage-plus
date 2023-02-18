#ifndef _HISTOGRAMFILTER_H_
#define _HISTOGRAMFILTER_H_

#include "cgeImageFilter.h"
#include "cgeTextureUtils.h"
#include "cgeVec.h"
namespace CGE
{
	class CGEHistogramFilter : public CGEImageFilterInterface
	{
	public:
		~CGEHistogramFilter() override;

		/**
         * @brief 左上角的点
         */
		void setFormPosition(float left, float top);
		/**
         * @brief 相对大小
         */
		void setFormSize(float width, float height);

		/**
         * @brief 背景色
         */
		void setColor(float r, float g, float b, float a);

		bool init() override;

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID) override;

	protected:
		std::unique_ptr<TextureDrawer> m_drawer;
		std::unique_ptr<FrameBufferWithTexture> m_renderTarget;
		Vec2f m_position;
		Vec2f m_size;
		Vec4f m_color;
	};
} // namespace CGE

#endif