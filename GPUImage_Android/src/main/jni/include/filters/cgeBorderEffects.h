/*
 * cgeBorderEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEBORDEREFFECTS_H_
#define _CGEBORDEREFFECTS_H_

#include "cgeBordersCommon.h"

namespace CGE
{
	class CGEBorderFilter : public CGEImageFilterInterfaceAbstract
	{
	public:
		CGEBorderFilter() {}
		~CGEBorderFilter();
		
		bool initWithBorderID(int index, bool isShortCut = false);
		bool initWithBorderString(const char* pstr, bool isShortCut = false);

		bool initWithBorderShortcutID(int index);
		bool initWithBorderShortcutString(const char* pstr);

		void setThickness(float thickness); //Range [0, 0.5], 0 for orign

		void setAlign(float align); //Range: differ from the borders. See more in the borders.

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

		void setLoadFunction(CGEBufferLoadFun fLoad, void* loadParam, CGEBufferUnloadFun fUnload, void* unloadParam);

		GLuint loadResourceToTexture(const char* srcName);

		CGEBufferLoadFun getLoadFunc() { return m_loadFunc; };
		CGEBufferUnloadFun getUnloadFunc() { return m_unloadFunc; }
		void* getLoadParam() { return m_loadParam; }
		void* getUnloadParam() { return m_unloadParam; }

		void addFilter(CGEBorderFilterInterface* proc) { m_vecProc.push_back(proc); }
		void clearFilters();

	protected:
		CGEBufferLoadFun m_loadFunc;
		CGEBufferUnloadFun m_unloadFunc;
		void* m_loadParam;
		void* m_unloadParam;

	private:
		std::vector<CGEBorderFilterInterface*> m_vecProc;
	};
}

#endif /* CGEBORDEREFFECTS_H_ */
