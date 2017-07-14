//
// Created by Xue Weining on 2017/6/27.
//

#ifndef ANDROID_GPUIMAGE_PLUS_CGEFACEDETECTFILTER_H_H
#define ANDROID_GPUIMAGE_PLUS_CGEFACEDETECTFILTER_H_H

#include "cgeGLFunctions.h"
namespace CGE
{
    class CGEFaceDetectFilter : public CGEImageFilterInterface {
    public:
        bool init();
        void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);
    protected:
        ProgramObject m_programDrawPoint;
    private:
        GLfloat* m_keyfacePoint;
        GLfloat* m_keyfaceColor;
        GLint aPosition;
        GLint aColor;
    };
}

#endif //ANDROID_GPUIMAGE_PLUS_CGEFACEDETECTFILTER_H_H
