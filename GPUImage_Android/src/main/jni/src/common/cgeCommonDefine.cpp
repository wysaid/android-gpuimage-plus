/*
* cgeCommonDefine.cpp
*
*  Created on: 2013-12-6
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeCommonDefine.h"

namespace CGE
{
	void cgePrintGLString(const char* name, GLenum em)
	{
		CGE_LOG_INFO("GL_INFO %s = %s\n", name, glGetString(em));
	}

	void _cgeCheckGLError(const char* name, const char* file, int line)
	{
		int loopCnt = 0;
		for (GLint error = glGetError(); loopCnt < 32 && error; error = glGetError(), ++loopCnt)
		{		
			const char* pMsg;
			switch (error) 
			{
			case GL_INVALID_ENUM: pMsg = "invalid enum"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: pMsg = "invalid framebuffer operation"; break;
			case GL_INVALID_OPERATION: pMsg = "invalid operation"; break;
			case GL_INVALID_VALUE: pMsg = "invalid value"; break;
			case GL_OUT_OF_MEMORY: pMsg = "out of memory"; break;
			default: pMsg = "unknown error";
			}
			CGE_LOG_ERROR("After \"%s\" glGetError %s(0x%x) at %s:%d\n", name, pMsg, error, file, line);
		}
	}

	const char* cgeGetVersion()
	{
		return CGE_VERSION;
	}

	const char* cgeGetBlendModeName(CGETextureBlendMode mode, bool withChinese)
	{
		if(mode < 0 || mode >= CGE_BLEND_TYPE_MAX_NUM)
			return NULL;

		static CGEConstString s_blendModeName[] = 
		{
			"mix", // 0
			"dissolve", // 1

			"darken", // 2
			"multiply", // 3
			"colorburn", // 4
			"linearburn", // 5
			"darkercolor", // 6

			"lighten", // 7
			"screen", // 8
			"colordodge", // 9
			"lineardodge", // 10
			"lightercolor", // 11

			"overlay", // 12
			"softlight", // 13
			"hardlight", // 14
			"vividlight", // 15
			"linearlight", // 16
			"pinlight", // 17
			"hardmix", // 18

			"difference", // 19
			"exclude", // 20
			"subtract", // 21
			"divide", // 22

			"hue", // 23
			"saturation", // 24
			"color", // 25
			"luminosity", // 26

			/////////////    More blend mode below (You can't see them in Adobe Photoshop)    //////////////

			"add",			// 19
			"addrev",	// 21
			"colorbw",		// 22

			/////////////    More blend mode above     //////////////
		};

		static CGEConstString s_blendModeNameWithChinese[] = 
		{
			"mix(正常)", // 0
			"dissolve(溶解)", // 1

			"darken(变暗)", // 2
			"multiply(正片叠底)", // 3
			"color burn(颜色加深)", // 4
			"linear burn(线性加深)", // 5
			"darker color(深色)", // 6

			"lighten(变亮)", // 7
			"screen(滤色)", // 8
			"color dodge(颜色减淡)", // 9
			"linear dodge(线性减淡)", // 10
			"lighter color(浅色)", // 11

			"overlay(叠加)", // 12
			"soft light(柔光)", // 13
			"hard light(强光)", // 14
			"vivid light(亮光)", // 15
			"linear light(线性光)", // 16
			"pin light(点光)", // 17
			"hard mix(实色混合)", // 18

			"difference(差值)", // 19
			"exclude(排除)", // 20
			"subtract(减去)", // 21
			"divide(划分)", // 22

			"hue(色相)", // 23
			"saturation(饱和度)", // 24
			"color(颜色)", // 25
			"luminosity(明度)", // 26

			/////////////    More blend mode below (You can't see them in Adobe Photoshop)    //////////////

			"add(相加)",			// 19
			"addrev(反向加)",	// 21
			"colorbw(黑白)",		// 22

			/////////////    More blend mode above     //////////////
		};

		return withChinese ? s_blendModeNameWithChinese[mode] : s_blendModeName[mode];
	}

	
// 	static const unsigned short n = 0xff00;
// 	const bool g_isBigEnd = (*(const unsigned char*)n) == (unsigned char)0xf;

}