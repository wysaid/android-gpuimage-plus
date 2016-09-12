/*
 * cgeTiltshiftAdjust.h
 *
 *  Created on: 2014-1-24
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
*/

#ifndef _CGETILTSHIFT_H_
#define _CGETILTSHIFT_H_

#include "cgeGLFunctions.h"
#include "cgeSharpenBlurAdjust.h"

namespace CGE
{
	class CGETiltshiftVectorFilter : public CGEImageFilterInterface
	{
	public:
		CGETiltshiftVectorFilter() : m_texture(0), m_samplerScale(0) {}
		~CGETiltshiftVectorFilter() { glDeleteTextures(1, &m_texture); }

		virtual bool init();

		//Both the real size about the image.
		//eg: 100, 100. This means blurring from "start" and gradual changing in "gradient".
		void setBlurGradient(GLfloat start, GLfloat gradient);

		//Real pixels. This means the position the blur line would pass by.
		void setBlurPassPos(GLfloat x, GLfloat y);

		//range: both [0, 1]. This means the direction of the blur line.
		void setBlurNormal(GLfloat x, GLfloat y);

		//range: [0, π/2]. This means the angle of the blur normal.
		//Note: You can choose one of "setBlurNormal" and "setRotation" as you like. The two functions did the same thing.
		void setRotation(GLfloat angle);

		//range: [0, 50]. This means the intensity of the blurring.
		void setBlurRadiusScale(int radius);

		//range: > 1. This defines the max true radius of the blurring.
		//You should know that the sampling would be very slow in OpenGL ES2.0+ on your device if the sampling-radius is too large. The radius you set by "setBlurRadiusScale" simply provides a sampling radius scale whose true radius is less than the "limit" set by this function.
		//This should be called before setBlurRadiusScale
		void setBlurRadiusLimit(int limit);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void flush();

	protected:
		GLuint m_texture;
		CGEBlurFastFilter m_blurProc;
		static CGEConstString paramGradientName;
		static CGEConstString paramBlurPassPosName;
		static CGEConstString paramBlurNormalName;
		
	private:
		int m_samplerScale;
	};

	class CGETiltshiftEllipseFilter : public CGEImageFilterInterface
	{
	public:
		CGETiltshiftEllipseFilter() : m_texture(0), m_samplerScale(0) {}
		~CGETiltshiftEllipseFilter() { glDeleteTextures(1, &m_texture); }

		virtual bool init();

		//gradient > 1. The gradient is defined as the multiples of "RadiusStart".And ("BlurGradient" - "RadiusStart") is the gradual changing zone.
		void setBlurGradient(GLfloat gradient);

		//Real Pixels. The "centralX" and "centralY" sets the center of the ellipse. 
		void setBlurCentralPos(GLfloat centralX, GLfloat centralY);
			
		//Real Pixels.
		//The "radiusX" and "randiusY" sets the radius of the ellipse.
		void setRadiusStart(GLfloat radiusX, GLfloat radiusY);

		//range: [0, 50]. This means the intensity of the blurring.
		void setBlurRadiusScale(int radius);

		//This function is the same as "CGETiltshiftVectorFilter::setBlurRadiusLimit", see the discription above.
		//This should be called before setBlurRadiusScale
		void setBlurRadiusLimit(int limit);

		//range: [0, π/2]. This means the rotation of the ellipse.
		void setRotation(GLfloat rot);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void flush();

	protected:
		GLuint m_texture;
		CGEBlurFastFilter m_blurProc;
		static CGEConstString paramRotationName;
		static CGEConstString paramGradientName;
		static CGEConstString paramCentralPosName;
		static CGEConstString paramRadiusStartName;

	private:
		int m_samplerScale;
	};

	//////////////////////////////////////////////////////////////////////////
	
	class CGETiltshiftVectorWithFixedBlurRadiusFilter : public CGETiltshiftVectorFilter
	{
	public:
		bool init();
	};

	class CGETiltshiftEllipseWithFixedBlurRadiusFilter : public CGETiltshiftEllipseFilter
	{
	public:
		bool init();
	};

}

#endif