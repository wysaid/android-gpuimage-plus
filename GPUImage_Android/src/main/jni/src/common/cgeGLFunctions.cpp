/*
* cgeGLFunctions.cpp
*
*  Created on: 2013-12-5
*      Author: Wang Yang
*/

#include "cgeGLFunctions.h"
#include <cmath>

namespace CGE
{
	static CGEEnableGLContextFunction s_enableGLFunc = NULL;
	static CGEDisableGLContextFunction s_disableGLFunc = NULL;
	static void* s_enableGLParam;
	static void* s_disableGLParam;
	static bool s_stopGlobalGLEnableFunc = false;

	void cgeSetGLContextEnableFunction(CGEEnableGLContextFunction func, void* param)
	{
		s_enableGLFunc = func;
		s_enableGLParam = param;
	}

	void cgeSetGLContextDisableFunction(CGEDisableGLContextFunction func, void* param)
	{
		s_disableGLFunc = func;
		s_disableGLParam = param;
	}

	void* cgeGetGLEnableParam()
	{
		return s_enableGLParam;
	}

	void* cgeGetGLDisableParam()
	{
		return s_disableGLParam;
	}

	void cgeStopGlobalGLEnableFunction()
	{
		s_stopGlobalGLEnableFunc = true;
	}

	void cgeRestoreGlobalGLEnableFunction()
	{
		s_stopGlobalGLEnableFunc = false;
	}

	void cgeEnableGlobalGLContext()
	{
		if(s_enableGLFunc)
			s_enableGLFunc(s_enableGLParam);
	}

	void cgeDisableGlobalGLContext()
	{
		if(s_disableGLFunc)
			s_disableGLFunc(s_disableGLParam);
	}

	//////////////////////////////////////////////////////////////////////////

	static CGEBufferLoadFun s_loadDataWithSourceNameCommon = NULL;
	static CGEBufferUnloadFun s_unloadBufferDataCommon = NULL;
	static void* s_cgeLoadParam = NULL;
	static void* s_cgeUnloadParam = NULL;

	

	void cgeSetCommonLoadFunction(CGEBufferLoadFun fun, void* arg)
	{
		s_loadDataWithSourceNameCommon = fun;
		s_cgeLoadParam = arg;

	}
	void cgeSetCommonUnloadFunction(CGEBufferUnloadFun fun, void* arg)
	{
		s_unloadBufferDataCommon = fun;
		s_cgeUnloadParam = arg;
	}

	void* cgeLoadResourceCommon(const char* sourceName, void** bufferData, CGEBufferFormat* fmt, GLint* w, GLint* h)
	{
		if(s_loadDataWithSourceNameCommon != NULL)
			return s_loadDataWithSourceNameCommon(sourceName, bufferData, w, h, fmt, &s_cgeLoadParam);
		return NULL;
	}

	bool cgeUnloadResourceCommon(void* bufferArg)
	{
		if(s_unloadBufferDataCommon != NULL)
			return s_unloadBufferDataCommon(bufferArg, s_cgeUnloadParam);
		return false;
	}

	CGEBufferLoadFun cgeGetCommonLoadFunc()
	{
		return s_loadDataWithSourceNameCommon;
	}

	CGEBufferUnloadFun cgeGetCommonUnloadFunc()
	{
		return s_unloadBufferDataCommon;
	}

	void* cgeGetCommonLoadArg()
	{
		return s_cgeLoadParam;
	}

	void* cgeGetCommonUnloadArg()
	{
		return s_cgeUnloadParam;
	}

	void cgeGetDataAndChannelByFormat(CGEBufferFormat fmt, GLenum* dataFmt, GLenum* channelFmt, GLint* channel)
	{
		GLenum df, cf;
		GLint c;
		switch(fmt)
		{
		case CGE_FORMAT_RGB_INT8:
			df = GL_UNSIGNED_BYTE;
			cf = GL_RGB;
			c = 3;
			break;
		case CGE_FORMAT_RGBA_INT8:
			df = GL_UNSIGNED_BYTE;
			cf = GL_RGBA;
			c = 4;
			break;
		case CGE_FORMAT_RGB_INT16:
			df = GL_UNSIGNED_SHORT;
			cf = GL_RGB;
			c = 3;
			break;
		case CGE_FORMAT_RGBA_INT16:
			df = GL_UNSIGNED_SHORT;
			cf = GL_RGBA;
			c = 4;
			break;
		case CGE_FORMAT_RGB_FLOAT32:
			df = GL_FLOAT;
			cf = GL_RGB;
			c = 3;
			break;
		case CGE_FORMAT_RGBA_FLOAT32:
			df = GL_FLOAT;
			cf = GL_RGB;
			c = 4;
			break;

#ifdef GL_BGR
		case CGE_FORMAT_BGR_INT8:
			df = GL_UNSIGNED_BYTE;
			cf = GL_BGR;
			c = 3;
			break;
		case CGE_FORMAT_BGR_INT16:
			df = GL_UNSIGNED_SHORT;
			cf = GL_BGR;
			c = 3;
			break;
		case CGE_FORMAT_BGR_FLOAT32:
			df = GL_FLOAT;
			cf = GL_BGR;
			c = 3;
			break;
#endif
#ifdef GL_BGRA
		case CGE_FORMAT_BGRA_INT8:
			df = GL_UNSIGNED_BYTE;
			cf = GL_BGRA;
			c = 4;
			break;	
		case CGE_FORMAT_BGRA_INT16:
			df = GL_UNSIGNED_SHORT;
			cf = GL_BGRA;
			c = 4;
			break;	
		case CGE_FORMAT_BGRA_FLOAT32:
			df = GL_FLOAT;
			cf = GL_BGRA;
			c = 4;
			break;
#endif
#ifdef GL_RED_EXT
		case CGE_FORMAT_LUMINANCE:
			df = GL_UNSIGNED_BYTE;
			cf = GL_LUMINANCE;
			c = 1;
#endif
#ifdef GL_LUMINANCE_ALPHA
		case CGE_FORMAT_LUMINANCE_ALPHA:
			df = GL_UNSIGNED_BYTE;
			cf = GL_LUMINANCE_ALPHA;
			c = 2;
#endif
		default:
			df = GL_FALSE;
			cf = GL_FALSE;
			c = 0;
		}

		if(dataFmt != NULL) *dataFmt = df;
		if(channelFmt != NULL) *channelFmt = cf;
		if(channel != NULL) *channel = c;
	}

	GLuint cgeGenTextureWithBuffer(const void* bufferData, GLint w, GLint h, GLenum channelFmt, GLenum dataFmt, GLint channel, GLint bindID, GLenum texFilter, GLenum texWrap)
	{
		GLuint tex;
		static const GLenum eArrs[] = { GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
		static const GLint unpackArgs[] = { 1, 2, 1, 4};
		if(channel <= 0 || channel > 4)
			return 0;
		const GLenum& internalFormat = eArrs[channel - 1];
		const GLint& unpackArg = unpackArgs[channel - 1];
		glActiveTexture(GL_TEXTURE0 + bindID);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, unpackArg);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, channelFmt, dataFmt, bufferData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrap);
		return tex;
	}

	char* cgeGetScaledBufferOutofSize(const void* buffer, int& w, int& h, int channel, int minSizeX, int minSizeY)
	{
		if((minSizeX < w && minSizeY < h) || buffer == NULL) return NULL;
		char *tmpbuffer = NULL;
		const char* data = (const char*)buffer;

		int width = w;
		double scale = CGE_MIN(w / (float)minSizeX, h / (float)minSizeY);
		w = ceilf(w / scale);
		h = ceilf(h / scale);

		int len = w * h;
		tmpbuffer = new char[len * channel];
		if(channel == 4)
		{
			for(int i = 0; i != h; ++i)
			{
				for(int j = 0; j != w; ++j)
				{
					const int L = (j + i * w) * channel;
					const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
					tmpbuffer[L] = data[R];
					tmpbuffer[L+1] = data[R+1];
					tmpbuffer[L+2] = data[R+2];
					tmpbuffer[L+3] = data[R+3];
				}
			}
		}
		else
		{
			for(int i = 0; i != h; ++i)
			{
				for(int j = 0; j != w; ++j)
				{
					const int L = (j + i * w) * channel;
					const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
					tmpbuffer[L] = data[R];
					tmpbuffer[L+1] = data[R+1];
					tmpbuffer[L+2] = data[R+2];
				}
			}
		}
		return tmpbuffer;
	}

	char* cgeGetScaledBufferInSize(const void* buffer, int& w, int& h, int channel, int maxSizeX, int maxSizeY)
	{
		if((maxSizeX > w && maxSizeY > h) || buffer == NULL) return NULL;
		char *tmpbuffer = NULL;
		const char* data = (const char*)buffer;

		int width = w;
		double scale = CGE_MAX(w / (float)maxSizeX, h / (float)maxSizeY);
		w = w / scale;
		h = h / scale;
		int len = w * h;
		tmpbuffer = new char[len * channel];
		if(channel == 4)
		{
			for(int i = 0; i != h; ++i)
			{
				for(int j = 0; j != w; ++j)
				{
					const int L = (j + i * w) * channel;
					const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
					tmpbuffer[L] = data[R];
					tmpbuffer[L+1] = data[R+1];
					tmpbuffer[L+2] = data[R+2];
					tmpbuffer[L+3] = data[R+3];
				}
			}
		}
		else
		{
			for(int i = 0; i != h; ++i)
			{
				for(int j = 0; j != w; ++j)
				{
					const int L = (j + i * w) * channel;
					const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
					tmpbuffer[L] = data[R];
					tmpbuffer[L+1] = data[R+1];
					tmpbuffer[L+2] = data[R+2];
				}
			}
		}
		return tmpbuffer;
	}	

	CGE_LOG_CODE(
		int  SharedTexture::sTextureCount;
	)

}
