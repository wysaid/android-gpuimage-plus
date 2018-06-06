/*
* cgeShaderFunctions.h
*
*  Created on: 2013-12-6
*      Author: Wang Yang
*/

#ifndef _CGESAHDERFUNCTIONS_H_
#define _CGESAHDERFUNCTIONS_H_

#include "cgeCommonDefine.h"
#include "cgeGLFunctions.h"
#include <cstring>

namespace CGE
{

	class ShaderObject
	{
	private:
		explicit ShaderObject(const ShaderObject&) {}
	public:
        ShaderObject();
		~ShaderObject();

		bool init(GLenum shaderType);

		bool loadShaderSourceFromString(const char* shaderString);
		inline GLuint shaderID() { return m_shaderID; }
		inline GLenum shaderType() { return m_shaderType; }
		void clear();
	private:

		GLenum m_shaderType;
		GLuint m_shaderID;
	};

	class CGEImageHandlerInterface;

	//This class is used when one filter needs many parameters 
	// within familiar programs. 
	// Or the param is modified frequently.
	// The uniforms would only be assigned when the filter is running.
	class UniformParameters
	{
	public:
		~UniformParameters();

		typedef enum UNIFORM_TYPE
		{
			uniformBOOL,
			uniformBOOLV2,
			uniformBOOLV3,
			uniformBOOLV4,
			uniformINT,
			uniformINTV2,
			uniformINTV3,
			uniformINTV4,
			uniformFLOAT,
			uniformFLOATV2,
			uniformFLOATV3,
			uniformFLOATV4,
			uniformSAMPLER1D,
			uniformSAMPLER2D,
			uniformStepsFactor,
			uniformRatioAspect,
			uniformStepsRatio,
		}UNIFORM_TYPE;

		typedef union UniformValue
		{
			GLfloat valuef;
			GLint valuei;
			GLuint* valueuPtr;
		}UniformValue;

		typedef struct UniformData
		{
			UniformData() {}
			UniformData(const char* name, UNIFORM_TYPE type) :
				uniformType(type){ strncpy(uniformName, name, CGE_UNIFORM_MAX_LEN); }

			inline void setValuesi(GLint x, GLint y = 0, GLint z = 0, GLint w = 0)
			{
				uniformValue[0].valuei = x;
				uniformValue[1].valuei = y;
				uniformValue[2].valuei = z;
				uniformValue[3].valuei = w;
			}

			inline void setValuesf(GLfloat x, GLfloat y = 0, GLfloat z = 0, GLfloat w = 0)
			{
				uniformValue[0].valuef = x;
				uniformValue[1].valuef = y;
				uniformValue[2].valuef = z;
				uniformValue[3].valuef = w;
			}

			char uniformName[CGE_UNIFORM_MAX_LEN];
			UNIFORM_TYPE uniformType;
			UniformValue uniformValue[4];
		}UniformData;

		void pushi(const char* name, GLint);
		void pushi(const char* name, GLint, GLint);
		void pushi(const char* name, GLint, GLint, GLint);
		void pushi(const char* name, GLint, GLint, GLint, GLint);

		void pushf(const char* name, GLfloat);
		void pushf(const char* name, GLfloat, GLfloat);
		void pushf(const char* name, GLfloat, GLfloat, GLfloat);
		void pushf(const char* name, GLfloat, GLfloat, GLfloat, GLfloat);

		//Note: the bind ID starts from 0!
		void pushSampler1D(const char* name, GLuint* textureID, GLint textureBindID);
		void pushSampler2D(const char* name, GLuint* textureID, GLint textureBindID);
		void requireStepsFactor(const char* name);
		void requireRatioAspect(const char* name, GLfloat texAspectRatio = 1.0f);
		void requireStepsRatio(const char* name, GLfloat texAspectRatio = 1.0f);

		void clear();
		void assignUniforms(CGEImageHandlerInterface* handler, GLuint programID);

		UniformData* getDataPointerByName(const char* name);


	private:

		std::vector<UniformData*> m_vecUniforms;
	};

	class ProgramObject
	{
	private:
		explicit ProgramObject(const ProgramObject&) {}
	public:
		ProgramObject();
		~ProgramObject();

		bool initWithShaderStrings(const char* vsh, const char* fsh);
		
		bool initFragmentShaderSourceFromString(const char* fragShader);
		bool initVertexShaderSourceFromString(const char* vertShader);

		bool linkWithShaderObject(ShaderObject& vertObj, ShaderObject& fragObj, bool shouldClear = true);
		inline bool link() { return linkWithShaderObject(m_vertObj, m_fragObj); }
		inline void bind() { glUseProgram(m_programID); }

		// For usage convenience, do not use template here.
		inline void sendUniformf(const char* name, GLfloat x)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform1f(uniform, x);
		}

		inline void sendUniformf(const char* name, GLfloat x, GLfloat y)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform2f(uniform, x, y);
		}

		inline void sendUniformf(const char* name, GLfloat x, GLfloat y, GLfloat z)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform3f(uniform, x, y, z);
		}

		inline void sendUniformf(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform4f(uniform, x, y, z, w);
		}

		inline void sendUniformi(const char* name, GLint x)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform1i(uniform, x);
		}

		inline void sendUniformi(const char* name, GLint x, GLint y)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform2i(uniform, x, y);
		}

		inline void sendUniformi(const char* name, GLint x, GLint y, GLint z)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform3i(uniform, x, y, z);
		}

		inline void sendUniformi(const char* name, GLint x, GLint y, GLint z, GLint w)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniform4i(uniform, x, y, z, w);
		}

		inline void sendUniformMat2(const char* name, int count, GLboolean transpose, const GLfloat* matrix)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniformMatrix2fv(uniform, count, transpose, matrix);
		}

		inline void sendUniformMat3(const char* name, GLsizei count, GLboolean transpose, const GLfloat* matrix)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniformMatrix3fv(uniform, count, transpose, matrix);
		}

		inline void sendUniformMat4(const char* name, GLsizei count, GLboolean transpose, const GLfloat* matrix)
		{
			GLint uniform = _getUniform(m_programID, name);
			glUniformMatrix4fv(uniform, count, transpose, matrix);
		}

		inline GLuint programID() { return m_programID; }
		inline GLint attributeLocation(const char* name) { return glGetAttribLocation(m_programID, name); }
		inline GLint uniformLocation(const char* name) { return glGetUniformLocation(m_programID, name); } 
		inline void bindAttribLocation(const char* name, GLuint index) { glBindAttribLocation(m_programID, index, name); }

	protected:
		inline GLint _getUniform(GLuint programId, const char* name)
		{
			GLint uniform = glGetUniformLocation(programId, name);
			CGE_LOG_CODE(
				if(uniform < 0)
					CGE_LOG_ERROR("uniform name %s does not exist!\n", name);
			);
			return uniform;
		}

	private:
		ShaderObject m_vertObj, m_fragObj;
		GLuint m_programID;
	};

}

#endif
