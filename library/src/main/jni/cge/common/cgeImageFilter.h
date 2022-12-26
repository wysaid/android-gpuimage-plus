/*﻿
* cgeImageFilter.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*/

#ifndef _CGEIMAGEFILTER_H_
#define _CGEIMAGEFILTER_H_

#include "cgeGLFunctions.h"
#include "cgeShaderFunctions.h"

#ifndef CGE_CURVE_PRECISION
#define CGE_CURVE_PRECISION 256
#endif

namespace CGE
{

	extern CGEConstString g_vshDefault;
	extern CGEConstString g_vshDefaultWithoutTexCoord;
	extern CGEConstString g_vshDrawToScreen;
	extern CGEConstString g_vshDrawToScreenRot90;
	extern CGEConstString g_fshDefault;
	extern CGEConstString g_fshFastAdjust;
	extern CGEConstString g_fshFastAdjustRGB;
	extern CGEConstString g_fshCurveMapNoIntensity;

	extern CGEConstString g_paramFastAdjustArrayName;
	extern CGEConstString g_paramFastAdjustRGBArrayName;
	extern CGEConstString g_paramCurveMapTextureName;


	class CGEImageHandlerInterface;

	class CGEImageFilterInterface;

	class CGEImageFilterInterfaceAbstract
	{
	public:
        CGEImageFilterInterfaceAbstract();
        virtual ~CGEImageFilterInterfaceAbstract();
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID) = 0;

		virtual void setIntensity(float value) {}

		//mutiple effects专有， 若返回为 true， handler在添加filter时会进行拆解。
		virtual bool isWrapper() { return false; }
		virtual std::vector<CGEImageFilterInterface*> getFilters(bool bMove = true) { return std::vector<CGEImageFilterInterface*>(); }
		
	};

	class CGEImageFilterInterface : public CGEImageFilterInterfaceAbstract
	{
	public:
		CGEImageFilterInterface();
		virtual ~CGEImageFilterInterface();

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		//////////////////////////////////////////////////////////////////////////
		bool initShadersFromString(const char* vsh, const char* fsh);
		//bool initShadersFromFile(const char* vshFileName, const char* fshFileName);

		void setAdditionalUniformParameter(UniformParameters* param);
		UniformParameters* getUniformParam() { return m_uniformParam; }

		virtual bool init() { return false; }

		ProgramObject& getProgram() { return m_program; }

		static CGEConstString paramInputImageName;
		static CGEConstString paramPositionIndexName;

	//protected:
		//////////////////////////////////////////////////////////////////////////
		//virtual bool initVertexShaderFromString(const char* vsh);
		//	virtual bool initVertexShaderFromFile(const char* vshFileName);

		//virtual bool initFragmentShaderFromString(const char* fsh);	
		//	virtual bool initFragmentShaderFromFile(const char* fshFileName);

		//virtual bool finishLoadShaders(); //如果单独调用上方函数初始化，请在结束后调用本函数。	

	protected:
		ProgramObject m_program;

		//See the description of "UniformParameters" to know "How to use it".
		UniformParameters* m_uniformParam;
	};

	class CGEFastAdjustFilter : public CGEImageFilterInterface
	{
	public:

		typedef struct CurveData
		{
			float data[3];

			float& operator[](int index)
			{
				return data[index];
			}

			const float& operator[](int index) const
			{
				return data[index];
			}
		}CurveData;

		bool init();

	protected:
		static CGEConstString paramArray;
		void assignCurveArrays();
		void initCurveArrays();

	protected:
		std::vector<CurveData> m_curve;
	};

	class CGEFastAdjustRGBFilter : public CGEImageFilterInterface
	{
	public:

		bool init();

	protected:
		static CGEConstString paramArrayRGB;
		void assignCurveArray();
		void initCurveArray();

	protected:
		std::vector<float> m_curveRGB;
	};

}

#endif
