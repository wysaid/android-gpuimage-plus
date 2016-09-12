/*
* cgeSketchFilter.h
*
*  Created on: 2015-3-20
*      Author: Wang Yang
*/

#ifndef _CGE_SKETCHFILTER_H_
#define _CGE_SKETCHFILTER_H_

#include "cgeMaxValueFilter.h"
#include "cgeMultipleEffects.h"

namespace CGE
{
	class CGESketchFilter : public CGEImageFilterInterface
	{
	public:

		CGESketchFilter();
		~CGESketchFilter();

		bool init();

		void setIntensity(float intensity);

		void render2Texture(CGE::CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void flush();

	protected:

		static CGEConstString paramCacheTextureName;
		static CGEConstString paramIntensityName;
		
		CGEMaxValueFilter3x3 m_maxValueFilter;
		GLuint m_textureCache;
		CGESizei m_cacheSize;
	};
}

#endif