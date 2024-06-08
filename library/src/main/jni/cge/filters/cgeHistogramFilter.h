#ifndef _HISTOGRAMFILTER_H_
#define _HISTOGRAMFILTER_H_

#include "cgeImageFilter.h"
#include "cgeTextureUtils.h"
#include "cgeVec.h"

namespace CGE{
class CGEHistogramFilter : public CGEImageFilterInterface
    {
    public:
        ~CGEHistogramFilter() override;

        void setFormPosition(float left, float top);

        void setFormSize(float width, float height);

        bool init() override;

        void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID) override;

    protected:
        std::unique_ptr<TextureDrawer> m_drawer;
        std::unique_ptr<FrameBufferTexture> m_renderTarget;
        Vec2f m_position;
        Vec2f m_size;
    };
}
#endif
