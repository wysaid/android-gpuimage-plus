/*
 * cgeDynamicWaveFilter.h
 *
 *  Created on: 2015-11-12
 *      Author: Wang Yang
 */

#ifndef _CGEDYNAMICWAVE_H_
#define _CGEDYNAMICWAVE_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEDynamicWaveFilter : public CGEImageFilterInterface
	{
	public:

		void setIntensity(float value); // The same to setAutoMotionSpeed

		bool init();

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void setWaveMotion(float motion);
		void setWaveAngle(float angle); //default: 20 (从左往右, 从上往下 包含 angle / PI 个周期)
        void setStrength(float strength); //default: 0.01 强度(百分比), 范围 (0, 1)

		void setAutoMotionSpeed(float speed); // Auto motion would be disabled if speed <= 0.

	protected:
		static CGEConstString paramMotion;
		static CGEConstString paramAngle;
        static CGEConstString paramStrength;

	private:

		GLint m_motionLoc;
		GLint m_angleLoc;
        GLint m_strengthLoc;
		
		GLfloat m_motion;
		GLfloat m_motionSpeed;
		GLfloat m_angle;
        GLfloat m_strength;
		bool m_autoMotion;
	};

}

#endif