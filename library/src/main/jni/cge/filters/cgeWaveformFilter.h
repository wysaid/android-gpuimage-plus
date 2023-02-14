#ifndef _WAVEFORMFILTER_H_
#define _WAVEFORMFILTER_H_

#include "cgeImageFilter.h"
#include "cgeTextureUtils.h"
#include "cgeVec.h"

namespace CGE
{
class CGEWaveformFilter : public CGEImageFilterInterface
{
public:
    ~CGEWaveformFilter() override;

    /**
     * @brief 左上角的点
     */
    void setFormPosition(float left, float top);
    /**
     * @brief 相对大小
     */
    void setFormSize(float width, float height);

    bool init() override;

    void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID) override;

protected:
    std::unique_ptr<TextureDrawer> m_drawer;
    std::unique_ptr<TextureObject> m_diagramTexture;
    ProgramObject m_clearImageProgram;
    
    Vec2f m_position;
    Vec2f m_size;
};
} // namespace CGE

#endif