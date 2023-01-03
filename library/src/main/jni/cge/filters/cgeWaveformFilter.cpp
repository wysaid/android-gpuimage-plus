#include "cgeWaveformFilter.h"

#include <EGL/egl.h>

static CGEConstString s_vshWaveform = "#version 310 es\n" CGE_SHADER_STRING_PRECISION_H(
    layout(location = 0) in vec2 position;
    layout(location = 0) out vec2 textureCoordinate;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        textureCoordinate = (position.xy + 1.0) / 2.0;
    });

static CGEConstString s_fshWaveform = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(location = 0) in vec2 textureCoordinate;
    layout(binding = 0) uniform sampler2D inputImageTexture;
    layout(rgba8, binding = 1) uniform writeonly image2D outputImageTexture;
    layout(location = 0) out vec4 fragColor;

    void main() {
        fragColor = texture(inputImageTexture, textureCoordinate);

        fragColor.r = 1.0;
    });

namespace CGE
{
CGEWaveformFilter::~CGEWaveformFilter()
{
}

bool CGEWaveformFilter::init()
{
    if (initShadersFromString(s_vshWaveform, s_fshWaveform))
    {
        setFormPosition(0.5f, 0.5f);
        setFormSize(0.4f, 0.3f);
        setColor(0.0f, 0.0f, 0.0f, 0.5f);
        m_drawer.reset(TextureDrawer::create());
        return true;
    }

    CGE_LOG_ERROR(R"(CGEWaveformFilter::init failed. This filter needs GLES3.1 and later!
 Only GLES 3.1+ support image store. 
 You need to imp a fallback version which reading pixels every frame like `cgeColorMappingFilter`
)");
    CGE_LOG_ERROR("Failed Vertex Shader: %s\n", s_vshWaveform);
    CGE_LOG_ERROR("Failed Fragment Shader: %s\n", s_fshWaveform);
    return false;
}

void CGEWaveformFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();

    /// 渲染不写入, 使用 imageStore 写入.
    // glColorMask(false, false, false, false);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    // glBindImageTexture(0, srcTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    // glActiveTexture(GL_TEXTURE1);
    // glBindImageTexture(1, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    // glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void CGEWaveformFilter::setFormPosition(float left, float top)
{
    m_position = { left, top };
}

void CGEWaveformFilter::setFormSize(float width, float height)
{
    m_size = { width, height };
}

void CGEWaveformFilter::setColor(float r, float g, float b, float a)
{
    m_color = { r, g, b, a };
}
} // namespace CGE