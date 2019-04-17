//
//  cgeMultiInputFilter.h
//
//  Created by wysaid on 19/4/16.
//  Copyright © 2019年 wysaid. All rights reserved.
//

#include "cgeGLFunctions.h"

// A demo for multi input
namespace CGE
{
    // We define a rule for uniform sampler2D, all inputs takes the name as "inputTexture<N>"
    // eg: inputTexture0 for the first input.
    class CGEMultiInputFilter : public CGEImageFilterInterface
    {
    public:
        ~CGEMultiInputFilter() override;

        static CGEMultiInputFilter* create(const char* vsh, const char* fsh)
        {
            auto* f = new CGEMultiInputFilter();
            if(!f->initShadersFromString(vsh, fsh))
            {
                delete f;
                f = nullptr;
            }
            return f;
        }

        void render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID) override;

        // Normally the count should not be more than 7 (1 for the origin input).
        void updateInputTextures(GLuint* textures, int count);

        GLuint getProgramID() { return m_program.programID(); }

    protected:
        // The input textures will not be released here.
        std::vector<GLuint> m_inputTextures;
        std::vector<GLint> m_inputTextureLocations;
    };
}