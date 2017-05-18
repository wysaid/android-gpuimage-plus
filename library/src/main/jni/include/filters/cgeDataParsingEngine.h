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
		static CGEImageFilterInterface* adjustParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* curveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* lomoWithCurveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* lomoParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* blendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* vignetteBlendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* colorScaleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* pixblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* krblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* vignetteParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* selfblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* colorMulParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* selectiveColorParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* blendTileParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* advancedStyleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* beautifyParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* blurParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);
		static CGEImageFilterInterface* dynamicParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter = nullptr);

	};

}
#endif /* _CGEDATAPARSINGENGINE_H_ */
