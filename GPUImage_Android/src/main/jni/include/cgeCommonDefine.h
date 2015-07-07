/*
* cgeCommonDefine.h
*
*  Created on: 2013-12-6
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGECOMMONDEFINE_H_
#define _CGECOMMONDEFINE_H_

#ifdef GLEW_USED
#include "cgePlatform_GLEW.h"
#elif defined(ANDROID_NDK)
#include "cgePlatforms_ANDROID.h"
#elif defined(IOS_SDK)
#include "cgePlatform_IOS.h"
#elif defined(LIBCGE4QT_LIB)
#include "cgePlatform_QT.h"
#endif

#define CGE_VERSION "1.13.1"

#ifdef CGE_LOG_ERROR
#define cgeCheckGLError(name) CGE::_cgeCheckGLError(name, __FILE__, __LINE__);
#else
#define cgeCheckGLError(name)
#endif
#ifndef _CGE_GET_MACRO_STRING_HELP
#define _CGE_GET_MACRO_STRING_HELP(x) #x
#endif
#ifndef CGE_GET_MACRO_STRING
#define CGE_GET_MACRO_STRING(x) _CGE_GET_MACRO_STRING_HELP(x)
#endif
#define CGE_FLOATCOMP0(x)	(x < 0.001f && x > -0.001f)

#define CGE_UNIFORM_MAX_LEN 32

typedef const char* const CGEConstString;

#define CGE_DELETE(p) do { delete p; p = NULL; } while(0)
#define CGE_DELETE_ARR(p) do { delete[] p; p = NULL; } while(0)

namespace CGE
{
	const char* cgeGetVersion();

	enum CGEBufferFormat
	{
		CGE_FORMAT_RGB_INT8,
		CGE_FORMAT_RGB_INT16,
		CGE_FORMAT_RGB_FLOAT32,
		CGE_FORMAT_RGBA_INT8,
		CGE_FORMAT_RGBA_INT16,
		CGE_FORMAT_RGBA_FLOAT32,
#ifdef GL_BGR
		CGE_FORMAT_BGR_INT8, 
		CGE_FORMAT_BGR_INT16,
		CGE_FORMAT_BGR_FLOAT32,
#endif
#ifdef GL_BGRA
		CGE_FORMAT_BGRA_INT8,		
		CGE_FORMAT_BGRA_INT16,		
		CGE_FORMAT_BGRA_FLOAT32,
#endif
#ifdef GL_LUMINANCE
		CGE_FORMAT_LUMINANCE, // 8 bit
#endif
#ifdef GL_LUMINANCE_ALPHA
		CGE_FORMAT_LUMINANCE_ALPHA, //8+8 bit
#endif

	};

	enum CGETextureBlendMode
	{
		CGE_BLEND_MIX,            // 0 正常
		CGE_BLEND_DISSOLVE,       // 1 溶解

		CGE_BLEND_DARKEN,         // 2 变暗
		CGE_BLEND_MULTIPLY,       // 3 正片叠底
		CGE_BLEND_COLORBURN,      // 4 颜色加深
		CGE_BLEND_LINEARBURN,     // 5 线性加深
		CGE_BLEND_DARKER_COLOR,   // 6 深色

		CGE_BLEND_LIGHTEN,        // 7 变亮
		CGE_BLEND_SCREEN,         // 8 滤色
		CGE_BLEND_COLORDODGE,     // 9 颜色减淡
		CGE_BLEND_LINEARDODGE,    // 10 线性减淡
		CGE_BLEND_LIGHTERCOLOR,  // 11 浅色

		CGE_BLEND_OVERLAY,        // 12 叠加
		CGE_BLEND_SOFTLIGHT,      // 13 柔光
		CGE_BLEND_HARDLIGHT,      // 14 强光
		CGE_BLEND_VIVIDLIGHT,     // 15 亮光
		CGE_BLEND_LINEARLIGHT,    // 16 线性光
		CGE_BLEND_PINLIGHT,       // 17 点光
		CGE_BLEND_HARDMIX,        // 18 实色混合

		CGE_BLEND_DIFFERENCE,     // 19 差值
		CGE_BLEND_EXCLUDE,        // 20 排除
		CGE_BLEND_SUBTRACT,       // 21 减去
		CGE_BLEND_DIVIDE,         // 22 划分

		CGE_BLEND_HUE,            // 23 色相
		CGE_BLEND_SATURATION,     // 24 饱和度
		CGE_BLEND_COLOR,          // 25 颜色
		CGE_BLEND_LUMINOSITY,     // 26 明度		

		/////////////    More blend mode below (You can't see them in Adobe Photoshop)    //////////////

		CGE_BLEND_ADD,			// 27
		CGE_BLEND_ADDREV,	// 28
		CGE_BLEND_COLORBW,		// 29

		/////////////    More blend mode above     //////////////

		CGE_BLEND_TYPE_MAX_NUM //Its value defines the max num of blend.
	};

	const char* cgeGetBlendModeName(CGETextureBlendMode mode, bool withChinese = false);

	void cgePrintGLString(const char*, GLenum);
	void _cgeCheckGLError(const char* name, const char* file, int line); //请直接使用 cgeCheckGLError



	struct CGESizei
	{
		CGESizei(): width(0), height(0) {}
		CGESizei(int w, int h) : width(w), height(h) {}
		void set(int w, int h)
		{
			width = w;
			height = h;
		}
		bool operator ==(const CGESizei &other) const
		{
			return width == other.width && height == other.height; 
		}
		bool operator !=(const CGESizei &other) const
		{
			return width != other.width || height != other.height; 
		}
		GLint width;
		GLint height;
	};

	struct CGESizef
	{
		CGESizef() : width(0.0f), height(0.0f) {}
		CGESizef(float w, float h) : width(w), height(h) {}
		void set(float w, float h)
		{
			width = w;
			height = h;
		}
		GLfloat width;
		GLfloat height;
	};

#ifndef CGE_MIN

	template<typename Type>
	inline Type CGE_MIN(Type a, Type b)
	{
		return a < b ? a : b;
	}

#endif

#ifndef CGE_MAX

	template<typename Type>
	inline Type CGE_MAX(Type a, Type b)
	{
		return a > b ? a : b;
	}

#endif

#ifndef CGE_MID

	template<typename Type>
	inline Type CGE_MID(Type n, Type vMin, Type vMax)
	{
		if(n < vMin)
			n = vMin;
		else if(n > vMax)
			n = vMax;
		return n;
	}

#endif

	struct CGELuminance
	{
		enum { CalcPrecision = 16 };
		enum { Weight = (1<<CalcPrecision) };
		enum { RWeight = int(0.299*Weight), GWeight = int(0.587*Weight), BWeight = int(0.114*Weight) };

		static inline int RGB888(int r, int g, int b)
		{			
			return (r * RWeight + g * GWeight + b * BWeight) >> CalcPrecision;
		}

		//color 从低位到高位的顺序为r-g-b, 传参时需要注意大小端问题
		static inline int RGB565(unsigned short color)
		{
			const int r = (color & 31) << 3;
			const int g = ((color >> 5) & 63) << 2;
			const int b = ((color >> 11) & 31) << 3;

			return RGB888(r, g, b);
		}
	};
	
//	extern const bool g_isBigEnd;
}
#endif /* _CGECOMMONDEFINE_H_ */
