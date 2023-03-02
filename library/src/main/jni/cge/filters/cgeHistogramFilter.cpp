
#include "cgeHistogramFilter.h"

#include <EGL/egl.h>
#include <array>

static CGEConstString s_vshHistogram = "#version 320 es\n" CGE_SHADER_STRING_PRECISION_H(
        layout(location = 0) in vec2 position;
        layout(location = 0) out vec2 textureCoordinate;
        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            textureCoordinate = (position.xy + 1.0) / 2.0;
        });

static CGEConstString s_fshHistogram = "#version 320 es\n" CGE_SHADER_STRING(
        precision highp float;
        precision highp int;
        layout(location = 0) in vec2 textureCoordinate;
        layout(rgba8ui, binding = 0) uniform readonly highp uimage2D inputImageTexture;
        layout(rgba8ui, binding = 1) uniform writeonly highp uimage2D outputImage;
        layout(location = 0) out vec4 fragColor;

        void main() {
            fragColor = vec4(1.0);

            vec4 color = texture(inputImageTexture, textureCoordinate);
            float lum = dot(color.rgb, vec3(0.299, 0.587, 0.114));
            int newLoc = int(lum * 255.0);
            if (newLoc >= 0 && newLoc < 256) {
                //vec2 location = vec2(newLoc, 0.0);
                ivec2 location = ivec2(newLoc, 0);
                atomicAdd(imageAtomic(outputImage, location), 1);
                //imageAtomic(outputImage, ivec2(location.x, location.y), uvec4(1, 0, 0, 0));
            }
        });

namespace CGE {

    CGEHistogramFilter::~CGEHistogramFilter() {}

    bool CGEHistogramFilter::init() {
        if (initShadersFromString(s_vshHistogram, s_fshHistogram)) {
            m_program.bind();
            setFormPosition(0.1f, 0.1f);
            setFormSize(0.3f, 0.3f);
            m_drawer.reset(TextureDrawer::create());
            m_drawer->setFlipScale(1.0f, -1.0f);
            m_renderTarget = std::make_unique<FrameBufferTexture>();
            return true;
        }

        return false;
    }

    void CGEHistogramFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID) {
        auto&& sz = handler->getOutputFBOSize();
        if (sz.width != m_renderTarget->width() || m_renderTarget->texture() == 0) {
            m_renderTarget->bindTexture2D(sz.width, 256);
        }
        m_renderTarget->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glFinish();

        m_program.bind();
        glColorMask(false, false, false, false);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        glUniform1i(m_program.uniformLocation("inputImageTexture"), 0);

        // 获取纹理尺寸，用于计算像素位置
        const auto texSize = handler->getOutputFBOSize();
        // 清空直方图数组
        std::array<float, 256> histogram{};
        // 遍历纹理像素，计算亮度直方图
        std::vector<unsigned char> pixels(texSize.width * texSize.height * 4);
        glReadPixels(0, 0, texSize.width, texSize.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
//        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        for (int i = 0; i < pixels.size(); i += 4) {
            const auto r = pixels[i];
            const auto g = pixels[i + 1];
            const auto b = pixels[i + 2];
            const auto lum = 0.299f * r + 0.587f * g + 0.114f * b;
            histogram[static_cast<int>(lum * 255)]++;
        }

        // 将直方图数据传入图像数据，写入 GLSL Image 中
        auto data = m_renderTarget->mapBuffer();
        for (int i = 0; i < histogram.size(); i++) {
            const auto height = static_cast<int>(histogram[i] * 256);
            for (int j = 0; j < height; j++) {
                const auto pixel = reinterpret_cast<unsigned char*>(data) + j * m_renderTarget->width() * 4 + i * 4;
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
                pixel[3] = 255;
            }
        }
        m_renderTarget->unmapBuffer();

        glViewport(m_position[0] * sz.width, m_position[1] * sz.height, m_size[0] * sz.width, m_size[1] * sz.height);
        // 绘制直方图
        m_drawer->drawTexture(m_renderTarget->texture());

        glColorMask(true, true, true, true);

        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void CGEHistogramFilter::setFormPosition(float left, float top)
    {
        m_position = {left, top};
    }

    void CGEHistogramFilter::setFormSize(float width, float height)
    {
        m_size = {width, height};
    }
}
