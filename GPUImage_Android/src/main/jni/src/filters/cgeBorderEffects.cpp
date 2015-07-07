/*
 * cgeBorderEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeBorderEffects.h"
#include "cgeBordersDataset.h"
#include "cgeDataParsingEngine.h"

namespace CGE
{

	CGEBorderFilter::~CGEBorderFilter()
	{
		clearFilters();
	}

	bool CGEBorderFilter::initWithBorderShortcutID(int index)
	{
		const char* effectData = CGEEffectsDataSet::getBorderStringByID(index);
		return initWithBorderShortcutString(effectData);
	}

	bool CGEBorderFilter::initWithBorderShortcutString(const char* pstr)
	{
		if(pstr == NULL) return false;
		while(*pstr != '\0' && *pstr != '@') ++pstr;
		while(*pstr != '\0' && (*pstr == '@' || *pstr == ' ' || *pstr == '\t')) ++pstr;
		while(*pstr != '\0' && (*pstr != ' ' && *pstr != '\t')) ++pstr;
		if(*pstr == '\0') return false;
		CGEDataParsingEngine::borderSParser(pstr, this);
		return true;
	}

	bool CGEBorderFilter::initWithBorderID(int index, bool isShortCut)
	{
		const char* effectData = CGEEffectsDataSet::getBorderStringByID(index);		
		return initWithBorderString(effectData, isShortCut);
	}

	bool CGEBorderFilter::initWithBorderString(const char* ptr, bool isShortCut)
	{
		//////////////////////////////////////////////////////////////////////////
		//Hardcode for parsing the border functions.
		//////////////////////////////////////////////////////////////////////////
		if(isShortCut)
			return initWithBorderShortcutString(ptr);
		if(ptr == NULL) return false;
		while(*ptr != '\0')
		{
			while(*ptr != '\0' && *ptr != '@') ++ptr;
			while(*ptr != '\0' && (*ptr == '@' || *ptr == ' ' || *ptr == '\t')) ++ptr;
			if(*ptr == '\0') break;
			if(strncmp(ptr, "coverS", 6) == 0)
			{
				ptr += 6;
				CGEDataParsingEngine::borderSParser(ptr, this);
			}
			else if(strncmp(ptr, "coverM", 6) == 0)
			{
				ptr += 6;
				CGEDataParsingEngine::borderMParser(ptr, this);
			}
			else if(strncmp(ptr, "card", 4) == 0)
			{
				ptr += 4;
				CGEDataParsingEngine::borderCardParser(ptr, this);
			}
			else if(strncmp(ptr, "coverskr", 8) == 0)
			{
				ptr += 8;
				CGEDataParsingEngine::borderCoverSKRParser(ptr, this);
			}
			else if(strncmp(ptr, "covercorners", 12) == 0)
			{
				ptr += 12;
				CGEDataParsingEngine::borderCoverCornerParser(ptr, this);
			}
			else if(strncmp(ptr, "coverudt", 8) == 0)
			{
				ptr += 8;
				CGEDataParsingEngine::borderCoverUDTParser(ptr, this);
			}
			else if(strncmp(ptr, "coverstex", 9) == 0)
			{
				ptr += 9;
				CGEDataParsingEngine::borderCoverSWithTexParser(ptr, this, false);
			}
			else if(strncmp(ptr, "coverstkr", 9) == 0)
			{
				ptr += 9;
				CGEDataParsingEngine::borderCoverSWithTexParser(ptr, this, true);
			}
			else if(strncmp(ptr, "covermtex", 9) == 0)
			{
				ptr += 9;
				CGEDataParsingEngine::borderCoverMWithTexParser(ptr, this);
			}
			else if(strncmp(ptr, "cover2ct", 8) == 0)
			{
				ptr += 8;
				CGEDataParsingEngine::borderCoverRTAndLBParser(ptr, this);
			}
			else if(strncmp(ptr, "coverbkr", 8) == 0)
			{
				ptr += 8;
				CGEDataParsingEngine::borderCoverBottomKRParser(ptr, this);
			}
			else if(strncmp(ptr, "udcover", 7) == 0)
			{
				ptr += 7;
				CGEDataParsingEngine::borderCoverUpDownParser(ptr, this);
			}
			else if(strncmp(ptr, "lrcover", 7) == 0)
			{
				ptr += 7;
				CGEDataParsingEngine::borderCoverLeftRightParser(ptr, this);
			}
			else
			{
				CGE_LOG_ERROR("CGEBorderFilter::initWithBorderID - ###Invalid Parameter### %s\n", ptr);
			}
		}

		setThickness(0.1f);
		return true;
	}

	void CGEBorderFilter::setThickness(float thickness)
	{
		for(std::vector<CGEBorderFilterInterface*>::iterator iter = m_vecProc.begin();
			iter != m_vecProc.end(); ++iter)
		{
			(*iter)->setThickness(thickness);
		}
	}

	void CGEBorderFilter::setAlign(float align)
	{
		for(std::vector<CGEBorderFilterInterface*>::iterator iter = m_vecProc.begin();
			iter != m_vecProc.end(); ++iter)
		{
			(*iter)->setAlign(align);
		}
	}

	void CGEBorderFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		if(m_vecProc.empty())
		{
			CGE_LOG_ERROR("CGEBorderFilter::render2Texture has nothing to run!\n");
			return;
		}
		std::vector<CGEBorderFilterInterface*>::iterator iter = m_vecProc.begin();
		for(;;)
		{
			(*iter)->render2Texture(handler, handler->getBufferTextureID(), posVertices);
			if(++iter != m_vecProc.end())
			{
				handler->swapBufferFBO();
			}
			else
			{
				break;
			}
		}
	}

	void CGEBorderFilter::setLoadFunction(CGEBufferLoadFun fLoad, void* loadParam, CGEBufferUnloadFun fUnload, void* unloadParam)
	{
		m_loadFunc = fLoad;
		m_loadParam = loadParam;
		m_unloadFunc = fUnload;
		m_unloadParam = unloadParam;
	}

	GLuint CGEBorderFilter::loadResourceToTexture(const char* srcName)
	{
		int w, h;
		void* bufferData = NULL;
		CGEBufferLoadFun fLoad = m_loadFunc;
		CGEBufferUnloadFun fUnload = m_unloadFunc;
		void* loadParam = m_loadParam;
		void* unloadParam = m_unloadParam;

		if(fLoad == NULL)
		{
			fLoad = cgeGetCommonLoadFunc();
			loadParam = cgeGetCommonLoadArg();
			fUnload = cgeGetCommonUnloadFunc();
			unloadParam = cgeGetCommonUnloadArg();
		}

		CGEBufferFormat fmt;
		void* pArg;
		if(fLoad == NULL || (pArg = fLoad(srcName, &bufferData, &w, &h, &fmt, loadParam)) == NULL)
		{
			CGE_LOG_ERROR("Load texture %s failed!\n", srcName);
			return 0;
		}
		
		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, NULL);

		GLuint texture = cgeGenTextureWithBuffer(bufferData, w, h, channelFmt, dataFmt);

		if(fUnload != NULL)
			fUnload(pArg, unloadParam);
		return texture;
	}

	void CGEBorderFilter::clearFilters()
	{
		for(std::vector<CGEBorderFilterInterface*>::iterator iter = m_vecProc.begin();
			iter != m_vecProc.end(); ++iter)
		{
			delete *iter;
		}
		m_vecProc.clear();
	}

}