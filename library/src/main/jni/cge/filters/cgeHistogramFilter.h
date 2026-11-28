/*
 * cgeHistogramFilter.h
 *
 *  Luminance histogram overlay filter using GLES 3.1 compute shaders
 *  with imageAtomicAdd for GPU-side accumulation.
 *
 *  Rule syntax: @style hist <left> <top> <width> <height>
 *
 *  e.g. "@style hist 0.05 0.05 0.25 0.25"
 *       Renders a 256×256 histogram in the top-left corner of the frame.
 */

#ifndef _CGE_HISTOGRAM_FILTER_H_
#define _CGE_HISTOGRAM_FILTER_H_

#include "cgeImageFilter.h"
#include "cgeTextureUtils.h"
#include "cgeVec.h"

namespace CGE
{
class CGEHistogramFilter : public CGEImageFilterInterface
{
public:
    ~CGEHistogramFilter() override;

    /**
     * @brief Position of the top-left corner of the overlay (normalised, [0, 1]).
     *        Defaults to (0.1, 0.1).
     */
    void setFormPosition(float left, float top);

    /**
     * @brief Size of the histogram overlay (normalised, [0, 1]).
     *        Defaults to (0.3, 0.3).
     */
    void setFormSize(float width, float height);

    bool init() override;

    void render2Texture(CGEImageHandlerInterface* handler,
                        GLuint srcTexture,
                        GLuint vertexBufferID) override;

protected:
    std::unique_ptr<TextureDrawer> m_drawer;
    std::unique_ptr<TextureObject> m_displayTexture; ///< 256×256 rgba8 — drawn onto frame

    GLuint m_histogramTex = 0; ///< 256×1 r32ui  — atomic luminance counters

    ProgramObject m_clearProgram; ///< pass 1: zero out histogram counters
    ProgramObject m_countProgram; ///< pass 2: accumulate per-pixel luminance
    ProgramObject m_drawProgram;  ///< pass 3: render bar chart into display tex

    Vec2f m_position; ///< normalised top-left
    Vec2f m_size;     ///< normalised width/height
};
} // namespace CGE

#endif // _CGE_HISTOGRAM_FILTER_H_
