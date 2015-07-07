/*
 * cgePainter.cpp
 *
 *  Created on: 2014-3-7
 *      Author: Wang Yang
 *        Blog: http://wysaid.org
*/

#include "cgePainter.h"
#include "cgePainterDataset.h"
#include <cstdlib>

#define TEXTURE_BRUSH_STYLE CGE_TEXTURE_START
#define TEXTURE_BRUSH_STYLE_INDEX CGE_TEXTURE_START_INDEX

static CGEConstString s_vshPaint = CGE_SHADER_STRING
(
attribute vec4 vPosition;
uniform mat4 uMat;
uniform float pointSize;

void main()
{
	gl_Position = (uMat * vPosition);
	gl_PointSize = pointSize;
}
);

static CGEConstString s_fshPaint = CGE_SHADER_STRING_PRECISION_M
(
uniform sampler2D brushTexture;
uniform vec4 vColor;

void main()
{
	gl_FragColor = vColor * texture2D(brushTexture, gl_PointCoord);
}
);

// static CGEConstString s_fshPaintEraser = SHADER_STRING_PRECISION_M
// (
// uniform sampler2D brushTexture;
// uniform vec4 vColor;
// 
// void main()
// {
// 	float alpha = texture2D(brushTexture, gl_PointCoord).a * vColor.a;
// 	gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);
// }
// );

static CGEConstString s_vshPaintRotWithRad = CGE_SHADER_STRING
(
attribute vec4 vPosition;
attribute float pointRadian;

uniform mat4 uMat;
uniform float pointSize;
varying mat2 mRot;
varying float pointScale;

const vec2 corner = vec2(0.5, 0.5);

mat2 makeRotation(float rad)
{
	float cosRad = cos(rad);
	float sinRad = sin(rad);
	return mat2(cosRad, sinRad, -sinRad, cosRad);
}

void main()
{
	mRot = makeRotation(pointRadian);
	vec2 v2 = abs(corner * mRot);
	pointScale = max(v2.x, v2.y) / 0.5;
	gl_PointSize = pointSize * pointScale;
	gl_Position = (uMat * vPosition);
}
);

static CGEConstString s_vshPaintRotWithNorm = CGE_SHADER_STRING
(
attribute vec4 vPosition;
attribute vec2 vPointNorm;

uniform mat4 uMat;
uniform float pointSize;
varying mat2 mRot;
varying float pointScale;

const vec2 corner = vec2(0.5, 0.5);

mat2 makeRotation(vec2 norm)
{
	float len = length(norm);
	if(len == 0.0)
		return mat2(1.0, 0.0, 0.0, 1.0);
	float cosRad = norm.x / len;
	float sinRad = - norm.y / len;
	return mat2(cosRad, sinRad, -sinRad, cosRad);
}

void main()
{
	mRot = makeRotation(vPointNorm);
	vec2 v2 = abs(corner * mRot);
	pointScale = max(v2.x, v2.y) / 0.5;
	gl_PointSize = pointSize * pointScale;
	gl_Position = (uMat * vPosition);
}
);

static CGEConstString s_fshPaintRot = CGE_SHADER_STRING_PRECISION_M
(
uniform sampler2D brushTexture;
uniform vec4 vColor;
varying mat2 mRot;
varying float pointScale;

void main()
{
	gl_FragColor = vColor * texture2D(brushTexture, (gl_PointCoord - 0.5) * pointScale * mRot + 0.5);
}
);

static CGEConstString s_fshPaintRotMulAlpha = CGE_SHADER_STRING_PRECISION_M
(
uniform sampler2D brushTexture;
uniform vec4 vColor;
varying mat2 mRot;
varying float pointScale;

void main()
{
	gl_FragColor = vColor * texture2D(brushTexture, (gl_PointCoord - 0.5) * pointScale * mRot + 0.5);
	gl_FragColor.rgb *= gl_FragColor.a;
}
);

static CGEConstString s_fshPainterMosaicMode = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D blendTexture;
uniform vec2 samplerSteps;
uniform float intensity;
uniform float blurPixels;

void main()
{
	vec2 coord = floor(textureCoordinate / samplerSteps / blurPixels) * samplerSteps * blurPixels;
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 mosaic = texture2D(inputImageTexture, coord + samplerSteps * 0.5);
	float alpha = texture2D(blendTexture, textureCoordinate).a;
	gl_FragColor = mix(src, mosaic, alpha * intensity);
}
);

const GLfloat* getDraw2ScreenPosVertices()
{
	static const GLfloat vertices[] = 
	{
		-1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, -1.0f
	};
	return vertices;
}

namespace CGE
{
	GLuint GPUPainterTextureManager::painterBrushTex[PathAttrib::MAX_BRUSH_STYLE_NUM] = {0u};
	CGEBufferLoadFun GPUPainterTextureManager::texLoadFunc = NULL;
	CGEBufferUnloadFun GPUPainterTextureManager::texUnloadFunc = NULL;
	void *GPUPainterTextureManager::texLoadParam = NULL, *GPUPainterTextureManager::texUnLoadParam = NULL;

	GLuint GPUPainterTextureManager::getTextureByBrushStyle(PathAttrib::BrushStyle style)
	{
		CGE_LOG_CODE(
		if(style < 0 || style > PathAttrib::MAX_BRUSH_STYLE_NUM)
		{
			CGE_LOG_ERROR("Invalid Brush Style!");
			return 0;
		});

		GLuint& texID = painterBrushTex[style];
		if(texID == 0)
		{
			if(style == PathAttrib::BRUSH_DEFAULT)
			{
				char buffer[128*128*4] = {0};
				//memset(buffer, 0, 128*128*4);
				for(int i = 0; i < 128*128; ++i)
				{
					const float dx = float(i % 128 - 64);
					const float dy = float(i / 128 - 64);
					const float alpha1 = 1.0f - (dx*dx + dy*dy) / (64.0f*64.0f*2.0f);
					const float alpha2 = alpha1 * alpha1;
					const float alpha3 = alpha2 * alpha2;
					const float alpha4 = alpha3 * alpha3;
					const char alpha = (char)(alpha4 * 255.0f);
					buffer[i*4] = alpha;
					buffer[i*4 + 1] = alpha;
					buffer[i*4 + 2] = alpha;
					buffer[i*4 + 3] = alpha;
				}
				texID = cgeGenTextureWithBuffer(buffer, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE);
			}
			else
				texID = loadBrushTexture(style);
		}
		return texID;
	}

	void GPUPainterTextureManager::clearTextures()
	{
		for(int i = 0; i != PathAttrib::MAX_BRUSH_STYLE_NUM; ++i)
		{
			glDeleteTextures(1, GPUPainterTextureManager::painterBrushTex + i);
			GPUPainterTextureManager::painterBrushTex[i] = 0;
		}
	}

	GLuint GPUPainterTextureManager::loadBrushTexture(PathAttrib::BrushStyle style)
	{
		CGEBufferLoadFun loadFunc = texLoadFunc;
		CGEBufferUnloadFun unloadFunc = texUnloadFunc;
		void *loadParam = texLoadParam, *unloadParam = texUnLoadParam;
		if(loadFunc == NULL)
		{
			loadFunc = cgeGetCommonLoadFunc();
			loadParam = cgeGetCommonLoadArg();
			unloadFunc = cgeGetCommonUnloadFunc();
			unloadParam = cgeGetCommonUnloadArg();
		}
		const char* name = getBrushTextureName(style);
		if(name == NULL || loadFunc == NULL)
			return 0;
		void* buffer = NULL;
		int w, h;
		CGEBufferFormat fmt;
		loadFunc(name, &buffer, &w, &h, &fmt, loadParam);
		GLenum channelFmt, dataFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, NULL);
		GLuint texID = cgeGenTextureWithBuffer(buffer, w, h, channelFmt,dataFmt);
		if(unloadFunc != NULL)
			unloadFunc(buffer, unloadParam);
		return texID;
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString GPUPainterLayer::paramDraw2ScreenTextureName = paramInputImageName;
	CGEConstString GPUPainterLayer::paramPointSizeName = "pointSize";
	CGEConstString GPUPainterLayer::paramColorOpacityName = "vColor";
	CGEConstString GPUPainterLayer::paramRotationRadName = "pointRadian";
	CGEConstString GPUPainterLayer::paramRotationNormalName = "vPointNorm";
	CGEConstString GPUPainterLayer::paramBrushTextureName = "brushTexture";
	CGEConstString GPUPainterLayer::paramModelViewProjectionName = "uMat";
	CGEConstString GPUPainterLayer::paramBlendIntensityName = "intensity";
	CGEConstString GPUPainterLayer::paramBlendTextureName = "blendTexture";

	GPUPainterLayer::GPUPainterLayer() : m_draw2ScreenProgram(NULL), m_canvasTex(0), m_pathIndex(0), m_blendMode(CGE_BLEND_MIX), m_opacity(1.0f), m_blendProgram(NULL), m_canvasSize(0.0f, 0.0f)
	{
		
		glGenFramebuffers(1, &m_frameBuffer);
	}

	GPUPainterLayer::~GPUPainterLayer()
	{
		
		glDeleteTextures(1, &m_canvasTex);

		glDeleteFramebuffers(1, &m_frameBuffer);
        delete m_blendProgram;
		delete m_draw2ScreenProgram;
	}

	void GPUPainterLayer::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		usePainterBlendMode();
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);

		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_blendProgram->sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);

		glActiveTexture(CGE_TEXTURE_START);
		glBindTexture(GL_TEXTURE_2D, m_canvasTex);
		m_blendProgram->sendUniformi(paramBlendTextureName, CGE_TEXTURE_START_INDEX);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	bool GPUPainterLayer::init()
	{
		if(!m_paintDefaultProgram.initWithShaderStrings(s_vshPaint, s_fshPaint))
		{
			CGE_LOG_ERROR("GPUPainter::m_paintDefaultProgram init Failed!");
			return false;
		}

		if(!m_paintRotWithNormProgram.initWithShaderStrings(s_vshPaintRotWithNorm, s_fshPaintRot))
		{
			CGE_LOG_ERROR("GPUPainter::m_paintRotWithNormProgram init Failed!");
			return false;
		}

		if(!m_paintRotWithRadProgram.initWithShaderStrings(s_vshPaintRotWithRad, s_fshPaintRot))
		{
			CGE_LOG_ERROR("GPUPainter::m_paintRotWithRadProgram init Failed!");
			return false;
		}

		return true;
	}


	bool GPUPainterLayer::resizeCanvas(int width, int height, const void* buffer, CGEBufferFormat format)
	{
		
		if(m_canvasSize[0] == width && m_canvasSize[1] == height && buffer == NULL && m_canvasTex != 0)
			return true;
		m_canvasSize[0] = (float)width;
		m_canvasSize[1] = (float)height;
		if(m_frameBuffer == 0)
			glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glDeleteTextures(1, &m_canvasTex);
		GLenum channelFmt, dataFmt;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, NULL);
		m_canvasTex = cgeGenTextureWithBuffer(buffer, (GLint)width, (GLint)height, channelFmt, dataFmt);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_canvasTex, 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteFramebuffers(1, &m_frameBuffer);
			glDeleteTextures(1, &m_canvasTex);
			m_canvasTex = 0;
			m_frameBuffer = 0;
			CGE_LOG_ERROR("GPUPainter::resizeCanvas create frame buffer failed!");
			return false;
		}
		m_mvp = Mat4::makeOrtho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
		m_paintRotWithNormProgram.bind();
		glUniformMatrix4fv(m_paintRotWithNormProgram.uniformLocation(paramModelViewProjectionName), 1, GL_FALSE, m_mvp[0]);

		m_paintDefaultProgram.bind();
		glUniformMatrix4fv(m_paintDefaultProgram.uniformLocation(paramModelViewProjectionName), 1, GL_FALSE, m_mvp[0]);

		m_paintRotWithRadProgram.bind();
		glUniformMatrix4fv(m_paintRotWithRadProgram.uniformLocation(paramModelViewProjectionName), 1, GL_FALSE, m_mvp[0]);

		redrawPathToIndex();
		return true;
	}

	bool GPUPainterLayer::drawResultToScreen(int x, int y, int w, int h)
	{
		
		glDisable(GL_BLEND);
		glViewport(x, y, w, h);
		if(m_draw2ScreenProgram == NULL)
		{
			m_draw2ScreenProgram = new ProgramObject;
			if(!m_draw2ScreenProgram->initWithShaderStrings(g_vshDrawToScreen, g_fshDefault))
			{
				CGE_LOG_ERROR("GPUPainter::drawResultToScreen create cache program failed!");
				return false;
			}
		}

		m_draw2ScreenProgram->bind();
		GLuint vPosition = m_draw2ScreenProgram->attributeLocation(paramPositionIndexName);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, getDraw2ScreenPosVertices());
		glEnableVertexAttribArray(vPosition);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, m_canvasTex);
		m_draw2ScreenProgram->sendUniformi(paramDraw2ScreenTextureName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		return true;
	}

	void GPUPainterLayer::drawPath(PathAttrib& attrib)
	{
		switch (attrib.kind)
		{
		case PathAttrib::PATH_NORMAL:
			drawPathNormal(attrib);
			break;
		case PathAttrib::PATH_HAS_DIRECTION:
			drawPathWithDirection(attrib);
			break;
		case PathAttrib::PATH_POINT_ROT_RANDOM:
			drawPathRandomRotation(attrib);
			break;
		case PathAttrib::PATH_ERASER:
			drawPathEraser(attrib);
			break;
		default:
			CGE_LOG_ERROR("Invalid PathArrib Kind!");
			return;
		}
	}

	void GPUPainterLayer::drawPathNormal(PathAttrib& attrib)
	{
		if(attrib.vecPoints.empty())
			return;
		setPaintEnv();
		ProgramObject& attribProgram = setPainterAttrib(attrib);
		std::vector<Vec2f> vecPoints;
		genInterpolationPoints(attrib.vecPoints, vecPoints, m_canvasSize, attrib.pointStride);
		std::vector<Vec2f>::size_type sz = vecPoints.size();
		if(sz < 1) return;
		GLuint vPosition = attribProgram.attributeLocation(paramPositionIndexName);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vecPoints.data());
		glEnableVertexAttribArray(vPosition);
		glDrawArrays(GL_POINTS, 0, (GLsizei)sz);
		CGE_LOG_INFO("Draw Points: %lu\n", sz);
		cgeCheckGLError("GPUPainter::drawPathNormal::glDrawArrays");
	}

	void GPUPainterLayer::drawPathWithDirection(PathAttrib& attrib)
	{
		if(attrib.vecPoints.size() <= 1)
		{
			drawPathNormal(attrib);
			return;
		}
		setPaintEnv();
		ProgramObject& attribProgram = setPainterAttrib(attrib);

		std::vector<Vec2f> vecPoints, vecDirections;
		genInterpolationPointsAndDirections(attrib.vecPoints, vecPoints, vecDirections, m_canvasSize, attrib.pointStride);
		const std::vector<Vec2f>::size_type sz = vecPoints.size();
		
		if(sz < 1 || sz != vecDirections.size())
		{
			CGE_LOG_ERROR("Error occurred!");
			return;
		}

		GLuint vPosition = attribProgram.attributeLocation(paramPositionIndexName);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vecPoints.data());
		glEnableVertexAttribArray(vPosition);

		GLuint vPointNorm = attribProgram.attributeLocation(paramRotationNormalName);
		glVertexAttribPointer(vPointNorm, 2, GL_FLOAT, GL_FALSE, 0, vecDirections.data());
		glEnableVertexAttribArray(vPointNorm);

		glDrawArrays(GL_POINTS, 0, (GLsizei)sz);
		CGE_LOG_INFO("Draw Points: %lu\n", sz);
		cgeCheckGLError("GPUPainter::drawPathWithDirection::glDrawArrays");
	}

	void GPUPainterLayer::drawPathRandomRotation(PathAttrib& attrib)
	{
		if(attrib.vecPoints.empty())
			return;
		setPaintEnv();

		ProgramObject& attribProgram = setPainterAttrib(attrib);

		std::vector<Vec2f> vecPoints;
		std::vector<Vec1f> vecDirections;
		
		genInterpolationPointsAndRandomDir(attrib.vecPoints, vecPoints, vecDirections, m_canvasSize, attrib.pointStride);
		const std::vector<Vec2f>::size_type sz = vecPoints.size();

		if(sz < 1 || sz != vecDirections.size())
		{
			CGE_LOG_ERROR("Error occurred!");
			return;
		}

		GLuint vPosition = attribProgram.attributeLocation(paramPositionIndexName);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vecPoints.data());
		glEnableVertexAttribArray(vPosition);

		GLuint vPointNorm = attribProgram.attributeLocation(paramRotationRadName);
		glVertexAttribPointer(vPointNorm, 1, GL_FLOAT, GL_FALSE, 0, vecDirections.data());
		glEnableVertexAttribArray(vPointNorm);

		glDrawArrays(GL_POINTS, 0, (GLsizei)sz);
		CGE_LOG_INFO("Draw Points: %lu\n", sz);
		cgeCheckGLError("GPUPainter::drawPathRandomRotation::glDrawArrays");
	}

	void GPUPainterLayer::drawPathEraser(PathAttrib& attrib)
	{
		if(attrib.vecPoints.empty())
			return;
		
#ifdef GL_VERTEX_PROGRAM_POINT_SIZE
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

#ifdef GL_POINT_SPRITE
		glEnable(GL_POINT_SPRITE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, (GLsizei)m_canvasSize[0], (GLsizei)m_canvasSize[1]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		setPointSize(m_paintDefaultProgram, attrib.pointSize);
		setColorAndOpacity(m_paintDefaultProgram, 0.0f, 0.0f, 0.0f, attrib.opacity);
		setBrushStyle(m_paintDefaultProgram, attrib.brushStyle);

		ProgramObject& attribProgram = m_paintDefaultProgram;
		std::vector<Vec2f> vecPoints;
		genInterpolationPoints(attrib.vecPoints, vecPoints, m_canvasSize, attrib.pointStride);
		std::vector<Vec2f>::size_type sz = vecPoints.size();
		if(sz < 1) return;
		GLuint vPosition = attribProgram.attributeLocation(paramPositionIndexName);
		glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vecPoints.data());
		glEnableVertexAttribArray(vPosition);
		glDrawArrays(GL_POINTS, 0, (GLsizei)sz);
		CGE_LOG_INFO("Draw Points: %lu\n", sz);
		cgeCheckGLError("GPUPainter::drawPathEraser::glDrawArrays");
	}
// 
// 	void GPUPainterLayer::drawPath(float startX, float startY, float endX, float endY)
// 	{
// 		Vec2f v1(startX, startY), v2(endX, endY);
// 		drawPath(v1, v2);
// 	}
// 
// 	void GPUPainterLayer::drawPath(Vec2f& start, Vec2f& end)
// 	{
// 		setPaintEnv();
// 		m_paintRotWithNormProgram.useProgram();
// 
// 	}

	void GPUPainterLayer::pushPath(PathAttrib& attrib)
	{
		clearPath(m_pathIndex);			
		m_vecPath.push_back(attrib);		
		m_pathIndex = m_vecPath.size();
	}

	bool GPUPainterLayer::undo()
	{
		if(!canUndo())
			return false;
		--m_pathIndex;
		redrawPathToIndex();
		return true;
	}

	bool GPUPainterLayer::canUndo() const
	{
		return !m_vecPath.empty() && m_pathIndex > 0;
	}

	bool GPUPainterLayer::redo()
	{
		if(!canRedo())
			return false;
		
		drawPathByIndex(m_pathIndex, m_pathIndex + 1);
		++m_pathIndex;
		return true;
	}

	bool GPUPainterLayer::canRedo() const
	{
		return !m_vecPath.empty() && m_pathIndex < m_vecPath.size();
	}

	void GPUPainterLayer::clearCanvas(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void GPUPainterLayer::redrawPathToIndex()
	{
		if(m_pathIndex > m_vecPath.size())
		{
			CGE_LOG_ERROR("Something Wrong.");
			m_pathIndex = m_vecPath.size();
		};

		clearCanvas(0.0f, 0.0f, 0.0f, 0.0f);

		for(std::vector<PathAttrib>::iterator iter = m_vecPath.begin(), iEnd = iter + m_pathIndex;
			iter != iEnd; ++iter)
		{
			drawPath(*iter);
		}
	}

	void GPUPainterLayer::drawPathByIndex(size_t indexStart, size_t indexEnd)
	{
		if(indexStart >= indexEnd || indexStart >= m_vecPath.size())
			return ;

		if(indexEnd > m_vecPath.size())
			indexEnd = m_vecPath.size();

		for(std::vector<PathAttrib>::iterator iter = m_vecPath.begin() + indexStart,
			iEnd = m_vecPath.begin() + indexEnd; iter != iEnd; ++iter)
		{
			drawPath(*iter);
		}
	}

	ProgramObject& GPUPainterLayer::setPainterAttrib(PathAttrib& attrib)
	{
		ProgramObject* program = NULL;

		switch (attrib.kind)
		{
		case PathAttrib::PATH_NORMAL:
			program = &m_paintDefaultProgram;
			break;
		case PathAttrib::PATH_HAS_DIRECTION:
			program = &m_paintRotWithNormProgram;
			break;
		case PathAttrib::PATH_POINT_ROT_RANDOM:
			program = &m_paintRotWithRadProgram;
			break;
		case PathAttrib::PATH_ERASER:
			CGE_LOG_ERROR("GPUPainterLayer::setPainterAttrib - Check your code! 'PATH_ERASER' has unique usage. These codes should never be reached!\n"); //Fall through
		default:
			CGE_LOG_ERROR("GPUPainterLayer::setPainterAttrib - Invalid attribute kind!");
			program = &m_paintDefaultProgram;
			break;;
		}
		
		//////////////////////////////////////////////////////////////////////////

		setPointSize(*program, attrib.pointSize);
		setColorAndOpacity(*program, attrib.r, attrib.g, attrib.b, attrib.opacity);
		setBrushStyle(*program, attrib.brushStyle);
		return *program;
	}

	void GPUPainterLayer::setPointSize(ProgramObject& obj, GLfloat pointSize)
	{
		obj.bind();
		obj.sendUniformf(paramPointSizeName, pointSize);
	}

	void GPUPainterLayer::setColorAndOpacity(ProgramObject& obj, GLfloat r, GLfloat g, GLfloat b, GLfloat opacity)
	{
		obj.bind();
		obj.sendUniformf(paramColorOpacityName, r, g, b, opacity);
	}

// 	void GPUPainterLayer::setRotation(ProgramObject& obj, GLfloat normX, GLfloat normY)
// 	{
// 		obj.bind();
// 		obj.sendUniformf(paramRotationNormalName, normX, normY);
// 	}

	void GPUPainterLayer::setBrushStyle(ProgramObject& obj, PathAttrib::BrushStyle style)
	{
		GLuint texID = GPUPainterTextureManager::getTextureByBrushStyle(style);
		obj.bind();
		glActiveTexture(TEXTURE_BRUSH_STYLE);
		glBindTexture(GL_TEXTURE_2D, texID);
		obj.sendUniformi(paramBrushTextureName, TEXTURE_BRUSH_STYLE_INDEX);
	}

	void GPUPainterLayer::genInterpolationPoints(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, Vec2f& canvasSize, GLfloat stride)
	{
		
		std::vector<Vec2f>::size_type sz = srcPoints.size();
		if(sz < 1 || stride < 1.0f)
		{
			CGE_LOG_ERROR("Invalid Parameters");
			return;
		}
		for(std::vector<Vec2f>::size_type t = 1; t < sz; ++t)
		{
			const Vec2f d = (srcPoints[t] - srcPoints[t - 1]) * canvasSize;
			const int count = (int)CGE_MAX(ceilf(d.length()) / stride, 1.0f);
			const Vec2f dd = d / (float)count;
			const Vec2f startPoint = srcPoints[t - 1] * canvasSize;
			for(int i = 0; i < count; ++i)
			{
				resultPoints.push_back(startPoint + dd * (float)i);
			}
		}
	}

	void GPUPainterLayer::genInterpolationPointsAndDirections(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, std::vector<Vec2f>& dirPoints, Vec2f& canvasSize, GLfloat stride)
	{
		
		std::vector<Vec2f>::size_type sz = srcPoints.size();
		if(sz < 1 || stride < 1.0f)
		{
			CGE_LOG_ERROR("Invalid Parameters");
			return;
		}

		for(std::vector<Vec2f>::size_type t = 1; t < sz; ++t)
		{
			const Vec2f d = (srcPoints[t] - srcPoints[t - 1]) * canvasSize;
			const int count = (int)CGE_MAX(ceilf(d.length()) / stride, 1.0f);
			const Vec2f dd = d / (float)count;
			const Vec2f startPoint = srcPoints[t - 1] * canvasSize;
			for(int i = 0; i < count; ++i)
			{
				resultPoints.push_back(startPoint + dd * (float)i);
			}
			dirPoints.insert(dirPoints.end(), count, d);
		}
	}

	void GPUPainterLayer::genInterpolationPointsAndRandomDir(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, std::vector<Vec1f>& dir, Vec2f& canvasSize, GLfloat stride)
	{
		
		std::vector<Vec2f>::size_type sz = srcPoints.size();
		if(sz < 1 || stride < 1.0f)
		{
			CGE_LOG_ERROR("Invalid Parameters");
			return;
		}

		for(std::vector<Vec2f>::size_type t = 1; t < sz; ++t)
		{
			const Vec2f d = (srcPoints[t] - srcPoints[t - 1]) * canvasSize;
			const int count = (int)CGE_MAX(ceilf(d.length()) / stride, 1.0f);
			const Vec2f dd = d / (float)count;
			const Vec2f startPoint = srcPoints[t - 1] * canvasSize;
			const float seed = (startPoint.x() + startPoint.y()) * float(RAND_MAX);
			srand((unsigned)seed);
			srand(rand());
			for(int i = 0; i < count; ++i)
			{
				resultPoints.push_back(startPoint + dd * (float)i);
				dir.push_back(rand() / (float)RAND_MAX * 3.1415927f);
			}
		}
	}

	float GPUPainterLayer::calcRadianByVec(Vec2f& start, Vec2f& end)
	{
		return calcRadianByVec(end[0] - start[0], end[1] - start[1]);
	}

	float GPUPainterLayer::calcRadianByVec(float x, float y)
	{
		const float PI = 3.14159265f;
		const float len = sqrtf(x*x + y*y);
		if(len < 0.01f) return 0.0f;
		const float angle = asinf(y / len);

		if(x > 0.0f)
		{
			if(y > 0.0f)
				return angle;
			return PI * 2 + angle;
		}
		return PI - angle;
	}

	void GPUPainterLayer::setPaintEnv()
	{
		
#ifdef GL_VERTEX_PROGRAM_POINT_SIZE
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

#ifdef GL_POINT_SPRITE
		glEnable(GL_POINT_SPRITE);
#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, (GLsizei)m_canvasSize[0], (GLsizei)m_canvasSize[1]);

		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	}

	bool GPUPainterLayer::usePainterBlendMode()
	{
		if(m_blendMode == m_programBlendMode && m_blendProgram != NULL)
		{
			m_blendProgram->bind();
			m_blendProgram->sendUniformf(paramBlendIntensityName, m_opacity);
			return true;
		}
		m_programBlendMode = m_blendMode;
		delete m_blendProgram;

		const char* fshFunc = CGEBlendInterface::getShaderFuncByBlendMode(m_programBlendMode);
		char buffer[2048];
		
		sprintf(buffer, CGEBlendInterface::getBlendWrapper(), fshFunc);

		m_blendProgram = new ProgramObject;
		if(!m_blendProgram->initWithShaderStrings(g_vshDefaultWithoutTexCoord, buffer))
		{
			CGE_LOG_ERROR("GPUPainter::usePainterBlendMode init program failed!");
			return false;
		}
		m_blendProgram->bind();
		m_blendProgram->sendUniformf(paramBlendIntensityName, m_opacity);
		return true;
	}

	void GPUPainterLayer::clearPath(size_t index)
	{
		if(index >= m_vecPath.size())
			return;
		m_vecPath.erase(m_vecPath.begin() + index, m_vecPath.end());
	}

	//////////////////////////////////////////////////////////////////////////

	bool GPUPainterLayerLighter::init()
	{
		

		if(!m_paintDefaultProgram.initWithShaderStrings(s_vshPaint, s_fshPaint))
		{
			CGE_LOG_ERROR("GPUPainterLayerLighter::m_paintDefaultProgram init Failed!");
			return false;
		}

		if(!m_paintRotWithNormProgram.initWithShaderStrings(s_vshPaintRotWithNorm, s_fshPaintRotMulAlpha))
		{
			CGE_LOG_ERROR("GPUPainterLayerLighter::m_paintRotWithNormProgram init Failed!");
			return false;
		}

		if(!m_paintRotWithRadProgram.initWithShaderStrings(s_vshPaintRotWithRad, s_fshPaintRotMulAlpha))
		{
			CGE_LOG_ERROR("GPUPainter::m_paintRotWithRadProgram init Failed!");
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString GPUPainterMosaicLayer::paramBlurRadiusName = "blurPixels";
	CGEConstString GPUPainterMosaicLayer::paramSamplerStepsName = "samplerSteps";

	bool GPUPainterMosaicLayer::init()
	{
		

		m_blendProgram = new ProgramObject;
		if(!m_blendProgram->initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fshPainterMosaicMode))
		{
			CGE_LOG_ERROR("GPUPainterMosaicLayer::usePainterBlendMode init program failed!");
			return false;
		}
		
		setBlurRadius(10.0f);
		m_programBlendMode = CGE_BLEND_TYPE_MAX_NUM;
		m_blendMode = CGE_BLEND_TYPE_MAX_NUM;
		return GPUPainterLayer::init();
	}

	void GPUPainterMosaicLayer::setBlurRadius(float value)
	{
		m_blendProgram->bind();
		m_blendProgram->sendUniformf(paramBlurRadiusName, value);
	}

	bool GPUPainterMosaicLayer::usePainterBlendMode()
	{
		if(m_blendProgram != NULL)
		{
			m_blendProgram->bind();
			m_blendProgram->sendUniformf(paramBlendIntensityName, m_opacity);
			return true;
		}
		
		m_blendProgram = new ProgramObject;
		if(!m_blendProgram->initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fshPainterMosaicMode))
		{
			CGE_LOG_ERROR("GPUPainterMosaicLayer::usePainterBlendMode init program failed!");
			return false;
		}
		m_blendProgram->bind();
		m_blendProgram->sendUniformf(paramBlendIntensityName, m_opacity);
		return true;
	}

	void GPUPainterMosaicLayer::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		usePainterBlendMode();
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);

		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_blendProgram->sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);

		glActiveTexture(CGE_TEXTURE_START);
		glBindTexture(GL_TEXTURE_2D, m_canvasTex);
		m_blendProgram->sendUniformi(paramBlendTextureName, CGE_TEXTURE_START_INDEX);

		CGESizei sz = handler->getOutputFBOSize();
		m_blendProgram->sendUniformf(paramSamplerStepsName, 1.0f / sz.width, 1.0f / sz.height);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

}