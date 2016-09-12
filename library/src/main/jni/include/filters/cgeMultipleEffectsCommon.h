/*
 * cgeMultipleEffectsCommon.h
 *
 *  Created on: 2014-1-2
 *      Author: Wang Yang
 */

#ifndef _CGEMUTIPLEEFFECTSCOMMON_H_
#define _CGEMUTIPLEEFFECTSCOMMON_H_

#include "cgeGLFunctions.h"
#include "cgeCurveAdjust.h"

namespace CGE
{
	void cgeEnableColorScale();
	void cgeDisableColorScale();	

	//////////////////////////////////////////////////////////////////////////

	class CGELomoFilter : public CGEImageFilterInterface
	{
	public:
		CGELomoFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}

		bool init();

		void setVignette(float start, float end);
		void setIntensity(float value);
		void setSaturation(float value);
		void setColorScale(float low, float range);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);


	protected:
		static CGEConstString paramColorScaleName;
		static CGEConstString paramSaturationName;
		static CGEConstString paramVignetteName;
		static CGEConstString paramAspectRatio;
		static CGEConstString paramIntensityName;

	private:
		GLfloat m_scaleDark, m_scaleLight, m_saturate;
	};

	class CGELomoLinearFilter : public CGELomoFilter
	{
	public:
		bool init();
	};

	//////////////////////////////////////////////////////////////////////////

	class CGELomoWithCurveFilter : public CGEMoreCurveFilter
	{
	public:
		CGELomoWithCurveFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}
		virtual bool init();

		void setVignette(float start, float end);
		void setSaturation(float value);
		void setColorScale(float low, float range);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramColorScaleName;
		static CGEConstString paramSaturationName;
		static CGEConstString paramVignetteName;
		static CGEConstString paramAspectRatio;

	private:
		GLfloat m_scaleDark, m_scaleLight, m_saturate;
	};

	class CGELomoWithCurveLinearFilter : public CGELomoWithCurveFilter
	{
	public:
		bool init();
	};

	//////////////////////////////////////////////////////////////////////////

	class CGELomoWithCurveTexFilter : public CGELomoWithCurveFilter
	{
	public:
		virtual bool init();

		virtual void flush();

	protected:
		void initSampler();

	protected:
		GLuint m_curveTexture;
	};

	class CGELomoWithCurveTexLinearFilter : public CGELomoWithCurveTexFilter
	{
	public:
		bool init();
	};

	//////////////////////////////////////////////////////////////////////////

    //TODO: 重写colorscale filter， 避免CPU方法!
	class CGEColorScaleFilter : public CGEImageFilterInterface
	{
	public:
		CGEColorScaleFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}
		~CGEColorScaleFilter() {}

		virtual bool init();

		void setColorScale(float low, float range);
		//Set saturation value to -1.0 ( < 0.0 ) when your shader program did nothing with this value.
		void setSaturation(float value);

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramColorScaleName;
		static CGEConstString paramSaturationName;
	private:
		GLfloat m_scaleDark, m_scaleLight, m_saturate;
	};

	class CGEColorMulFilter : public CGEImageFilterInterface
	{
	public:

		enum MulMode { mulFLT, mulVEC, mulMAT };

		bool initWithMode(MulMode mode);

		void setFLT(float value);
		void setVEC(float r, float g, float b);
		void setMAT(float* mat); //The lenth of "mat" must be at least 9.
	protected:
		static CGEConstString paramMulName;
	};

}


#endif
