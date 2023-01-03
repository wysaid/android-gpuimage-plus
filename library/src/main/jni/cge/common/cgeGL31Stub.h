#ifndef __gl31_h_
#define __gl31_h_ 1

#include <GLES2/gl2.h>
#include <android/api-level.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

#include <GLES3/gl3platform.h>

GLboolean cgeGl31StubInit();

#ifndef GL_ES_VERSION_3_1
#define GL_ES_VERSION_3_1 1
#define GL_COMPUTE_SHADER 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS 0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS 0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS 0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT 0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE 0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE 0x8267
#define GL_DISPATCH_INDIRECT_BUFFER 0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT 0x00000020
#define GL_DRAW_INDIRECT_BUFFER 0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING 0x8F43
#define GL_MAX_UNIFORM_LOCATIONS 0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH 0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT 0x9311
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES 0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH 0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT 0x9316
#define GL_MAX_FRAMEBUFFER_SAMPLES 0x9318
#define GL_UNIFORM 0x92E1
#define GL_UNIFORM_BLOCK 0x92E2
#define GL_PROGRAM_INPUT 0x92E3
#define GL_PROGRAM_OUTPUT 0x92E4
#define GL_BUFFER_VARIABLE 0x92E5
#define GL_SHADER_STORAGE_BLOCK 0x92E6
#define GL_ATOMIC_COUNTER_BUFFER 0x92C0
#define GL_TRANSFORM_FEEDBACK_VARYING 0x92F4
#define GL_ACTIVE_RESOURCES 0x92F5
#define GL_MAX_NAME_LENGTH 0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES 0x92F7
#define GL_NAME_LENGTH 0x92F9
#define GL_TYPE 0x92FA
#define GL_ARRAY_SIZE 0x92FB
#define GL_OFFSET 0x92FC
#define GL_BLOCK_INDEX 0x92FD
#define GL_ARRAY_STRIDE 0x92FE
#define GL_MATRIX_STRIDE 0x92FF
#define GL_IS_ROW_MAJOR 0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX 0x9301
#define GL_BUFFER_BINDING 0x9302
#define GL_BUFFER_DATA_SIZE 0x9303
#define GL_NUM_ACTIVE_VARIABLES 0x9304
#define GL_ACTIVE_VARIABLES 0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER 0x9306
#define GL_REFERENCED_BY_FRAGMENT_SHADER 0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER 0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE 0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE 0x930D
#define GL_LOCATION 0x930E
#define GL_VERTEX_SHADER_BIT 0x00000001
#define GL_FRAGMENT_SHADER_BIT 0x00000002
#define GL_ALL_SHADER_BITS 0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE 0x8258
#define GL_ACTIVE_PROGRAM 0x8259
#define GL_PROGRAM_PIPELINE_BINDING 0x825A
#define GL_ATOMIC_COUNTER_BUFFER_BINDING 0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START 0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE 0x92C3
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS 0x92D2
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS 0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS 0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS 0x92D9
#define GL_UNSIGNED_INT_ATOMIC_COUNTER 0x92DB
#define GL_MAX_IMAGE_UNITS 0x8F38
#define GL_MAX_VERTEX_IMAGE_UNIFORMS 0x90CA
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS 0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS 0x90CF
#define GL_IMAGE_BINDING_NAME 0x8F3A
#define GL_IMAGE_BINDING_LEVEL 0x8F3B
#define GL_IMAGE_BINDING_LAYERED 0x8F3C
#define GL_IMAGE_BINDING_LAYER 0x8F3D
#define GL_IMAGE_BINDING_ACCESS 0x8F3E
#define GL_IMAGE_BINDING_FORMAT 0x906E
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT 0x00000002
#define GL_UNIFORM_BARRIER_BIT 0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT 0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT 0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT 0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT 0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT 0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT 0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT 0x00001000
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_IMAGE_2D 0x904D
#define GL_IMAGE_3D 0x904E
#define GL_IMAGE_CUBE 0x9050
#define GL_IMAGE_2D_ARRAY 0x9053
#define GL_INT_IMAGE_2D 0x9058
#define GL_INT_IMAGE_3D 0x9059
#define GL_INT_IMAGE_CUBE 0x905B
#define GL_INT_IMAGE_2D_ARRAY 0x905E
#define GL_UNSIGNED_INT_IMAGE_2D 0x9063
#define GL_UNSIGNED_INT_IMAGE_3D 0x9064
#define GL_UNSIGNED_INT_IMAGE_CUBE 0x9066
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY 0x9069
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING 0x90D3
#define GL_SHADER_STORAGE_BUFFER_START 0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE 0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE 0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE 0x90EA
#define GL_STENCIL_INDEX 0x1901
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_SAMPLE_POSITION 0x8E50
#define GL_SAMPLE_MASK 0x8E51
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_VERTEX_ATTRIB_BINDING 0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D5
#define GL_VERTEX_BINDING_DIVISOR 0x82D6
#define GL_VERTEX_BINDING_OFFSET 0x82D7
#define GL_VERTEX_BINDING_STRIDE 0x82D8
#define GL_VERTEX_BINDING_BUFFER 0x8F4F
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS 0x82DA
#define GL_MAX_VERTEX_ATTRIB_STRIDE 0x82E5

extern GL_APICALL void (*GL_APIENTRY glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
extern GL_APICALL void (*GL_APIENTRY glDispatchComputeIndirect)(GLintptr indirect);
extern GL_APICALL void (*GL_APIENTRY glDrawArraysIndirect)(GLenum mode, const void* indirect);
extern GL_APICALL void (*GL_APIENTRY glDrawElementsIndirect)(GLenum mode, GLenum type, const void* indirect);
extern GL_APICALL void (*GL_APIENTRY glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
extern GL_APICALL void (*GL_APIENTRY glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint* params);
extern GL_APICALL void (*GL_APIENTRY glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint* params);
extern GL_APICALL GLuint (*GL_APIENTRY glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar* name);
extern GL_APICALL void (*GL_APIENTRY glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize,
                                                               GLsizei* length, GLchar* name);
extern GL_APICALL void (*GL_APIENTRY glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount,
                                                             const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params);
extern GL_APICALL GLint (*GL_APIENTRY glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar* name);
extern GL_APICALL void (*GL_APIENTRY glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
extern GL_APICALL void (*GL_APIENTRY glActiveShaderProgram)(GLuint pipeline, GLuint program);
extern GL_APICALL GLuint (*GL_APIENTRY glCreateShaderProgramv)(GLenum type, GLsizei count, const GLchar* const* strings);
extern GL_APICALL void (*GL_APIENTRY glBindProgramPipeline)(GLuint pipeline);
extern GL_APICALL void (*GL_APIENTRY glDeleteProgramPipelines)(GLsizei n, const GLuint* pipelines);
extern GL_APICALL void (*GL_APIENTRY glGenProgramPipelines)(GLsizei n, GLuint* pipelines);
extern GL_APICALL GLboolean (*GL_APIENTRY glIsProgramPipeline)(GLuint pipeline);
extern GL_APICALL void (*GL_APIENTRY glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint* params);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1i)(GLuint program, GLint location, GLint v0);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose,
                                                                  const GLfloat* value);
extern GL_APICALL void (*GL_APIENTRY glValidateProgramPipeline)(GLuint pipeline);
extern GL_APICALL void (*GL_APIENTRY glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern GL_APICALL void (*GL_APIENTRY glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer,
                                                         GLenum access, GLenum format);
extern GL_APICALL void (*GL_APIENTRY glGetBooleani_v)(GLenum target, GLuint index, GLboolean* data);
extern GL_APICALL void (*GL_APIENTRY glMemoryBarrier)(GLbitfield barriers);
extern GL_APICALL void (*GL_APIENTRY glMemoryBarrierByRegion)(GLbitfield barriers);
extern GL_APICALL void (*GL_APIENTRY glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width,
                                                                GLsizei height, GLboolean fixedsamplelocations);
extern GL_APICALL void (*GL_APIENTRY glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat* val);
extern GL_APICALL void (*GL_APIENTRY glSampleMaski)(GLuint maskNumber, GLbitfield mask);
extern GL_APICALL void (*GL_APIENTRY glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint* params);
extern GL_APICALL void (*GL_APIENTRY glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat* params);
extern GL_APICALL void (*GL_APIENTRY glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
extern GL_APICALL void (*GL_APIENTRY glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
                                                           GLuint relativeoffset);
extern GL_APICALL void (*GL_APIENTRY glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
extern GL_APICALL void (*GL_APIENTRY glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
extern GL_APICALL void (*GL_APIENTRY glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);

#endif /* GL_ES_VERSION_3_1 */

#ifdef __cplusplus
}
#endif

#endif
