/*
 * cgeFastTiltShift.h
 *
 *  Created on: 2015-01-12
 *      Author: Su Zhiyun
 */

#ifndef _CGEFASTTILTSHIFT_H_
#define _CGEFASTTILTSHIFT_H_

#include "cgeGLFunctions.h"
#include "CGEImageFilter.h"

namespace CGE
{
	class CGEFastTiltShiftInterface : public CGEImageFilterInterface
	{
	public:

		CGEFastTiltShiftInterface(){}
		~CGEFastTiltShiftInterface(){}

		virtual bool init() = 0;


		//Both the real size about the image.
		//eg: 100, 100. This means blurring from "start" and gradual changing in "gradient".
		virtual void setBlurGradient(GLfloat start, GLfloat gradient) = 0;

		//Real Pixels. The "centralX" and "centralY" sets the center of the ellipse. 
		virtual void setBlurCentralPos(GLfloat centralX, GLfloat centralY) = 0;

		//Real Pixels.
		//The "radiusX" and "randiusY" sets the radius of the ellipse.
		virtual void setEllipseFactor(GLfloat fac) = 0;

		//range: both [0, 1]. This means the direction of the blur line.
		virtual void setBlurNormal(GLfloat x, GLfloat y) = 0;

		//range: [0, π/2]. This means the angle of the blur normal.
		//Note: You can choose one of "setBlurNormal" and "setRotation" as you like. The two functions did the same thing.
		virtual void setRotation(GLfloat angle) = 0;

		virtual void setRadius  (float radius) = 0; // range: [0.0f,0.5f]

		virtual void flush() = 0;

		enum GradientMode
		{
			VECTOR = 1,
			ELLIPSE,
		};

		virtual void setGradientMode(GradientMode mode) = 0;


		static CGEFastTiltShiftInterface * Create(GradientMode mode);

		static CGEFastTiltShiftInterface * CreateVector()
		{
			return Create(VECTOR);
		}

		static CGEFastTiltShiftInterface * CreateEllipse()
		{
			return Create(ELLIPSE);
		}

	};

	CGEFastTiltShiftInterface * getFastVectorTiltShiftFilter();
	CGEFastTiltShiftInterface * getFastEllipseTiltShiftFilter();
}

#endif