/*
* cgeMultipleEffects.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeDataParsingEngine.h"
#include "cgeMultipleEffectsCommon.h"
#include "cgeMultipleEffectsSpecial.h"
#include "cgeCardEffects.h"
#include "cgeAdvancedEffects.h"
#include "cgeBlendFilter.h"
#include "cgeFilterBasic.h"

#include <cstring>
#include <cctype>
#include <sstream>

//为了加快处理速度，使用固定大小的buffer来存储Parser所需参数。
//每个method后面的参数长度都不应该超过BUFFER_LEN。
//如果你的Parser所需参数超过此长度，请将BUFFER_LEN增加到合适的长度。
#define BUFFER_LEN 1024
#define BUFFER_LEN_STR "1023"

namespace CGE
{
	extern bool g_isFastFilterImpossible;

	void tableParserHelper(std::vector<CGECurveTexFilter::CurvePoint>& vecPnts, const char* pstr, int n)
	{
		const char* p = pstr;
		int a, b;

		for(int i = 0; i < n;)
		{
			while(i < n && pstr[i] != '\0' && pstr[i] != '(') ++i;
			if(pstr[i] != '(') break;
			p = pstr + i + 1;
			if(sscanf(p, "%d%*c%d", &a, &b) == 2)
			{
				vecPnts.push_back(CGECurveTexFilter::makeCurvePoint(a / 255.0f, b / 255.0f));
			}
			while(i < n && pstr[i] != '\0' && pstr[i] != ')') ++i;
			if(pstr[i] != ')') break;
			++i;
		}
	}

#define PARSER_TABLE_COMMON_FUNC(vec) \
{ \
int n = i; \
for(char c = toupper(pstr[i]); \
c != '\0' && c != 'R' && c != 'G' && c != 'B' && c != '@'; c = toupper(pstr[++i])); \
tableParserHelper(vec, pstr + n, i - n); \
}

	CGEImageFilterInterface* CGEDataParsingEngine::curveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace std;
		vector<CGEMoreCurveFilter::CurvePoint> vecR, vecG, vecB, vecRGB;
		CGEMoreCurveFilter* proc = g_isFastFilterImpossible ? NULL : getMoreCurveTexFilter();

		if(proc == NULL)
		{
			CGE_LOG_INFO("curveParser - Curve With Texture is used!(Not error, everything is ok)\n");
			proc = getMoreCurveTexFilter();

			if(proc == NULL)
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::curveParser Create Curve filter Failed!\n");
				return NULL;
			}
		}		

		for(int i = 0; pstr[i] != '\0' && pstr[i] != '@';)
		{
			switch (pstr[i])
			{
			case 'R': case 'r':				
				if(toupper(pstr[i + 1]) == 'G' && toupper(pstr[i + 2]) == 'B')
				{
					vecRGB.clear();
					//RGB
					i += 3;
					PARSER_TABLE_COMMON_FUNC(vecRGB);
					if(vecRGB.size() < 2)
                    {
                        CGE_LOG_ERROR("Not enough RGB curve points: %s\n", pstr);
                    }
					else
					{
						proc->pushPointsRGB(vecRGB.data(), vecRGB.size());
					}
				}
				else
				{
					vecR.clear();
					++i;
					int n = i;
					for(char c = toupper(pstr[i]); 
						c != '\0' && c != 'R' && c != 'G' && c != 'B' && c != '@'; c = toupper(pstr[i])) ++i;
						tableParserHelper(vecR, pstr + n, i - n);
					if(vecR.size() < 2)
                    {
                        CGE_LOG_ERROR("Not enough R curve points: %s\n", pstr);
                    }
					else
					{
						proc->pushPointsR(vecR.data(), vecR.size());
					}
				}
				break;
			case 'G': case 'g':
				vecG.clear();
				++i;
				PARSER_TABLE_COMMON_FUNC(vecG);
                if(vecG.size() < 2)
                {
                    CGE_LOG_ERROR("Not enough G curve points: %s\n", pstr);
                }
				else
				{
					proc->pushPointsG(vecG.data(), vecG.size());
				}				
				break;
			case 'B': case 'b':
				vecB.clear();
				++i;
				PARSER_TABLE_COMMON_FUNC(vecB);
				if(vecB.size() < 2)
                {
                    CGE_LOG_ERROR("Not enough B curve points: %s\n", pstr);
                }
				else
				{
					proc->pushPointsB(vecB.data(), vecB.size());
				}				
				break;
			default:				
				++i;
				break;
			}
		}
		if(vecRGB.empty() && vecR.empty() && vecG.empty() && vecB.empty())
		{
			CGE_LOG_ERROR("curveParser - Empty Curve!!\n");
			return NULL;
		}

		proc->flush();
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::lomoWithCurveParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace std;
		using namespace CGE;
		
		float vignetteStart, vignetteEnd, colorScaleLow, colorScaleRange, saturation;
		int isLinear = 0;
		while(*pstr != '\0' && !isdigit(*pstr)) ++pstr;
		if(sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f%*c%d",
			&vignetteStart, &vignetteEnd, &colorScaleLow, &colorScaleRange, &saturation, &isLinear) < 5)
		{
			return NULL;
		}

		CGELomoWithCurveFilter* proc = g_isFastFilterImpossible ? NULL : (isLinear ? new CGELomoWithCurveLinearFilter : new CGELomoWithCurveFilter);

		if(proc == NULL || !proc->init())
		{
			delete proc;
			proc = isLinear ? new CGELomoWithCurveTexLinearFilter : new CGELomoWithCurveTexFilter;
			if(!proc->init())
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::lomoWithCurveParser Create filter Failed!\n");
				return NULL;
			}
			CGE_LOG_INFO("lomoWithCurveParser - Curve With Texture is used!(Not error, everything is ok)\n");
		}
		proc->setVignette(vignetteStart, vignetteEnd);
		proc->setColorScale(colorScaleLow, colorScaleRange);
		proc->setSaturation(saturation);

		vector<CGECurveInterface::CurvePoint> vecR, vecG, vecB, vecRGB;
		for(int i = 0; pstr[i] != '\0' && pstr[i] != '@';)
		{
			switch (pstr[i])
			{
			case 'R': case 'r':
				if(toupper(pstr[i + 1]) == 'G' && toupper(pstr[i + 2]) == 'B')
				{
					//RGB
					i += 3;
					PARSER_TABLE_COMMON_FUNC(vecRGB);
				}
				else
				{
					++i;
					int n = i;
					for(char c = toupper(pstr[i]); 
						c != '\0' && c != 'R' && c != 'G' && c != 'B' && c != '@'; c = toupper(pstr[i])) ++i;
						tableParserHelper(vecR, pstr + n, i - n);
				}
				break;
			case 'G': case 'g':
				++i;
				PARSER_TABLE_COMMON_FUNC(vecG);
				break;
			case 'B': case 'b':
				++i;
				PARSER_TABLE_COMMON_FUNC(vecB);
				break;
			default:

				++i;
				break;
			}
		}

		if(vecRGB.empty() && vecR.empty() && vecG.empty() && vecB.empty())
		{
			CGE_LOG_ERROR("lomoParser - Warning: Empty Curve!!\n");
		}

		proc->pushPointsRGB(vecRGB.data(), (GLuint)vecRGB.size());
		proc->pushPoints(vecR.data(), (GLuint)vecR.size(),
						vecG.data(), (GLuint)vecG.size(),
						vecB.data(), (GLuint)vecB.size());
		proc->flush();
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::lomoParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace std;
		using CGE::CGELomoLinearFilter;
		using CGE::CGELomoFilter;

		float vignetteStart, vignetteEnd, colorScaleLow, colorScaleRange, saturation;
		int isLinear = 0;
		while(*pstr != '\0' && !isdigit(*pstr)) ++pstr;
		if(sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f%*c%d", 
			&vignetteStart, &vignetteEnd, &colorScaleLow, &colorScaleRange, &saturation, &isLinear) < 5)
		{
			return NULL;
		}
		CGELomoFilter* proc;
		if(isLinear)
			proc = new CGELomoLinearFilter;
		else proc = new CGELomoFilter;
		proc->init();
		proc->setVignette(vignetteStart, vignetteEnd);
		proc->setColorScale(colorScaleLow, colorScaleRange);
		proc->setSaturation(saturation);

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

#define ADJUSTHELP_COMMON_FUNC(str, procName, setFunc) \
do{\
	float intensity;\
	if(sscanf(str, "%f", &intensity) != 1)\
	{\
		CGE_LOG_ERROR("Invalid Parameters: %s\n", str);\
		return NULL;\
	}\
	procName* bp = new procName();\
	if(!bp->init())\
	{\
		delete bp;\
	}\
	else \
	{\
		proc = bp;\
		bp->setFunc(intensity);\
	}\
}while(0)

#define ADJUSTHELP_COMMON_FUNC2(str, procName, setFunc1, setFunc2) \
do{\
	float intensity1, intensity2;\
	if(sscanf(str, "%f%*c%f", &intensity1, &intensity2) != 2)\
	{\
		CGE_LOG_ERROR("Invalid Parameters: %s\n", str);\
		return NULL;\
	}\
	procName* bp = new procName();\
	if(!bp->init())\
	{\
		delete bp;\
	}\
	else \
	{\
		proc = bp;\
		bp->setFunc1(intensity1);\
		bp->setFunc2(intensity2);\
	}\
}while(0)

#define ADJUSTHELP_COMMON_FUNC3(str, procName, setFunc1, setFunc2, setFunc3) \
do{\
	float intensity1, intensity2, intensity3;\
	if(sscanf(str, "%f%*c%f%*c%f", &intensity1, &intensity2, &intensity3) != 3)\
	{\
		CGE_LOG_ERROR("Invalid Parameters: %s\n", str);\
		return NULL;\
	}\
	procName* bp = new procName();\
	if(!bp->init())\
	{\
		delete bp;\
	}\
	else \
	{\
		proc = bp;\
		bp->setFunc1(intensity1);\
		bp->setFunc2(intensity2);\
		bp->setFunc3(intensity3);\
	}\
}while(0)

#define ADJUSTHELP_COMMON_FUNC_ARG2(str, procName, setFunc) \
do{\
	float intensity1, intensity2;\
	if(sscanf(str, "%f%*c%f", &intensity1, &intensity2) != 2)\
	{\
		CGE_LOG_ERROR("Invalid Parameters: %s\n", str);\
		return NULL;\
	}\
	procName* bp = new procName();\
	if(!bp->init())\
	{\
		delete bp;\
	}\
	else \
	{\
		proc = bp;\
		bp->setFunc(intensity1, intensity2);\
	}\
}while(0)

	CGEImageFilterInterface* CGEDataParsingEngine::adjustParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		while(*pstr != '\0' && (*pstr == ' ' || *pstr == '\t')) ++pstr;
		CGEImageFilterInterface* proc = NULL;

		//2015-1-29 隐患fix, 将下一条指令直接取出再进行判断
		char buffer[128], *pBuffer = buffer;

		while(*pstr != '\0' && !isspace(*pstr) && (pBuffer - buffer) < sizeof(buffer))
		{
			*pBuffer++ = *pstr++;
		}

		*pBuffer = '\0';

		//Hardcode for all basic adjusts.
		if(strcmp(buffer, "brightness") == 0)
		{
			float intensity;
			if(sscanf(pstr, "%f", &intensity) != 1)
				return NULL;
			CGEBrightnessFastFilter* bfp = g_isFastFilterImpossible ? NULL : getBrightnessFastFilter();
			CGEBrightnessFilter* bp = (bfp == NULL) ? getBrightnessFilter() : NULL;

			if(bfp != NULL)
			{
				bfp->setIntensity(intensity);
				proc = bfp;
			}
			else if(bp != NULL)
			{
				bp->setIntensity(intensity);
				proc = bp;
			}
			else
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Brightness filter Failed\n");
				return NULL;
			}
		}
		else if(strcmp(buffer, "contrast") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGEContrastFilter, setIntensity);
		}
		else if(strcmp(buffer, "saturation") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGESaturationFilter, setIntensity);
		}
		else if(strcmp(buffer, "sharpen") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGESharpenBlurSimpleBetterFilter, setSharpenIntensity);
		}
		else if(strcmp(buffer, "blur") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGESharpenBlurSimpleBetterFilter, setBlurIntensity);
		}
		else if(strcmp(buffer, "whitebalance") == 0)
		{
			float temp, tint;
			if(sscanf(pstr, "%f%*c%f", &temp, &tint) != 2)
			{
				CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGEWhiteBalanceFastFilter* wfp = g_isFastFilterImpossible ? NULL : getWhiteBalanceFastFilter();
			CGEWhiteBalanceFilter* wp = (wfp == NULL) ? getWhiteBalanceFilter() : NULL;
			if(wfp != NULL)
			{
				wfp->setTempAndTint(temp, tint);
				proc = wfp;
			}
			else if(wp != NULL)
			{
				wp->setTemperature(temp);
				wp->setTint(tint);
				proc = wp;
			}
			else 
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create WhiteBalance filter Failed\n");
				return NULL;
			}
		}
		else if(strcmp(buffer, "monochrome") == 0)
		{
			float arg[6];
			if(sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f%*c%f", 
				arg, arg+1, arg+2, arg+3, arg+4, arg+5) != 6)
			{
				CGE_LOG_ERROR("adjust hsv - Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGEMonochromeFilter* monoProc = new CGEMonochromeFilter;
			if(!monoProc->init())
			{
				delete monoProc;
				return NULL;
			}
			else
			{
				proc = monoProc;
				monoProc->setRed(arg[0]);
				monoProc->setGreen(arg[1]);
				monoProc->setBlue(arg[2]);
				monoProc->setCyan(arg[3]);
				monoProc->setMagenta(arg[4]);
				monoProc->setYellow(arg[5]);
			}
		}
		else if(strcmp(buffer, "shl") == 0 || strcmp(buffer, "shadowhighlight") == 0)
		{
			float shadow, highlight;
			if(sscanf(pstr, "%f%*c%f", &shadow, &highlight) != 2)
			{
				CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGEShadowHighlightFastFilter* shfp = g_isFastFilterImpossible ? NULL : getShadowHighlightFastFilter();
			CGEShadowHighlightFilter* shp = (shfp == NULL) ? getShadowHighlightFilter() : NULL;
			if(shfp != NULL)
			{
				shfp->setShadowAndHighlight(shadow, highlight);
				proc = shfp;
			}
			else if(shp != NULL)
			{
				shp->setShadow(shadow);
				shp->setHighlight(highlight);
				proc = shp;
			}
			else 
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create ShadowHighlight filter Failed\n");
				return NULL;
			}
		}
		else if(strcmp(buffer, "hsv") == 0)
		{
			float arg[6];
			if(sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f%*c%f", 
				arg, arg+1, arg+2, arg+3, arg+4, arg+5) != 6)
			{
				CGE_LOG_ERROR("adjust hsv - Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGESaturationHSVFilter* hsvProc = new CGESaturationHSVFilter;
			if(!hsvProc->init())
			{
				delete hsvProc;
				return NULL;
			}
			else
			{
				proc = hsvProc;
				hsvProc->setAdjustColors(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
			}
		}
		else if(strcmp(buffer, "hsl") == 0)
		{
			float h, s, l;
			if(sscanf(pstr, "%f%*c%f%*c%f", &h, &s, &l) != 3)
			{
				CGE_LOG_ERROR("adjust hsl - Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGESaturationHSLFilter* hslProc = getSaturationHSLFilter();
			proc = hslProc;
			if(hslProc != NULL)
			{
				hslProc->setHue(h);
				hslProc->setSaturation(s);
				hslProc->setLum(l);
			}
		}
		else if(strcmp(buffer, "level") == 0)
		{
			float dark, light, gamma;
			if(sscanf(pstr, "%f%*c%f%*c%f", &dark, &light, &gamma) != 3)
			{
				CGE_LOG_ERROR("adjust color level - Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGEColorLevelFilter* levelProc = getColorLevelFilter();
			proc = levelProc;
			if(levelProc != NULL)
			{
				levelProc->setLevel(dark, light);
				levelProc->setGamma(gamma);
			}
		}
		else if(strcmp(buffer, "exposure") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGEExposureFilter, setIntensity);
		}
		else if(strcmp(buffer, "hue") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGEHueAdjustFilter, setHue);
		}
		else if(strcmp(buffer, "colorbalance") == 0)
		{
			float red, green, blue;
			if(sscanf(pstr, "%f%*c%f%*c%f", &red, &green, &blue) != 3)
			{
				CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
				return NULL;
			}

			CGEColorBalanceFilter* filter = getColorBalanceFilter();

			if(filter != NULL)
			{
				proc = filter;
				filter->setRedShift(red);
				filter->setGreenShift(green);
				filter->setBlueShift(blue);
			}
			else
			{
				CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create ColorBalance filter Failed\n");
				return NULL;
			}
		}
		else
		{
			CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::blendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char modeName[32], textureName[128];
		int intensity;

		if(sscanf(pstr, "%31s%127s%d", modeName, textureName, &intensity) != 3)
		{
			CGE_LOG_ERROR("blendParser - Invalid Param: %s\n", pstr);
			return NULL;
		}
		CGEBlendWithResourceFilter* proc = new CGEBlendWithResourceFilter;		

		if(!proc->initWithMode(modeName))
		{
			delete proc;
			return NULL;			
		}

		proc->setLoadFunction(fatherFilter->getLoadFunc(), fatherFilter->getLoadParam(), fatherFilter->getUnloadFunc(), fatherFilter->getUnloadParam());
		proc->setIntensity(intensity / 100.0f);

		if(!proc->loadResources(textureName))
		{
			CGE_LOG_ERROR("blend - %s : loadResources failed: %s\n", modeName, textureName);
			delete proc;
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::vignetteBlendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char blendMethod[BUFFER_LEN];
		float color[4], intensity;
		float low, range, centerX, centerY;
		int kind = 0;

		if(sscanf(pstr, "%" BUFFER_LEN_STR "s%f%f%f%f%f%f%f%f%f%d", blendMethod, color, color + 1, color + 2, color + 3, &intensity, &low, &range, &centerX, &centerY, &kind) < 10)
		{
			CGE_LOG_ERROR("vignetteBlendParser - Invalid parameters: %s\n", pstr);
			return NULL;
		}

		CGEBlendVignetteFilter* proc = NULL;
		switch(kind)
		{
		case 0:
			proc = new CGEBlendVignetteNoAlphaFilter;
			break;
		case 1:
			proc = new CGEBlendVignetteFilter;
			break;
		case 2:
			proc = new CGEBlendVignette2NoAlphaFilter;
			break;
		case 3:
			proc = new CGEBlendVignette2Filter;
			break;
		default:
			CGE_LOG_ERROR("vignetteBlendParser - Invalid vignette kind %d", kind);
			return NULL;
		}

		if(!CGEBlendInterface::initWithModeName(blendMethod, proc))
		{
			delete proc;
			return NULL;
		}

		proc->setVignette(low, range);
		proc->setVignetteCenter(centerX, centerY);

		if(color[3] > 1.00001f) //判断值域范围为0~1还是0~255并进行相应处理。
		{
			color[0] /= 255.0f;
			color[1] /= 255.0f;
			color[2] /= 255.0f;
			color[3] /= 255.0f;
		}
		proc->setBlendColor(color[0], color[1], color[2], color[3]);
		proc->setIntensity(intensity / 100.0f);

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	//////////////////////////////////////////////////////////////////////////

	CGEImageFilterInterface* CGEDataParsingEngine::colorScaleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		float low, range, sat;
		if(sscanf(pstr, "%f%*c%f%*c%f", &low, &range, &sat) != 3)
		{
			CGE_LOG_ERROR("colorScaleParser - Invalid Parameters: %s\n", pstr);
			return NULL;
		}
		CGEColorScaleFilter* proc = new CGEColorScaleFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		proc->setColorScale(low, range);
		proc->setSaturation(sat);
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

#define SPECIALPARSER_COMMON_FUNC(p, procType) \
do{p = new procType;\
if(!p->init())\
{\
	delete p;\
	return NULL;\
}}while(0)

#define SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(p, procType, fatherFilter) \
do{procType* q = new procType;\
if(!q->initWithinCommonFilter(fatherFilter))\
{\
	delete q;\
	return NULL;\
}p=q;}while(0)


	CGEImageFilterInterface* CGEDataParsingEngine::specialParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		int index;
		if(sscanf(pstr, "%d", &index) != 1)
		{
			CGE_LOG_ERROR("specialParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		CGEImageFilterInterface* proc = NULL;

		switch(index)
		{
		case 16:
			{
				proc = new CGESpecialFilterE16;
				if(!proc->init())
				{
					delete proc;
					return NULL;
				}
			}
			break;
		case 21:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE21);
			break;
		case 28:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE28);
			break;
		case 29:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE29);
			break;
		case 30:
			{
				CGESpecialFilterE30To32* p = new CGESpecialFilterE30To32;
				if(!p->init())
				{
					delete p;
					return NULL;
				}
				proc = p;
				p->setColorScale(0.45f, 0.45f);
			}
			break;
		case 31:
			{
				CGESpecialFilterE30To32* p = new CGESpecialFilterE30To32;
				if(!p->init())
				{
					delete p;
					return NULL;
				}
				proc = p;
				p->setColorScale(0.01f, 0.25f);
			}
			break;
		case 32:
			{
				CGESpecialFilterE30To32* p = new CGESpecialFilterE30To32;
				if(!p->init())
				{
					delete p;
					return NULL;
				}
				proc = p;
				p->setColorScale(0.25f, 0.01f);
			}
			break;
		case 39:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE39, fatherFilter);
			break;

		case 73:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE73, fatherFilter);
			break;
		case 74:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE74, fatherFilter);
			break;
		case 75:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE75, fatherFilter);
			break;
		case 76:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE76, fatherFilter);
			break;
		case 77:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE77, fatherFilter);
			break;
		case 78:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE78, fatherFilter);
			break;
		case 79:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE79, fatherFilter);
			break;
		case 80:
			SPECIALPARSER_COMMON_FUNC_WITHfatherFilter(proc, CGESpecialFilterE80, fatherFilter);
			break;

		case 87:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE87);
			break;
		case 88:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE88);
			break;
		case 89:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE89);
			break;
		case 90:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE90);
			break;
		case 91:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE91);
			break;
		case 92:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE92);
			break;
		case 96:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE96);
			break;
		case 99:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE99);
			break;
		case 118:
			SPECIALPARSER_COMMON_FUNC(proc, CGESpecialFilterE118);
			break;
		default:
			CGE_LOG_ERROR("specialParser - unresolved index: %d\n", index);
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::pixblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char blendMethod[BUFFER_LEN];
		float color[4], intensity;
		
		if(sscanf(pstr, "%" BUFFER_LEN_STR "s%f%f%f%f%f", blendMethod, color, color + 1, color + 2, color + 3, &intensity) != 6)
		{
			CGE_LOG_ERROR("pixblendParser - Invalid parameters: %s\n", pstr);
			return NULL;
		}

		CGEPixblendFilter* proc = new CGEPixblendFilter;
		if(!proc->initWithMode(blendMethod))
		{
			delete proc;
			return NULL;
		}

		if(color[3] > 1.00001f) //判断值域范围为0~1还是0~255并进行相应处理。
		{
			color[0] /= 255.0f;
			color[1] /= 255.0f;
			color[2] /= 255.0f;
			color[3] /= 255.0f;
		}
		proc->setBlendColor(color[0], color[1], color[2], color[3]);
		proc->setIntensity(intensity / 100.0f);
	
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::krblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char modeName[32], textureName[128];
		int intensity;
		if(sscanf(pstr, "%31s%127s%d", modeName, textureName, &intensity) != 3)
		{
			CGE_LOG_ERROR("krblendParser - Invalid Param: %s\n", pstr);
			return NULL;
		}		

		CGEBlendKeepRatioFilter* proc = new CGEBlendKeepRatioFilter;
		if(!proc->initWithMode(modeName))
		{
			delete proc;
			return NULL;
		}

		proc->setIntensity(intensity / 100.0f);
		proc->setLoadFunction(fatherFilter->getLoadFunc(), fatherFilter->getLoadParam(), fatherFilter->getUnloadFunc(), fatherFilter->getUnloadParam());
		if(!proc->loadResources(textureName))
		{
			CGE_LOG_ERROR("blend - %s : loadResources failed: %s\n", modeName, textureName);
			delete proc;
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::vignetteParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		float low, range, centerX, centerY;
		int n = sscanf(pstr, "%f%*c%f%*c%f%*c%f", &low, &range, &centerX, &centerY);
		if(n < 2)
		{
			CGE_LOG_ERROR("vignetteParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		CGEVignetteFilter* proc = new CGEVignetteFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}

		proc->setVignette(low, range);
		if(n == 4) proc->setVignetteCenter(centerX, centerY);
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::selfblendParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char modeName[32];
		int intensity;
		if(sscanf(pstr, "%31s%d", modeName, &intensity) != 2)
		{
			CGE_LOG_ERROR("selfblendParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		CGEBlendWithSelfFilter* proc = new CGEBlendWithSelfFilter;
		if(!proc->initWithMode(modeName))
		{
			delete proc;
			return NULL;
		}
		proc->setIntensity(intensity / 100.0f);
		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::colorMulParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		using namespace CGE;
		char funcName[32] = "";
		if(sscanf(pstr,"%31s", funcName) != 1)
		{
			CGE_LOG_ERROR("colorMulParser - Invalid Param: %s", pstr);
			return NULL;
		}

		CGEColorMulFilter* proc = NULL;
		if(strncmp(funcName, "flt", 3) == 0)
		{
			float value;
			if(sscanf(pstr, "%*s%f", &value) != 1)
			{
				CGE_LOG_ERROR("colorMulParser - flt - Invalid Param:%s\n", pstr);
				return NULL;
			}
			proc = new CGEColorMulFilter;
			proc->initWithMode(CGEColorMulFilter::mulFLT);
			proc->setFLT(value);
		}
		else if(strncmp(funcName, "vec", 3) == 0)
		{
			float r, g, b;
			if(sscanf(pstr, "%*s%f%*c%f%*c%f", &r, &g, &b) != 3)
			{
				CGE_LOG_ERROR("colorMulParser - vec - Invalid Param:%s\n", pstr);
				return NULL;
			}
			proc = new CGEColorMulFilter;
			proc->initWithMode(CGEColorMulFilter::mulVEC);
			proc->setVEC(r, g, b);
		}
		else if(strncmp(funcName, "mat", 3) == 0)
		{
			float mat[9];
			if(sscanf(pstr, "%*s%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f",
				mat, mat + 1, mat + 2, mat + 3, mat + 4, mat + 5, mat + 6, mat + 7, mat + 8) != 9)
			{
				CGE_LOG_ERROR("colorMulParser - mat - Invalid Param:%s\n", pstr);
				return NULL;
			}
			proc = new CGEColorMulFilter;
			proc->initWithMode(CGEColorMulFilter::mulMAT);
			proc->setMAT(mat);
		}
		else
		{
			CGE_LOG_ERROR("colorMulParser - Invalid Param:%s\n", pstr);
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::selectiveColorParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		CGESelectiveColorFilter* proc = getSelectiveColorFilter();
		if(proc == NULL)
		{
			CGE_LOG_ERROR("selectiveColorParser - init processor failed!\n");
			return NULL;
		}

		while(pstr != NULL && *pstr != '\0' && *pstr != '@')
		{
			char funcName[32];
			float cyan, magenta, yellow, key;
			while(*pstr != '\0' && (isspace(*pstr) || *pstr == ',')) ++pstr;
			if(*pstr == '\0' || *pstr == '@')
				break;
			if(sscanf(pstr, "%31[^( \t\n]%*[^-0-9.]%f%*c%f%*c%f%*c%f", funcName, &cyan, &magenta, &yellow, &key) != 5)
			{
				CGE_LOG_ERROR("selectiveColorParser - Invalid Param %s!\n", pstr);
				break;
			}

			while(*pstr != '\0' && *pstr++ != ')') ;

			if(fabsf(cyan) > 1.0f || fabsf(magenta) > 1.0f || fabsf(yellow) > 1.0f || fabsf(key) > 1.0f)
			{
				cyan /= 100.0f;
				magenta /= 100.0f;
				yellow /= 100.0f;
				key /= 100.0f;
			}

			if(strcmp(funcName, "red") == 0)
			{
				proc->setRed(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "green") == 0)
			{
				proc->setGreen(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "blue") == 0)
			{
				proc->setBlue(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "cyan") == 0)
			{
				proc->setCyan(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "magenta") == 0)
			{
				proc->setMagenta(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "yellow") == 0)
			{
				proc->setYellow(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "white") == 0)
			{
				proc->setWhite(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "gray") == 0)
			{
				proc->setGray(cyan, magenta, yellow, key);
			}
			else if(strcmp(funcName, "black") == 0)
			{
				proc->setBlack(cyan, magenta, yellow, key);
			}
			else
			{
				CGE_LOG_ERROR("Unknown funcName: %s!\n", funcName);
			}
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::blendTileParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		char modeName[32], textureName[128];
		int intensity;

		if(sscanf(pstr, "%31s%127s%d", modeName, textureName, &intensity) != 3)
		{
			CGE_LOG_ERROR("blendTileParser - Invalid Param: %s\n", pstr);
			return NULL;
		}
		CGEBlendTileFilter* proc = new CGEBlendTileFilter;		

		if(!proc->initWithMode(modeName))
		{
			delete proc;
			return NULL;			
		}

		proc->setLoadFunction(fatherFilter->getLoadFunc(), fatherFilter->getLoadParam(), fatherFilter->getUnloadFunc(), fatherFilter->getUnloadParam());
		proc->setIntensity(intensity / 100.0f);

		if(!proc->loadResources(textureName))
		{
			CGE_LOG_ERROR("blend - %s : loadResources failed: %s\n", modeName, textureName);
			delete proc;
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::advancedStyleParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		while(*pstr != '\0' && (*pstr == ' ' || *pstr == '\t')) ++pstr;
		CGEImageFilterInterface* proc = NULL;

		//2015-1-29 隐患fix, 将下一条指令直接取出再进行判断
		char buffer[128], *pBuffer = buffer;

		while(*pstr != '\0' && !isspace(*pstr) && (pBuffer - buffer) < sizeof(buffer))
		{
			*pBuffer++ = *pstr++;
		}

		*pBuffer = '\0';

		//Hardcode for all basic adjusts.
		if(strcmp(buffer, "crosshatch") == 0)
		{
			ADJUSTHELP_COMMON_FUNC2(pstr, CGECrosshatchFilter, setCrosshatchSpacing, setLineWidth);
		}
		else if(strcmp(buffer, "edge") == 0)
		{
			ADJUSTHELP_COMMON_FUNC2(pstr, CGEEdgeSobelFilter, setIntensity, setStride);
		}
		else if(strcmp(buffer, "emboss") == 0)
		{
			ADJUSTHELP_COMMON_FUNC3(pstr, CGEEmbossFilter, setIntensity, setStride, setAngle);
		}
		else if(strcmp(buffer, "halftone") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGEHalftoneFilter, setDotSize);
		}
		else if(strcmp(buffer, "haze") == 0)
		{
			float dis, slope, r, g, b;
			if(sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f", &dis, &slope, &r, &g, &b) != 5)
			{
				CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
				return NULL;
			}
			CGEHazeFilter* filter = getHazeFilter();
			if(filter != NULL)
			{
				proc = filter;
				filter->setDistance(dis);
				filter->setSlope(slope);
				filter->setHazeColor(r, g, b);
			}
		}
		else if(strcmp(buffer, "polkadot") == 0)
		{
			ADJUSTHELP_COMMON_FUNC(pstr, CGEPolkaDotFilter, setDotScaling);
		}
		else
		{
			CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	CGEImageFilterInterface* CGEDataParsingEngine::beautifyParser(const char* pstr, CGEMutipleEffectFilter* fatherFilter)
	{
		while(*pstr != '\0' && (*pstr == ' ' || *pstr == '\t')) ++pstr;
		CGEImageFilterInterface* proc = NULL;

		char buffer[128], *pBuffer = buffer;

		while(*pstr != '\0' && !isspace(*pstr) && (pBuffer - buffer) < sizeof(buffer))
		{
			*pBuffer++ = *pstr++;
		}

		*pBuffer = '\0';

		//Hardcode for all basic adjusts.
		if(strcmp(buffer, "bilateral") == 0)
		{
			float blurScale, disFactor;
			int repeatTimes = 1;
			if(sscanf(pstr, "%f%*c%f%*c%d", &blurScale, &disFactor, &repeatTimes) < 2)
			{
				CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
				return NULL;
			}

			CGEBilateralWrapperFilter* filter = new CGEBilateralWrapperFilter;
			if(filter->init())
			{
				proc = filter;
				filter->setBlurScale(blurScale);
				filter->setDistanceNormalizationFactor(disFactor);
				filter->setRepeatTimes(repeatTimes);
			}
			
		}
		
		else
		{
			CGE_LOG_ERROR("Invalid Parameters: %s\n", pstr);
			return NULL;
		}

		if(fatherFilter != NULL) fatherFilter->addFilter(proc);
		return proc;
	}

	//////////////////////////////////////////////////////////////////////////

	CGEBorderFilterInterface* CGEDataParsingEngine::borderSParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		if(sscanf(pstr, "%s", srcName) != 1)
		{
			CGE_LOG_ERROR("borderSParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderSParser - Load resource %s failed\n", srcName);
			return NULL;
		}

		CGEBorderSFilter* proc = new CGEBorderSFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		proc->setSamplerID(texID);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderMParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float w = 0.0f, h = 0.0f;
		float blocks[32];
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 32; ++i)
			ss >> blocks[i];
		if(ss.fail() || ss.bad() || w == 0.0f || h == 0.0f)
		{
			CGE_LOG_ERROR("borderMParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderSParser - Load resource %s failed\n", srcName);
			return NULL;
		}

		CGEBorderMFilter* proc = new CGEBorderMFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		for(int i = 0; i != 8; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		proc->setBlocks(blocks, float(w) / h);
		proc->setSamplerID(texID);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCardParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float x, y, w, h, align;
		if(sscanf(pstr, "%255s%f%*c%f%*c%f%*c%f%*c%f", srcName, &x, &y, &w, &h, &align) != 6)
		{
			CGE_LOG_ERROR("borderCardParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCardParser - Load resource %s failed!\n", srcName);
			return NULL;
		}

		CGEBorderCardFilter* proc = new CGEBorderCardFilter;
		if(!proc->init())
		{
			glDeleteTextures(1, &texID);
			delete proc;
			return NULL;
		}
		proc->setPictureZone(x, y, w, h);
		proc->setAlign(align);
		proc->setSamplerID(texID);
		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverSKRParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		char srcName[256];
		float w, h;
		if(sscanf(pstr, "%s%f%*c%f", srcName, &w, &h) != 3)
		{
			CGE_LOG_ERROR("borderCoverSKRParser - Invalid Param %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverSKRParser - Load resource %s failed!\n", srcName);
			return NULL;
		}

		CGEBorderCoverSKRFilter* proc = new CGEBorderCoverSKRFilter;
		if(!proc->init())
		{
			glDeleteTextures(1, &texID);
			delete proc;
			return NULL;
		}
		proc->setSamplerID(texID);

		UniformParameters* param = proc->getUniformParam();
		if(param == NULL) param = new UniformParameters;
		
		param->requireStepsRatio(CGEBorderCoverSKRFilter::paramStepsRatioName, w / h);
		proc->setAdditionalUniformParameter(param);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverCornerParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float w = 0.0f, h = 0.0f;
		float blocks[16];
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 16; ++i)
			ss >> blocks[i];
		if(ss.fail() || ss.bad() || w == 0.0f || h == 0.0f)
		{
			CGE_LOG_ERROR("borderCoverCornerParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderSParser - Load resource %s failed\n", srcName);
			return NULL;
		}
		CGEBorderCoverCornersFilter* proc = new CGEBorderCoverCornersFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		for(int i = 0; i != 4; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		proc->setBlocks(blocks);
		proc->setSamplerID(texID);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverUDTParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		char srcName[256], align[128];
		float w, h;
		float blocks[12];
		float texArea[2];
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 12; ++i)
		{
			ss >> blocks[i];
		}
		ss >> align;
		ss >> texArea[0] >> texArea[1];
		if(ss.fail() || ss.bad() || w == 0.0f || h == 0.0f)
		{
			CGE_LOG_ERROR("borderCoverUDTParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverUDTParser - Load resource %s failed!\n", srcName);
			return NULL;
		}

		CGEBorderCoverUDTFilter* proc = new CGEBorderCoverUDTFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}

		for(int i = 0; i != 3; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		proc->setBlocks(blocks, w, h);
		proc->setTexAlign(align, texArea[0], texArea[1]);
		proc->setSamplerID(texID);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverSWithTexParser(const char* pstr, CGEBorderFilter* fatherFilter, bool keepRatio)
	{
		char srcName[256], align[128];
		float w, h;
		float blocks[8];
		float texArea[4] = {0.0f};
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 8; ++i)
		{
			ss >> blocks[i];
		}
		ss >> align;
		if(align[0] == 'X') ss >> texArea[0];
		if(align[1] == 'X') ss >> texArea[1];
		ss >> texArea[2] >> texArea[3];
		if(ss.fail() || ss.bad() || w == 0.0f || h == 0.0f)
		{
			CGE_LOG_ERROR("borderCoverUDTParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		float borderSize = 0.0f;
		ss >> borderSize;

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverUDTParser - Load resource %s failed!\n", srcName);
			return NULL;
		}

		CGEBorderSWithTexFilter* proc;
		if(keepRatio) proc = new CGEBorderSWithTexKRFilter;
		else proc = new CGEBorderSWithTexFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}

		for(int i = 0; i != 2; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		proc->setBlocks(blocks, w, h);
		proc->setTexAlign(align, texArea[0], texArea[1], texArea[2], texArea[3]);
		proc->setSamplerID(texID);
		proc->setBorderSize(borderSize);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverMWithTexParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256], align[128];
		float w = 0.0f, h = 0.0f;
		float blocks[32], texBlock[4], texArea[4] = {0.0f};
		float borderSizeX = 0.1f, borderSizeY = 0.1f;
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 32; ++i)
			ss >> blocks[i];
		ss >> texBlock[0] >> texBlock[1] >> texBlock[2] >> texBlock[3];
		ss >> align;
		if(align[0] == 'X') ss >> texArea[0];
		if(align[1] == 'X') ss >> texArea[1];
		ss >> texArea[2] >> texArea[3];

		if(ss.fail() || ss.bad() || w == 0 || h == 0)
		{
			CGE_LOG_ERROR("borderCoverMWithTexParser - Invalid Param: %s\n", pstr);
			return NULL;
		}

		ss >> borderSizeX >> borderSizeY;

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverMWithTexParser - Load resource %s failed\n", srcName);
			return NULL;
		}

		CGEBorderMWithTexFilter* proc = new CGEBorderMWithTexFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		for(int i = 0; i != 8; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		texBlock[0] /= w;
		texBlock[1] /= h;
		texBlock[2] /= w;
		texBlock[3] /= h;
		proc->setBlocks(blocks);
		proc->setSamplerID(texID);
		proc->setBorderSize(borderSizeX, borderSizeY);
		proc->setTexBlock(texBlock, w /h);
		proc->setTexAlign(align, texArea[0], texArea[1], texArea[2], texArea[3]);
		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverRTAndLBParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256], align[128];
		float w = 0.0f, h = 0.0f;
		float blocks[12], texArea[4] = {0.0f};
		float borderSize = 0.12f;
		std::stringstream ss(pstr);
		ss >> srcName >> w >> h;
		for(int i = 0; i != 12; ++i)
			ss >> blocks[i];
		ss >> align;
		if(align[0] == 'X') ss >> texArea[0];
		if(align[1] == 'X') ss >> texArea[1];
		ss >> texArea[2] >> texArea[3];
		if(ss.fail() || ss.bad() || w == 0.0f || h == 0.0f)
		{
			CGE_LOG_ERROR("borderCoverCornerParser - Invalid Param: %s\n", pstr);
			return NULL;
		}
		ss >> borderSize;

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderSParser - Load resource %s failed\n", srcName);
			return NULL;
		}
		CGEBorderCoverRTAndLBWithTexFilter* proc = new CGEBorderCoverRTAndLBWithTexFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		for(int i = 0; i != 3; ++i)
		{
			const int j = i * 4;
			blocks[j] /= w;
			blocks[j + 1] /= h;
			blocks[j + 2] /= w;
			blocks[j + 3] /= h;
		}
		proc->setBlocks(blocks);
		proc->setSamplerID(texID);
		proc->setBorderSize(borderSize);
		proc->setTexAlign(align, texArea[0], texArea[1], texArea[2], texArea[3]);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverBottomKRParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float w, h;
		if(sscanf(pstr, "%256s%f%*c%f", srcName, &w, &h) != 3)
		{
			CGE_LOG_ERROR("CGEDataParsingEngine::borderCoverBottomKRParser - Invalid Param %s\n", pstr);
			return NULL;
		}
		GLuint texID = fatherFilter->loadResourceToTexture(srcName);
		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverBottomKRParser - Load resource %s failed\n", srcName);
			return NULL;
		}
		CGEBorderCoverBottomKRFilter* proc = new CGEBorderCoverBottomKRFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		proc->setSamplerID(texID);
		proc->setBlockShape(w, h);

		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverUpDownParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float w, h, thickness = 0.1f;
		if(sscanf(pstr, "%256s%f%*c%f%*c%f", srcName, &w, &h, &thickness) < 3)
		{
			CGE_LOG_ERROR("CGEDataParsingEngine::borderCoverUpDownParser - Invalid Param %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);

		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverUpDownParser - Load resource %s failed\n", srcName);
			return NULL;
		}

		CGEBorderCoverUpDownFilter* proc = new CGEBorderCoverUpDownFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		proc->setSamplerID(texID);
		proc->setThickness(thickness);
		proc->setShapeRatio(w / h);
		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

	CGEBorderFilterInterface* CGEDataParsingEngine::borderCoverLeftRightParser(const char* pstr, CGEBorderFilter* fatherFilter)
	{
		using namespace CGE;
		char srcName[256];
		float w, h, thickness = 0.1f;
		if(sscanf(pstr, "%256s%f%*c%f%*c%f", srcName, &w, &h, &thickness) < 3)
		{
			CGE_LOG_ERROR("CGEDataParsingEngine::borderCoverLeftRightParser - Invalid Param %s\n", pstr);
			return NULL;
		}

		GLuint texID = fatherFilter->loadResourceToTexture(srcName);

		if(texID == 0)
		{
			CGE_LOG_ERROR("borderCoverLeftRightParser - Load resource %s failed\n", srcName);
			return NULL;
		}

		CGEBorderCoverLeftRightFilter* proc = new CGEBorderCoverLeftRightFilter;
		if(!proc->init())
		{
			delete proc;
			return NULL;
		}
		proc->setSamplerID(texID);
		proc->setThickness(thickness);
		proc->setShapeRatio(w / h);
		if(fatherFilter != NULL)
			fatherFilter->addFilter(proc);
		return proc;
	}

}

 