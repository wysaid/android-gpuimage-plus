/*
* cgeLookupFilter.h
*
*  Created on: 2016-7-4
*      Author: Wang Yang
* Description: 全图LUT滤镜
*/

#ifndef _CGE_LOOKUPFILTER_H_
#define _CGE_LOOKUPFILTER_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGELookupFilter : public CGEImageFilterInterface
	{
	public:
		CGELookupFilter();
		~CGELookupFilter();

		bool init();

		inline void setLookupTexture(GLuint tex) { m_lookupTexture = tex; };

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		inline GLuint& lookupTexture() { return m_lookupTexture; }

	protected: 
		GLuint m_lookupTexture;
	};

}

#endif