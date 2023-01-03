#include "cgeWaveformFilter.h"

#include <EGL/egl.h>

static CGEConstString s_vshWaveform = "#version 320 es\n" CGE_SHADER_STRING_PRECISION_H(
    layout(location = 0) in vec2 position;
    layout(location = 0) out vec2 textureCoordinate;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        textureCoordinate = (position.xy + 1.0) / 2.0;
    });

static CGEConstString s_fshWaveform = "#version 320 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;
    layout(location = 0) in vec2 textureCoordinate;
    layout(binding = 0) uniform sampler2D inputImageTexture;
    layout(rgba8ui, binding = 1) uniform writeonly highp uimage2D outputImage;
    layout(location = 0) out vec4 fragColor;

    void main() {
        fragColor = vec4(1.0);
        vec4 color = texture(inputImageTexture, textureCoordinate);
        float lum = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        ivec2 newLoc = ivec2(vec2(textureCoordinate.x, lum) * vec2(imageSize(outputImage)));
        uint newLum = uint(lum * 255.0);
        imageStore(outputImage, newLoc, uvec4(newLum, newLum, newLum, 255));

        // TODO: 考虑使用 imageAtomicAdd 保障原子操作 (不闪屏)
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
        m_program.bind();
        setFormPosition(0.1f, 0.1f);
        setFormSize(0.3f, 0.3f);
        setColor(0.0f, 0.0f, 0.0f, 0.5f);
        m_drawer.reset(TextureDrawer::create());
        m_drawer->setFlipScale(1.0f, -1.0f); // flip upside down, meet the gl coord.
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
    glClear(GL_COLOR_BUFFER_BIT);
    m_program.bind();

    /// 渲染不写入, 使用 imageStore 写入.
    glColorMask(false, false, false, false);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(1, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glColorMask(true, true, true, true);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

    handler->swapBufferFBO();
    handler->setAsTarget();
    auto&& sz = handler->getOutputFBOSize();
    glViewport(m_position[0] * sz.width, m_position[1] * sz.height, m_size[0] * sz.width, m_size[1] * sz.height);
    m_drawer->drawTexture(handler->getBufferTextureID());
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