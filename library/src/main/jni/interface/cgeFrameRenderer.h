/*
 * cgeFrameRenderer.h
 *
 *  Created on: 2015-11-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGEFRAMERENDERER_H_
#define _CGEFRAMERENDERER_H_

#include "cgeImageHandler.h"
#include "cgeSharedGLContext.h"
#include "cgeTextureUtils.h"
#include "cgeThread.h"
#include "cgeVec.h"

namespace CGE
{
class CGEFastFrameHandler : public CGEImageHandler
{
public:
    void processingFilters();

    void swapBufferFBO();

    inline void useImageFBO()
    {
        CGEImageHandler::useImageFBO();
    }
};

class CGEFrameRenderer
{
public:
    CGEFrameRenderer();
    virtual ~CGEFrameRenderer();

    // srcWidth, srcHeight: width and height of the external texture (values after rotation)
    // dstWidth, dstHeight: width and height for saving video, default 640*480
    bool init(int srcWidth, int srcHeight, int dstWidth = 640, int dstHeight = 480);

    // srcTexture size changed, will recalculate viewport etc.
    void srcResize(int width, int height);

    // Will affect framebuffer settings.
    virtual void update(GLuint externalTexture, float* transformMatrix);

    // Filter process interface, no filter effect will be applied if not called
    virtual void runProc();

    // Does not set framebuffer. To render to screen, bind to 0
    void render(int x, int y, int width, int height);

    // Draw result without mask
    // Note: drawCache, render, and update functions are not thread-safe when used together.
    void drawCache();

    // Does not set framebuffer, viewport etc., draws directly
    // void renderResult();
    // void renderResultWithMask();

    // Set rotation value for external texture
    void setSrcRotation(float rad);

    // Set flip scale value for external texture
    void setSrcFlipScale(float x, float y);

    // Set render result rotation (radians)
    void setRenderRotation(float rad);
    // Set render result flip scale
    void setRenderFlipScale(float x, float y);

    void setFilterWithConfig(CGEConstString config, CGETextureLoadFun texLoadFunc, void* loadArg);

    void setFilterIntensity(float value);

    void setFilter(CGEImageFilterInterfaceAbstract* filter);

    GLuint getTargetTexture();
    GLuint getBufferTexture();

    void setMaskTexture(GLuint maskTexture, float aspectRatio);
    void setMaskTextureRatio(float aspectRatio);

    void setMaskRotation(float rad);
    void setMaskFlipScale(float x, float y);

    inline CGEFastFrameHandler* getImageHandler() { return m_frameHandler; }

    inline void bindImageFBO() { m_frameHandler->useImageFBO(); }

    inline void swapBufferFBO() { m_frameHandler->swapBufferFBO(); }

protected:
    void _calcViewport(int srcWidth, int srcHeight, int dstWidth, int dstHeight);

protected:
    CGEFastFrameHandler* m_frameHandler;
    // Mainly used for redrawing external_OES type textures
    TextureDrawer* m_textureDrawer;
    TextureDrawer4ExtOES* m_textureDrawerExtOES;

    CGESizei m_srcSize, m_dstSize;
    Vec4i m_viewport;

    TextureDrawer* m_cacheDrawer;
    bool m_isUsingMask;

    // Auxiliary parameters
    float m_drawerFlipScaleX, m_drawerFlipScaleY;

    std::mutex m_resultMutex;
};

} // namespace CGE

#endif