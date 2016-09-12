/*
 * cgeBrightnessAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 */

#ifndef _CGEBRIGHTNESSADJUST_H_
#define _CGEBRIGHTNESSADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEBrightnessFilter : public CGEImageFilterInterface
	{
	public:

		void setIntensity(float value); // range: [-1, 1]

		bool init();
        
        void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramName;
        
    private:
        float m_intensity;
	};

	class CGEBrightnessFastFilter : public CGEFastAdjustRGBFilter
	{
	public:

		void setIntensity(float value);
		bool init();
	};

}

#endif