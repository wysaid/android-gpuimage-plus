/*
 * cgeHistogramFilter.cpp
 *
 *  Luminance histogram overlay rendered entirely on the GPU using three
 *  GLES 3.1 compute-shader passes:
 *
 *    Pass 1 (clear)  — zero the 256×1 r32ui atomic-counter texture
 *    Pass 2 (count)  — imageAtomicAdd per pixel into the counter texture
 *    Pass 3 (draw)   — write bar-chart pixels into the 256×256 display texture
 *
 *  The display texture is then blended onto the frame at the configured
 *  position / size, exactly like CGEWaveformFilter.
 *
 *  Requirements: OpenGL ES 3.1 (compute shaders + image load/store).
 */

#include "cgeHistogramFilter.h"

#define USING_ALPHA 1 /// 80 % semi-transparent blend, same as WaveformFilter

// ---------------------------------------------------------------------------
// Pass 1 — clear the 256 histogram bins
// Dispatched as glDispatchCompute(256, 1, 1)
// ---------------------------------------------------------------------------
static CGEConstString s_cshHistClear = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    layout(r32ui, binding = 2) writeonly uniform highp uimage2D histogramImage;

    void main() {
        imageStore(histogramImage, ivec2(int(gl_GlobalInvocationID.x), 0), uvec4(0u, 0u, 0u, 0u));
    });

// ---------------------------------------------------------------------------
// Pass 2 — accumulate luminance histogram via atomic add
// Dispatched as glDispatchCompute(frameWidth, frameHeight, 1)
// ---------------------------------------------------------------------------
static CGEConstString s_cshHistCount = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    layout(rgba8ui, binding = 0) uniform readonly highp uimage2D inputImageTexture;
    // coherent volatile restrict are required for imageAtomicAdd
    layout(r32ui, binding = 2) coherent volatile restrict uniform highp uimage2D histogramImage;

    void main() {
        ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
        uvec3 color    = imageLoad(inputImageTexture, texCoord).rgb;

        // BT.601 luminance (integer approximation kept in float for clarity)
        float lum = dot(vec3(color.rgb), vec3(0.299, 0.587, 0.114));
        uint  bin = clamp(uint(lum), 0u, 255u);
        imageAtomicAdd(histogramImage, ivec2(bin, 0), 1u);
    });

// ---------------------------------------------------------------------------
// Pass 3 — render bar chart into the 256×256 display texture
// Dispatched as glDispatchCompute(256, 256, 1)
//
// u_scaleInv = 256.0 / (frameWidth * frameHeight)
//   → a bin containing 1/256 of all pixels exactly fills the bar height
// ---------------------------------------------------------------------------
static CGEConstString s_cshHistDraw = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    layout(r32ui,   binding = 2) readonly  uniform highp uimage2D histogramImage;
    layout(rgba8ui, binding = 1) writeonly uniform highp uimage2D displayImage;

    uniform float u_scaleInv; // 256.0 / (frameWidth * frameHeight)

    void main() {
        ivec2 coord  = ivec2(gl_GlobalInvocationID.xy);
        uint  count  = imageLoad(histogramImage, ivec2(coord.x, 0)).r;

        // Normalise: full height when the bin contains 1/256 of total pixels
        uint barHeight = uint(clamp(float(count) * u_scaleInv, 0.0, 1.0) * 256.0);

        // coord.y == 0 is the bottom of the bar (low luminance side after flip)
        if (uint(coord.y) < barHeight) {
            imageStore(displayImage, coord, uvec4(255u, 255u, 255u, 255u));
        } else {
            imageStore(displayImage, coord, uvec4(0u, 0u, 0u, 255u));
        }
    });

// ===========================================================================

namespace CGE
{
CGEHistogramFilter::~CGEHistogramFilter()
{
    if (m_histogramTex != 0)
    {
        glDeleteTextures(1, &m_histogramTex);
        m_histogramTex = 0;
    }
}

bool CGEHistogramFilter::init()
{
    if (!m_clearProgram.initWithComputeShader(s_cshHistClear) ||
        !m_countProgram.initWithComputeShader(s_cshHistCount) ||
        !m_drawProgram.initWithComputeShader(s_cshHistDraw))
    {
        CGE_LOG_ERROR(
            "CGEHistogramFilter::init failed.\n"
            "This filter requires GLES 3.1+ (compute shaders + imageAtomicAdd).\n");
        CGE_LOG_ERROR("  clear shader : %s\n", s_cshHistClear);
        return false;
    }

    setFormPosition(0.1f, 0.1f);
    setFormSize(0.3f, 0.3f);

    m_drawer.reset(TextureDrawer::create());
    m_drawer->setFlipScale(1.0f, -1.0f); // flip to match GL coordinate system

    m_displayTexture = std::make_unique<TextureObject>();
    // Allocate the 256×256 display texture (RGBA8, same as waveform uses).
    m_displayTexture->resize(256, 256);

    // Create 256×1 r32ui texture for atomic luminance counters.
    glGenTextures(1, &m_histogramTex);
    glBindTexture(GL_TEXTURE_2D, m_histogramTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 256, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void CGEHistogramFilter::render2Texture(CGEImageHandlerInterface* handler,
                                        GLuint /*srcTexture*/,
                                        GLuint /*vertexBufferID*/)
{
    auto&& sz = handler->getOutputFBOSize();

    // scale factor: a uniform histogram bin (1/256 of pixels) → full bar height
    const float scaleInv = 256.0f / static_cast<float>(sz.width * sz.height);

    // -----------------------------------------------------------------------
    // Bind image units
    //   binding 0 : source frame    (rgba8ui, read-only)
    //   binding 1 : display texture (rgba8ui, write-only)
    //   binding 2 : histogram bins  (r32ui,   read-write for atomic)
    // -----------------------------------------------------------------------
    glBindImageTexture(0, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8UI);
    glBindImageTexture(1, m_displayTexture->texture(),   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);
    glBindImageTexture(2, m_histogramTex,                0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    // -----------------------------------------------------------------------
    // Pass 1 — clear bins
    // -----------------------------------------------------------------------
    m_clearProgram.bind();
    glDispatchCompute(256, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // -----------------------------------------------------------------------
    // Pass 2 — count luminance values
    // -----------------------------------------------------------------------
    m_countProgram.bind();
    glDispatchCompute(sz.width, sz.height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // -----------------------------------------------------------------------
    // Pass 3 — render bar chart
    // -----------------------------------------------------------------------
    m_drawProgram.bind();
    m_drawProgram.sendUniformf("u_scaleInv", scaleInv);
    glDispatchCompute(256, 256, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // -----------------------------------------------------------------------
    // Composite the display texture onto the frame
    // -----------------------------------------------------------------------
#if USING_ALPHA
    glEnable(GL_BLEND);
    glBlendColor(1, 1, 1, 0.8f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA);
#endif

    handler->setAsTarget();
    glViewport(
        static_cast<GLint>(m_position[0] * sz.width),
        static_cast<GLint>(m_position[1] * sz.height),
        static_cast<GLsizei>(m_size[0] * sz.width),
        static_cast<GLsizei>(m_size[1] * sz.height));

    m_drawer->drawTexture(m_displayTexture->texture());

#if USING_ALPHA
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
#endif
}

void CGEHistogramFilter::setFormPosition(float left, float top)
{
    m_position = { left, top };
}

void CGEHistogramFilter::setFormSize(float width, float height)
{
    m_size = { width, height };
}

} // namespace CGE
