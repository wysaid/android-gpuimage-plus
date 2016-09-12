/*
* cgeFaceBlendFilter.h
*
*  Created on: 2015-12-17
*      Author: Wang Yang
* Description: 人脸混合
*/

#ifndef _CGE_FACEBLEND_FILTER_H_
#define _CGE_FACEBLEND_FILTER_H_

#include "cgeGLFunctions.h"
#include "cgeVec.h"
#include "cgeTextureUtils.h"
#include "cgeBlendFilter.h"

namespace CGE
{
    //incomplete.
    class CGEFaceBlendFilter : public CGEImageFilterInterface
    {
    public:
        CGEFaceBlendFilter();
        ~CGEFaceBlendFilter();

        typedef struct FaceFeature
        {
            Vec2f leftEyePos, rightEyePos;
            Vec2f mouthPos;
            Vec2f chinPos;
            Vec2f texSize;
            GLuint texID;
        }FaceFeature;

        bool initWithBlendMode(CGETextureBlendMode blendMode);

        void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

        void setFaces(const FaceFeature& srcFace, const FaceFeature& dstFace);

    protected:
        void _clearBuffers();
        void _genFaceBuffers();
        static void _assginBufferWithFaceFeature(const FaceFeature& feature, GLuint vbo);

        double getAverageLumByTexture(const FrameBuffer& framebuffer, GLuint texture, const Vec4f& area, const Vec2f& texSize);

    private:
        FaceFeature m_srcFaceFeature, m_dstFaceFeature;
        GLuint m_srcFaceVertexVBO, m_dstFaceVertexVBO;
        GLuint m_faceGradientVBO;
        GLuint m_faceMeshIndexVBO;
        int m_drawCounts;

        GLuint m_cacheTexture;
        FrameBuffer m_framebuffer;
        double m_srcAvgColor, m_dstAvgColor;

        TextureDrawer* m_drawer;
    };

}


#endif // !_CGE_FACEBLEND_FILTER_H_
