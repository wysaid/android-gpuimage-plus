/*
* cgeTextureUtils.h
*
*  Created on: 2015-7-29
*      Author: Wang Yang
*/

#ifndef _CGETEXTUREUTILS_H_
#define _CGETEXTUREUTILS_H_

#include "cgeGLFunctions.h"

namespace CGE
{

	class TextureDrawer
	{
	protected:
		TextureDrawer() : m_vertBuffer(0) {}
		virtual bool init();
	public:
		
        virtual ~TextureDrawer();

		CGE_COMMON_CREATE_FUNC(TextureDrawer, init);

		//绘制 texture 到当前的framebuffer上
		//同样不对viewport进行处理
		virtual void drawTexture(GLuint src);

		//设置绘制时的旋转(弧度)
		//旋转操作先于翻转缩放
		void setRotation(float rad);

		//设置绘制时的翻转或者缩放, 翻转和缩放操作都后于旋转执行
		//eg. 当x，y大于0时为缩放
		//    当x/y 小于0时， 该边将会翻转
		void setFlipScale(float x, float y);

		//特殊外部用法, 绑定扇形逆时针方向四顶点
		void bindVertexBuffer();

        inline ProgramObject& getProgram() { return m_program; }
        
	protected:
		ProgramObject m_program;
		//不使用cgeGlobal里面的， 避免出现context冲突
		GLuint m_vertBuffer;
		GLint m_rotLoc, m_flipScaleLoc;

		void _rotate(GLint location, float rad);

	protected:
		virtual CGEConstString getFragmentShaderString();
		virtual CGEConstString getVertexShaderString();
	};


	////////////////////////////////////////////////////////////

	//直接绘制一张texture， 提供旋转缩放翻转等操作。
	class TextureDrawerExt : public TextureDrawer
	{
	public:
		
        ~TextureDrawerExt();

		CGE_COMMON_CREATE_FUNC(TextureDrawerExt, init);

		//不处理任何viewport相关， 需提前设置
		//将texture绘制到另一张texture上
		void drawTexture2Texture(GLuint src, GLuint dst);

		// void drawTexture(GLuint src) { TextureDrawer::drawTexture(src); }

		//将普通类型的texture绑定为当前的framebuffer
		//需要提供被绑定texture的宽高， 以对viewport匹配
		void bindTextureDst(GLuint dst, int width, int height);

		//绘制 texture 到当前的framebuffer上
		//同样不对viewport进行处理
		// virtual void drawTexture(GLuint src);

		//将texture绘制到绑定的framebuffer中
		void drawTexture2Buffer(GLuint src, bool fullSize = true);

		// //设置绘制时的旋转(弧度)
		// //旋转操作先于翻转缩放
		// void setRotation(float rad);

		// //设置绘制时的翻转或者缩放, 翻转和缩放操作都后于旋转执行
		// //eg. 当x，y大于0时为缩放
		// //    当x/y 小于0时， 该边将会翻转
		// void setFlipScale(float x, float y);

		//绑定扇形逆时针方向四顶点
		// void bindVertexBuffer();

		CGESizei getBufferTexSize() { return m_texSize; }

	protected:
		FrameBuffer m_framebuffer;		
		CGESizei m_texSize;
	};

	class TextureDrawerWithMask : public TextureDrawer
	{
	public:

		CGE_COMMON_CREATE_FUNC(TextureDrawerWithMask, init);
        
        ~TextureDrawerWithMask();

		void setMaskRotation(float rad);
		void setMaskFlipScale(float x, float y);

		void drawTexture(GLuint src);

		void setMaskTexture(GLuint maskTexture);

	protected:
		
		bool init();

	private:
		//将打破 "getFragmentShaderString" 方法的使用。
		CGEConstString getFragmentShaderString() { return nullptr; }

		GLint m_maskRotLoc, m_maskFlipScaleLoc;
		GLuint m_maskTexture;
	};

#ifdef GL_TEXTURE_EXTERNAL_OES

	//本类中， 使用的src texture 都必须是 external_OES 类型
	class TextureDrawer4ExtOES : public TextureDrawer
	{
	public:
		CGE_COMMON_CREATE_FUNC(TextureDrawer4ExtOES, init);

		void drawTexture(GLuint src);

		void setTransform(float* mat16);

	protected:
		CGEConstString getFragmentShaderString();
		CGEConstString getVertexShaderString();

		bool init();

	private:
		GLint m_transformLoc;
	};

#endif
    
    class TextureDrawerYUV : public TextureDrawer
    {
    public:
        CGE_COMMON_CREATE_FUNC(TextureDrawerYUV, init);
        
        //assume the lumaTexture is bind to GL_TEXTURE0
        //and the chromaTexture is bind to GL_TEXTURE1
        void drawTextures();
        void drawTextures(GLuint lumaTex, GLuint chromaTex);
        
    protected:
        void drawTexture(GLuint) {}
        
        CGEConstString getFragmentShaderString();
		CGEConstString getVertexShaderString();
        
    private:
        bool init();
    };

}


//#undef CGE_TEXTURE_DRAWER_STATIC_FUNC

#endif
