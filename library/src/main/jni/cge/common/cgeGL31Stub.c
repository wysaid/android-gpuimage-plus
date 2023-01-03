/*
** Copyright (c) 2013-2016 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
/*
** This header is generated from the Khronos OpenGL / OpenGL ES XML
** API Registry. The current version of the Registry, generator scripts
** used to make the header, and the header can be found at
**   http://www.opengl.org/registry/
**
** Khronos $Revision$ on $Date$
*/

#include "cgeGL31Stub.h"

#include <EGL/egl.h>

GLboolean cgeGl31StubInit()
{
#define FIND_PROC(s) s = (void*)eglGetProcAddress(#s)
    FIND_PROC(glDispatchCompute);
    FIND_PROC(glDispatchComputeIndirect);
    FIND_PROC(glDrawArraysIndirect);
    FIND_PROC(glDrawElementsIndirect);
    FIND_PROC(glFramebufferParameteri);
    FIND_PROC(glGetFramebufferParameteriv);
    FIND_PROC(glGetProgramInterfaceiv);
    FIND_PROC(glGetProgramResourceIndex);
    FIND_PROC(glGetProgramResourceName);
    FIND_PROC(glGetProgramResourceiv);
    FIND_PROC(glGetProgramResourceLocation);
    FIND_PROC(glUseProgramStages);
    FIND_PROC(glActiveShaderProgram);
    FIND_PROC(glCreateShaderProgramv);
    FIND_PROC(glBindProgramPipeline);
    FIND_PROC(glDeleteProgramPipelines);
    FIND_PROC(glGenProgramPipelines);
    FIND_PROC(glIsProgramPipeline);
    FIND_PROC(glGetProgramPipelineiv);
    FIND_PROC(glProgramUniform1i);
    FIND_PROC(glProgramUniform2i);
    FIND_PROC(glProgramUniform3i);
    FIND_PROC(glProgramUniform4i);
    FIND_PROC(glProgramUniform1ui);
    FIND_PROC(glProgramUniform2ui);
    FIND_PROC(glProgramUniform3ui);
    FIND_PROC(glProgramUniform4ui);
    FIND_PROC(glProgramUniform1f);
    FIND_PROC(glProgramUniform2f);
    FIND_PROC(glProgramUniform3f);
    FIND_PROC(glProgramUniform4f);
    FIND_PROC(glProgramUniform1iv);
    FIND_PROC(glProgramUniform2iv);
    FIND_PROC(glProgramUniform3iv);
    FIND_PROC(glProgramUniform4iv);
    FIND_PROC(glProgramUniform1uiv);
    FIND_PROC(glProgramUniform2uiv);
    FIND_PROC(glProgramUniform3uiv);
    FIND_PROC(glProgramUniform4uiv);
    FIND_PROC(glProgramUniform1fv);
    FIND_PROC(glProgramUniform2fv);
    FIND_PROC(glProgramUniform3fv);
    FIND_PROC(glProgramUniform4fv);
    FIND_PROC(glProgramUniformMatrix2fv);
    FIND_PROC(glProgramUniformMatrix3fv);
    FIND_PROC(glProgramUniformMatrix4fv);
    FIND_PROC(glProgramUniformMatrix2x3fv);
    FIND_PROC(glProgramUniformMatrix3x2fv);
    FIND_PROC(glProgramUniformMatrix2x4fv);
    FIND_PROC(glProgramUniformMatrix4x2fv);
    FIND_PROC(glProgramUniformMatrix3x4fv);
    FIND_PROC(glProgramUniformMatrix4x3fv);
    FIND_PROC(glValidateProgramPipeline);
    FIND_PROC(glGetProgramPipelineInfoLog);
    FIND_PROC(glBindImageTexture);
    FIND_PROC(glGetBooleani_v);
    FIND_PROC(glMemoryBarrier);
    FIND_PROC(glMemoryBarrierByRegion);
    FIND_PROC(glTexStorage2DMultisample);
    FIND_PROC(glGetMultisamplefv);
    FIND_PROC(glSampleMaski);
    FIND_PROC(glGetTexLevelParameteriv);
    FIND_PROC(glGetTexLevelParameterfv);
    FIND_PROC(glBindVertexBuffer);
    FIND_PROC(glVertexAttribFormat);
    FIND_PROC(glVertexAttribIFormat);
    FIND_PROC(glVertexAttribBinding);
    FIND_PROC(glVertexBindingDivisor);
#undef FIND_PROC

    if (!glDispatchCompute ||
        !glDispatchComputeIndirect ||
        !glDrawArraysIndirect ||
        !glDrawElementsIndirect ||
        !glFramebufferParameteri ||
        !glGetFramebufferParameteriv ||
        !glGetProgramInterfaceiv ||
        !glGetProgramResourceIndex ||
        !glGetProgramResourceName ||
        !glGetProgramResourceiv ||
        !glGetProgramResourceLocation ||
        !glUseProgramStages ||
        !glActiveShaderProgram ||
        !glCreateShaderProgramv ||
        !glBindProgramPipeline ||
        !glDeleteProgramPipelines ||
        !glGenProgramPipelines ||
        !glIsProgramPipeline ||
        !glGetProgramPipelineiv ||
        !glProgramUniform1i ||
        !glProgramUniform2i ||
        !glProgramUniform3i ||
        !glProgramUniform4i ||
        !glProgramUniform1ui ||
        !glProgramUniform2ui ||
        !glProgramUniform3ui ||
        !glProgramUniform4ui ||
        !glProgramUniform1f ||
        !glProgramUniform2f ||
        !glProgramUniform3f ||
        !glProgramUniform4f ||
        !glProgramUniform1iv ||
        !glProgramUniform2iv ||
        !glProgramUniform3iv ||
        !glProgramUniform4iv ||
        !glProgramUniform1uiv ||
        !glProgramUniform2uiv ||
        !glProgramUniform3uiv ||
        !glProgramUniform4uiv ||
        !glProgramUniform1fv ||
        !glProgramUniform2fv ||
        !glProgramUniform3fv ||
        !glProgramUniform4fv ||
        !glProgramUniformMatrix2fv ||
        !glProgramUniformMatrix3fv ||
        !glProgramUniformMatrix4fv ||
        !glProgramUniformMatrix2x3fv ||
        !glProgramUniformMatrix3x2fv ||
        !glProgramUniformMatrix2x4fv ||
        !glProgramUniformMatrix4x2fv ||
        !glProgramUniformMatrix3x4fv ||
        !glProgramUniformMatrix4x3fv ||
        !glValidateProgramPipeline ||
        !glGetProgramPipelineInfoLog ||
        !glBindImageTexture ||
        !glGetBooleani_v ||
        !glMemoryBarrier ||
        !glMemoryBarrierByRegion ||
        !glTexStorage2DMultisample ||
        !glGetMultisamplefv ||
        !glSampleMaski ||
        !glGetTexLevelParameteriv ||
        !glGetTexLevelParameterfv ||
        !glBindVertexBuffer ||
        !glVertexAttribFormat ||
        !glVertexAttribIFormat ||
        !glVertexAttribBinding ||
        !glVertexBindingDivisor)
    {
        return GL_FALSE;
    }

    return GL_TRUE;
}

GL_APICALL void (*GL_APIENTRY glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
GL_APICALL void (*GL_APIENTRY glDispatchComputeIndirect)(GLintptr indirect);
GL_APICALL void (*GL_APIENTRY glDrawArraysIndirect)(GLenum mode, const void* indirect);
GL_APICALL void (*GL_APIENTRY glDrawElementsIndirect)(GLenum mode, GLenum type, const void* indirect);
GL_APICALL void (*GL_APIENTRY glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
GL_APICALL void (*GL_APIENTRY glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint* params);
GL_APICALL void (*GL_APIENTRY glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint* params);
GL_APICALL GLuint (*GL_APIENTRY glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar* name);
GL_APICALL void (*GL_APIENTRY glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
GL_APICALL void (*GL_APIENTRY glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params);
GL_APICALL GLint (*GL_APIENTRY glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar* name);
GL_APICALL void (*GL_APIENTRY glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
GL_APICALL void (*GL_APIENTRY glActiveShaderProgram)(GLuint pipeline, GLuint program);
GL_APICALL GLuint (*GL_APIENTRY glCreateShaderProgramv)(GLenum type, GLsizei count, const GLchar* const* strings);
GL_APICALL void (*GL_APIENTRY glBindProgramPipeline)(GLuint pipeline);
GL_APICALL void (*GL_APIENTRY glDeleteProgramPipelines)(GLsizei n, const GLuint* pipelines);
GL_APICALL void (*GL_APIENTRY glGenProgramPipelines)(GLsizei n, GLuint* pipelines);
GL_APICALL GLboolean (*GL_APIENTRY glIsProgramPipeline)(GLuint pipeline);
GL_APICALL void (*GL_APIENTRY glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint* params);
GL_APICALL void (*GL_APIENTRY glProgramUniform1i)(GLuint program, GLint location, GLint v0);
GL_APICALL void (*GL_APIENTRY glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
GL_APICALL void (*GL_APIENTRY glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
GL_APICALL void (*GL_APIENTRY glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
GL_APICALL void (*GL_APIENTRY glProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
GL_APICALL void (*GL_APIENTRY glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
GL_APICALL void (*GL_APIENTRY glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
GL_APICALL void (*GL_APIENTRY glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
GL_APICALL void (*GL_APIENTRY glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
GL_APICALL void (*GL_APIENTRY glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
GL_APICALL void (*GL_APIENTRY glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GL_APICALL void (*GL_APIENTRY glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GL_APICALL void (*GL_APIENTRY glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void (*GL_APIENTRY glValidateProgramPipeline)(GLuint pipeline);
GL_APICALL void (*GL_APIENTRY glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
GL_APICALL void (*GL_APIENTRY glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
GL_APICALL void (*GL_APIENTRY glGetBooleani_v)(GLenum target, GLuint index, GLboolean* data);
GL_APICALL void (*GL_APIENTRY glMemoryBarrier)(GLbitfield barriers);
GL_APICALL void (*GL_APIENTRY glMemoryBarrierByRegion)(GLbitfield barriers);
GL_APICALL void (*GL_APIENTRY glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
GL_APICALL void (*GL_APIENTRY glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat* val);
GL_APICALL void (*GL_APIENTRY glSampleMaski)(GLuint maskNumber, GLbitfield mask);
GL_APICALL void (*GL_APIENTRY glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint* params);
GL_APICALL void (*GL_APIENTRY glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat* params);
GL_APICALL void (*GL_APIENTRY glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
GL_APICALL void (*GL_APIENTRY glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
GL_APICALL void (*GL_APIENTRY glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
GL_APICALL void (*GL_APIENTRY glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
GL_APICALL void (*GL_APIENTRY glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);
