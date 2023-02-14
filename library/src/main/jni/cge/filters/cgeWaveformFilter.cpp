#include "cgeWaveformFilter.h"

#define USING_ALPHA 1 /// 视图增加一个 80% 的半透明

static CGEConstString s_cshWaveform = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    layout(rgba8ui, binding = 0) uniform readonly highp uimage2D inputImageTexture;
    layout(rgba8ui, binding = 1) uniform writeonly highp uimage2D outputImage;

    void main() {
        ivec2 texCoord = ivec2(gl_GlobalInvocationID);
        uvec3 color = imageLoad(inputImageTexture, texCoord).rgb;
        float lum = dot(vec3(color.rgb), vec3(0.299, 0.587, 0.114));
        ivec2 newLoc = ivec2(texCoord.x, uint(lum));
        imageStore(outputImage, newLoc, uvec4(255, 255, 255, 255));
    });

static CGEConstString s_cshClearImage = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    layout(rgba8ui, binding = 1) uniform writeonly highp uimage2D outputImage;

    void main() {
        ivec2 texCoord = ivec2(gl_GlobalInvocationID);
        imageStore(outputImage, texCoord, uvec4(0, 0, 0, 255));
    });

namespace CGE
{
CGEWaveformFilter::~CGEWaveformFilter() = default;

bool CGEWaveformFilter::init()
{
    if (m_program.initWithComputeShader(s_cshWaveform) && m_clearImageProgram.initWithComputeShader(s_cshClearImage))
    {
        m_program.bind();
        setFormPosition(0.1f, 0.1f);
        setFormSize(0.3f, 0.3f);
        m_drawer.reset(TextureDrawer::create());
        m_drawer->setFlipScale(1.0f, -1.0f); // flip upside down, meet the gl coord.
        m_diagramTexture = std::make_unique<TextureObject>();
        return true;
    }

    CGE_LOG_ERROR(R"(CGEWaveformFilter::init failed. This filter needs GLES3.1 and later!
 Only GLES 3.1+ support image store. 
 You need to imp a fallback version which reading pixels every frame like `cgeColorMappingFilter`
)");
    CGE_LOG_ERROR("Failed Compute Shader: %s\n", s_cshWaveform);
    return false;
}

void CGEWaveformFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    auto&& sz = handler->getOutputFBOSize();
    if (sz.width != m_diagramTexture->width() || m_diagramTexture->texture() == 0)
    {
        m_diagramTexture->resize(sz.width, 256);
    }

    glBindImageTexture(0, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(1, m_diagramTexture->texture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);

    // Clear diagram texture before frame.
    // You can also use glClear(GL_COLOR_BUFFER_BIT) on some devices.
    {
        // @attention: glClear does not work on some devices. e.g. Mali-G76
        // Perform clear with a compute shader.
        m_clearImageProgram.bind();
        glDispatchCompute(sz.width, sz.height, 1);
    }

    // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_program.bind();

    glDispatchCompute(sz.width, sz.height, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#if USING_ALPHA
    glEnable(GL_BLEND);
    glBlendColor(1, 1, 1, 0.8);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA);
#endif

    handler->setAsTarget();
    glViewport(m_position[0] * sz.width, m_position[1] * sz.height, m_size[0] * sz.width, m_size[1] * sz.height);
    m_drawer->drawTexture(m_diagramTexture->texture());

#if USING_ALPHA
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
#endif
}

void CGEWaveformFilter::setFormPosition(float left, float top)
{
    m_position = { left, top };
}

void CGEWaveformFilter::setFormSize(float width, float height)
{
    m_size = { width, height };
}

} // namespace CGE