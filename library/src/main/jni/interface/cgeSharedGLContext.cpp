/*
* cgeSharedGLContext.cpp
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeSharedGLContext.h"
#include "cgeCommonDefine.h"

EGLint CGESharedGLContext::s_bitR = 8;
EGLint CGESharedGLContext::s_bitG = 8;
EGLint CGESharedGLContext::s_bitB = 8;
EGLint CGESharedGLContext::s_bitA = 8;

void CGESharedGLContext::setContextColorBits(int r, int g, int b, int a)
{
    s_bitR = r;
    s_bitG = g;
    s_bitB = b;
    s_bitA = a;
}

CGESharedGLContext* CGESharedGLContext::create()
{
    CGESharedGLContext* context = new CGESharedGLContext();
	if(!context->init(EGL_NO_CONTEXT, 64, 64, PBUFFER))
    {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext* CGESharedGLContext::create(int width, int height)
{
    CGESharedGLContext* context = new CGESharedGLContext();
    if(!context->init(EGL_NO_CONTEXT, width, height, PBUFFER))
    {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext* CGESharedGLContext::create(EGLContext sharedContext, int width, int height, ContextType type)
{
    CGESharedGLContext* context = new CGESharedGLContext();
    if(!context->init(sharedContext, width, height, type))
    {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext::~CGESharedGLContext()
{
	destroy();
}

bool CGESharedGLContext::init(EGLContext sharedContext, int width, int height, ContextType type)
{
    EGLint attribList[] {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLint configSpec[] = {
        // EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE, 0,
        EGL_NONE
    };

    const int configSize = sizeof(configSpec) / sizeof(*configSpec);

    switch(type)
    {
    case RECORDABLE_ANDROID:
        configSpec[configSize - 3] = EGL_RECORDABLE_ANDROID;
        configSpec[configSize - 2] = 1;
        break;
    case PBUFFER:
        configSpec[configSize - 3] = EGL_SURFACE_TYPE;
        configSpec[configSize - 2] = EGL_PBUFFER_BIT;

    default:;
    }

    const EGLint pBufferAttrib[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    EGLint format;


    CGE_LOG_INFO("Initializing context");

    if ((m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
    {
        CGE_LOG_ERROR("eglGetDisplay() returned error 0x%x", eglGetError());
        return false;
    }

    EGLint major, minor;

    if (!eglInitialize(m_display, &major, &minor))
    {
        CGE_LOG_ERROR("eglInitialize() returned error 0x%x", eglGetError());
        return false;
    }

    CGE_LOG_INFO("eglInitialize: major: %d, minor: %d", major, minor);

    if (!eglChooseConfig(m_display, configSpec, &config, 1, &numConfigs))
    {
        CGE_LOG_ERROR("eglChooseConfig() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    CGE_LOG_INFO("Config num: %d, sharedContext id: %p", numConfigs, sharedContext);

    m_context = eglCreateContext(m_display, config, sharedContext, attribList);
    if(m_context == EGL_NO_CONTEXT)
    {
        CGE_LOG_ERROR("eglCreateContext Failed:  0x%x", eglGetError());
        return false;
    }

    m_surface = eglCreatePbufferSurface(m_display, config, pBufferAttrib);
    if(m_surface == EGL_NO_SURFACE)
    {
        CGE_LOG_ERROR("eglCreatePbufferSurface Failed:  0x%x", eglGetError());
        return false;
    }

    if(!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
    {
        CGE_LOG_ERROR("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }

    int clientVersion;
    eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &clientVersion);
    CGE_LOG_INFO("EGLContext created, client version %d\n", clientVersion);

	return true;
}

void CGESharedGLContext::destroy()
{
    CGE_LOG_INFO("####CGESharedGLContext Destroying context... ####");

    if(m_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_display, m_context);
        eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);
    }	

    m_display = EGL_NO_DISPLAY;
    m_surface = EGL_NO_SURFACE;
    m_context = EGL_NO_CONTEXT;
}

void CGESharedGLContext::makecurrent()
{
    if(!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
    {
        CGE_LOG_ERROR("eglMakeCurrent failed: 0x%x", eglGetError());
    }
}

bool CGESharedGLContext::swapbuffers()
{
    return eglSwapBuffers(m_display, m_surface);
}
