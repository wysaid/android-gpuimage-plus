/*
* cgeMultipleEffects.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*/

#ifndef _CGEDATAPARSINGENGINE_H_
#define _CGEDATAPARSINGENGINE_H_

#include "cgeMultipleEffects.h"

namespace CGE
{
	class CGEDataParsingEngine
	{
	public:
		static CGEImageFilterInterface* adjustParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* curveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* lomoWithCurveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* lomoParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* blendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* vignetteBlendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* colorScaleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* pixblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* krblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* vignetteParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* selfblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* colorMulParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* selectiveColorParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* blendTileParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* advancedStyleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
		static CGEImageFilterInterface* beautifyParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = NULL);
	};

}
#endif /* _CGEDATAPARSINGENGINE_H_ */
