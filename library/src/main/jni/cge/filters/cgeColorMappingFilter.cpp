/*
* cgeColorMappingFilter.cpp
*
*  Created on: 2016-8-5
*      Author: Wang Yang
* Description: 色彩映射
*/

#include "cgeColorMappingFilter.h"
#include "cgeTextureUtils.h"
#include <algorithm>

static CGEConstString s_vshMapingBuffered= CGE_SHADER_STRING
(
attribute vec2 vPosition;
attribute vec2 vTexPosition;
varying vec2 textureCoordinate;


void main()
{
	gl_Position = vec4(vPosition * 2.0 - 1.0, 0.0, 1.0);
	textureCoordinate = vTexPosition;
}
);


static CGEConstString s_fshMapingBuffered = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
void main()
{
	gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
// 	gl_FragColor.rg = textureCoordinate;
// 	gl_FragColor.a = 1.0;
}
);

namespace CGE
{
	class CGEColorMappingFilterBuffered_Area : public CGEColorMappingFilter
	{
		CGEColorMappingFilterBuffered_Area() : m_mappingVertBuffer(0), m_mappingTexVertBuffer(0), m_drawer(nullptr) {}
	public:
		~CGEColorMappingFilterBuffered_Area()
		{
			delete m_drawer;
			CGE_DELETE_GL_OBJS(glDeleteBuffers, m_mappingVertBuffer, m_mappingTexVertBuffer);
		}

		static inline CGEColorMappingFilterBuffered_Area* create()
		{
			CGEColorMappingFilterBuffered_Area* f = new CGEColorMappingFilterBuffered_Area();
			f->m_drawer = TextureDrawer::create();
			if(!f->init() || f->m_drawer == nullptr)
			{
				delete f;
				f = nullptr;
			}
			return f;
		}

		bool init()
		{
			m_program.bindAttribLocation("vTexPosition", 1);
			if(m_program.initWithShaderStrings(s_vshMapingBuffered, s_fshMapingBuffered))
			{
				return true;
			}
			return false;
		}

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
		{
			assert(m_mappingAreas.size() > 0 && m_cacheBufferData.size() >= m_texUnitResolution.width * m_texUnitResolution.height && m_texVertBufferData.size() >= m_texUnitResolution.width * m_texUnitResolution.height * 6);

			handler->setAsTarget();
			glViewport(0, 0, m_texUnitResolution.width, m_texUnitResolution.height);
			m_drawer->drawTexture(srcTexture);

			glFinish();
			glReadPixels(0, 0, m_texUnitResolution.width, m_texUnitResolution.height, GL_RGBA, GL_UNSIGNED_BYTE, m_cacheBufferData.data());

			int index = 0;
			const int cnt = m_texUnitResolution.width * m_texUnitResolution.height;
			const int mappingSize = (int)m_mappingAreas.size() - 1;

			for(int i = 0; i != cnt; ++i)
			{
				const auto& v = m_cacheBufferData[i];
				const int mappingIndex = v[1] * (mappingSize / 255.0f);
				const auto& m = m_mappingAreas[mappingIndex];
				const auto& a = m.area;
				const Vec2f rb(a[0] + a[2], a[1] + a[3]);

				m_texVertBufferData[index] = Vec2f(a[0], a[1]);
				m_texVertBufferData[index + 1] = Vec2f(rb[0], a[1]);
				m_texVertBufferData[index + 2] = Vec2f(a[0], rb[1]);

				m_texVertBufferData[index + 3] = m_texVertBufferData[index + 1];
				m_texVertBufferData[index + 4] = rb;
				m_texVertBufferData[index + 5] = m_texVertBufferData[index + 2];
				index += 6;
			}

			handler->setAsTarget();
			m_program.bind();

			glBindBuffer(GL_ARRAY_BUFFER, m_mappingVertBuffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, m_mappingTexVertBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_texVertBufferData.size() * sizeof(m_texVertBufferData[0]), m_texVertBufferData.data());
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mappingTexture);

			glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
		}

		void setupMapping(GLuint mappingTex, int texWidth, int texHeight, int texUnitWidth, int texUnitHeight)
		{
			CGEColorMappingFilter::setupMapping(mappingTex, texWidth, texHeight, texUnitWidth, texUnitHeight);
			
			m_cacheBufferData.resize(texUnitWidth * texUnitHeight);
			m_texVertBufferData.resize(texUnitWidth * texUnitHeight * 6);

			if(m_mappingVertBuffer == 0)
				glGenBuffers(1, &m_mappingVertBuffer);

			if(m_mappingTexVertBuffer == 0)
				glGenBuffers(1, &m_mappingTexVertBuffer);

			CGESizef mappingSize(m_texSize.width / (float)m_texUnitResolution.width, m_texSize.height / (float)m_texUnitResolution.height);

			std::vector<Vec2f> vertBuf(m_texUnitResolution.width * m_texUnitResolution.height * 6);

			int index = 0;
			float w = m_texUnitResolution.width;
			float h = m_texUnitResolution.height;
			for(int i = 0; i != m_texUnitResolution.height; ++i)
			{
				for(int j = 0; j != m_texUnitResolution.width; ++j)
				{
					vertBuf[index] = Vec2f(j / w, i / h);
					vertBuf[index + 1] = Vec2f((j + 1) / w, i / h);
					vertBuf[index + 2] = Vec2f(j / w, (i + 1) / h);

					vertBuf[index + 3] = vertBuf[index + 1];
					vertBuf[index + 4] = Vec2f((j + 1) / w, (i + 1) / h);
					vertBuf[index + 5] = vertBuf[index + 2];
					index += 6;
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_mappingVertBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(vertBuf[0]), vertBuf.data(), GL_STATIC_DRAW);
			m_drawCount = (int)vertBuf.size();

			glBindBuffer(GL_ARRAY_BUFFER, m_mappingTexVertBuffer);
			glBufferData(GL_ARRAY_BUFFER, m_texVertBufferData.size() * sizeof(m_texVertBufferData[0]), nullptr, GL_STREAM_DRAW);

		}

// 		void endPushing()
// 		{
// 			CGEColorMappingFilter::endPushing();
// 
// 			glGenBuffers(1, &m_mappingVertBuffer);
// 			glBindBuffer(GL_ARRAY_BUFFER, m_mappingVertBuffer);
// 
// 
// 
// 		}

	protected:

		GLuint m_mappingVertBuffer;
		GLuint m_mappingTexVertBuffer;
		std::vector<Vec4ub> m_cacheBufferData;
		std::vector<Vec2f> m_texVertBufferData;
		FrameBuffer m_framebuffer;
		int m_drawCount;
		TextureDrawer* m_drawer;
	};

	//////////////////////////////////////////////////////////////////////////

	CGEColorMappingFilter::CGEColorMappingFilter() : m_mappingTexture(0), m_texSize(0, 0)
	{

	}

	CGEColorMappingFilter::~CGEColorMappingFilter()
	{
		glDeleteTextures(1, &m_mappingTexture);
	}

	CGEColorMappingFilter* CGEColorMappingFilter::createWithMode(MapingMode mode)
	{
		switch (mode)
		{
		case MAPINGMODE_BUFFERED_AREA:
			return CGEColorMappingFilterBuffered_Area::create();
		case MAPINGMODE_SINGLE:
		default:
			break;
		}

		return nullptr;
	}

	void CGEColorMappingFilter::pushMapingArea(const MappingArea& area)
	{
		 m_mappingAreas.push_back(area);
	}

	void CGEColorMappingFilter::endPushing()
	{
		std::stable_sort(m_mappingAreas.begin(), m_mappingAreas.end());
	}

	void CGEColorMappingFilter::setupMapping(GLuint mappingTex, int texWidth, int texHeight, int texUnitWidth, int texUnitHeight)
	{
		m_mappingTexture = mappingTex;
		m_texSize.set(texWidth, texHeight);
		m_texUnitResolution.set(texUnitWidth, texUnitHeight);
	}

}