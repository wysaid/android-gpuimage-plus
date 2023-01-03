#include "cgeWaveformFilter.h"

#include <EGL/egl.h>

static CGEConstString s_cshWaveform = "#version 310 es\n" CGE_SHADER_STRING(
    precision highp float;
    precision highp int;

    layout(local_size_x = 16, local_size_y = 16) in;
    layout(rgba8, binding = 0) uniform readonly highp image2D inputImage;
    layout(rgba8, binding = 1) uniform writeonly highp image2D outputImage;

    void main() {
        ivec2 workPos = ivec2(gl_GlobalInvocationID.xy);
        vec3 inputColor = imageLoad(inputImage, workPos).rgb;
        float lum = dot(inputColor, vec3(0.299, 0.587, 0.114));
        ivec2 newLoc = ivec2(workPos.x, int(lum * float(imageSize(outputImage).y)));
        imageStore(outputImage, newLoc, vec4(lum));
    });

namespace CGE
{
CGEWaveformFilter::~CGEWaveformFilter()
{
}

bool CGEWaveformFilter::init()
{
    if (m_program.initWithComputeShader(s_cshWaveform))
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
    CGE_LOG_ERROR("Failed Compute Shader: %s\n", s_cshWaveform);
    return false;
}

void CGEWaveformFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();
    auto& sz = handler->getOutputFBOSize();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glBindImageTexture(0, srcTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
    glBindImageTexture(0, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    glDispatchCompute(std::max(sz.width / 16, 1), std::max(sz.height / 16, 1), 1);

    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

    handler->swapBufferFBO();

    // glActiveTexture(GL_TEXTURE1);
    // glBindImageTexture(1, handler->getTargetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    // glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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