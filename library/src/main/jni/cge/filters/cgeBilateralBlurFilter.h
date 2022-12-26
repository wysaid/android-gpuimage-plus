/*
* cgeBilateralBlurFilter.h
*
*  Created on: 2014-4-1
*      Author: Wang Yang
*/

#ifndef _CGE_BLIATERALBLUR_H_
#define _CGE_BLIATERALBLUR_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
    //blur with fixed radius.
	class CGEBilateralBlurFilter : public CGEAdvancedEffectTwoStepFilterHelper
	{
	public:
		CGEBilateralBlurFilter() {}
		~CGEBilateralBlurFilter() {}

		bool init();

		//Range: value >= 0.0, and 0.0 for the origin. Default: 4.0
        virtual void setBlurScale(float value);

		//Range: [1.0, 20.0], none for the origin. Default: 8.0  (It'll show the origin when blurScale value is 1.0
		void setDistanceNormalizationFactor(float value);

	protected:
		static CGEConstString paramDistanceFactorName;
		static CGEConstString paramBlurSamplerScaleName;
		static CGEConstString paramBlurFactorsName;
	};

    //not blur with fixed radius.
    class CGEBilateralBlurBetterFilter : public CGEBilateralBlurFilter
    {
    public:
        bool init();
        void setSamplerRadiusLimit(int limit);
        virtual void setBlurScale(float value);

    protected:
        static CGEConstString paramBlurRadiusName;
        int m_limit;
    };

	//特殊用法
    class CGEBilateralWrapperFilter : public CGEImageFilterInterface
    {
    public:

		CGEBilateralWrapperFilter() : m_repeatTimes(1) {}
		~CGEBilateralWrapperFilter() { delete m_proc; }

        bool init();

        void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);


		//Range: [-100, 100]. the value would be transformed basing on the input image size.
        void setBlurScale(float value)
        {
            m_blurScale = value;
        }

        //Range: [1.0, 20.0], none for the origin. Default: 8.0  (It'll show the origin when blurScale value is 1.0
        void setDistanceNormalizationFactor(float value)
        {
            m_proc->setDistanceNormalizationFactor(value);
        }

		void setRepeatTimes(int n)
		{
			m_repeatTimes = n;
		}

        CGEBilateralBlurFilter *m_proc;
		float m_blurScale;
		int m_repeatTimes;
    };
}

#endif
