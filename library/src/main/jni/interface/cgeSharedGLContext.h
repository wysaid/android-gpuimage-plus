/*
* cgeSharedGLContext.h
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEJNI_H_
#define _CGEJNI_H_

#include <EGL/egl.h>

#ifndef EGL_CONTEXT_CLIENT_VERSION
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#endif

#ifndef EGL_RECORDABLE_ANDROID
#define EGL_RECORDABLE_ANDROID 0x3142
#endif

class CGESharedGLContext
{
	CGESharedGLContext() {}

public:

	enum ContextType {
		PBUFFER,
		RECORDABLE_ANDROID
	};

	static CGESharedGLContext* create();
	static CGESharedGLContext* create(int width, int height);
	static CGESharedGLContext* create(EGLContext sharedContext, int width, int height, ContextType type);

	~CGESharedGLContext();

	void makecurrent();

	bool swapbuffers();

	static void setContextColorBits(int r, int g, int b, int a);

private:

	EGLDisplay m_display;
    EGLSurface m_surface;
    EGLContext m_context;

    static EGLint s_bitR, s_bitG, s_bitB, s_bitA;

    bool init(EGLContext sharedContext, int width, int height, ContextType type);
	void destroy();
};

// bool enableGLFunction(void* arg);

#endif