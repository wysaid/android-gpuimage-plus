/*
 * cgePainter.h
 *
 *  Created on: 2014-3-7
 *      Author: Wang Yang
 */

#ifndef _CGEPAINTER_H_
#define _CGEPAINTER_H_

#include "cgeGLFunctions.h"
#include "cgeBlendFilter.h"
#include "cgeVec.h"
#include "cgeMat.h"
#include <vector>

// #ifndef GL_VERTEX_PROGRAM_POINT_SIZE
// #define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
// #endif
// 
// #ifndef GL_POINT_SPRITE
// #define GL_POINT_SPRITE 0x8861
// #endif

namespace CGE
{
	struct PathAttrib
	{
		PathAttrib() : opacity(1.0f), pointSize(20.0f), pointStride(3.0f), kind(PATH_NORMAL), brushStyle(BRUSH_DEFAULT)/*, rotation(0.0f) */{}
		~PathAttrib() {}

		void pushPoint(Vec2f point)
		{
			vecPoints.push_back(point);
		}

		void pushPoint(GLfloat x, GLfloat y)
		{
			vecPoints.push_back(Vec2f(x, y));
		}

		void clear() { vecPoints.clear(); }

		GLfloat r, g, b; //For red, green, blue. # range: [0, 1]

		/*
		特别提示：
	      此处最终亮度公式约为: opacity * (pointSize / pointStride)
		  由此可知， 当pointSize越大，pointStride越小时，绘制的路径会越不透明。
		  若要达到相应的效果，须得把opacity对应调小，或者pointSize与pointStride同比增长
		*/

		GLfloat opacity; //For path intensity. Default: 1. # range: [0, 1]
		GLfloat pointSize; //For texture size. Default: 20. # range: [0, 150] pixels.		
		GLfloat pointStride; //Default to 3. Leave it if the speed is enough.

		std::vector<Vec2f> vecPoints;
		enum PathKind { PATH_NORMAL, PATH_HAS_DIRECTION, PATH_POINT_ROT_RANDOM, PATH_ERASER, MAX_PATH_KIND_NUM } kind;  // Default: NORMAL.
		enum BrushStyle { BRUSH_DEFAULT = 0, BRUSH_TEST0, BRUSH_TEST1, BRUSH_TEST2, BRUSH_TEST3, BRUSH_TEST4, BRUSH_TEST5, MAX_BRUSH_STYLE_NUM } brushStyle; //Keep the brush status.

// 		static Vec2f getPointSizeRange()
// 		{
// 			Vec2f v;
// 			glGetFloatv(GL_POINT_SIZE_GRANULARITY, &v[0]);
// 			return 
// 		}
	};

	struct GPUPainterTextureManager
	{
		static GLuint getTextureByBrushStyle(PathAttrib::BrushStyle style);
        // 退出本次编辑时调用
		static void clearTextures();

		static void setLoadFunction(CGEBufferLoadFun loadFunc, void* loadParam, CGEBufferUnloadFun unloadFunc, void* unloadParam) { texLoadFunc = loadFunc; texLoadParam = loadParam; texUnloadFunc = unloadFunc, texUnLoadParam = unloadParam; }
		static GLuint loadBrushTexture(PathAttrib::BrushStyle style);

		static GLuint painterBrushTex[PathAttrib::MAX_BRUSH_STYLE_NUM];
		static CGEBufferLoadFun texLoadFunc;
		static CGEBufferUnloadFun texUnloadFunc;
		static void *texLoadParam, *texUnLoadParam;
	};

	class GPUPainterLayer : public CGEImageFilterInterface
	{
	public:
		GPUPainterLayer();
		~GPUPainterLayer();

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

		//////////////////////////////////////////////////////////////////////////

		virtual bool init();

		virtual bool resizeCanvas(int width, int height, const void* buffer = NULL, CGEBufferFormat format = CGE_FORMAT_RGBA_INT8);

		virtual bool drawResultToScreen(int x, int y, int w, int h);

		//////////////////////////////////////////////////////////////////////////

		virtual void drawPath(PathAttrib& attrib);
		virtual void drawPathNormal(PathAttrib& attrib);
		virtual void drawPathWithDirection(PathAttrib& attrib);
		virtual void drawPathRandomRotation(PathAttrib& attrib);
		virtual void drawPathEraser(PathAttrib& attrib);

// 		virtual void drawPath(float startX, float startY, float endX, float endY);
// 
// 		virtual void drawPath(Vec2f& start, Vec2f& end);

		virtual void pushPath(PathAttrib& attrib);

		virtual bool undo();
		virtual bool canUndo() const; //Test whether the painter could undo.

		virtual bool redo();
		virtual bool canRedo() const; //Test whether the painter could redo.

		virtual void clearCanvas(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha); //Drop the result.

		virtual void clearPath(size_t index = 0);


		//////////////////////////////////////////////////////////////////////////		

		GLfloat getOpacity() const { return m_opacity; }
		void setOpacity(GLfloat opacity) { m_opacity = opacity; }

		CGETextureBlendMode getBlendMode() const { return m_blendMode; }
		virtual void setBlendMode(CGETextureBlendMode mode) { m_blendMode = mode; }

	public:

		GLuint getCanvasTextureID() { return m_canvasTex; }

		//Clear the canvas and redraw the scene to the specified point.
		void redrawPathToIndex();


		//Draw to the end when indexEnd is unsigned(-1).
		void drawPathByIndex(size_t indexStart, size_t indexEnd);
		//No Enable OpenGL Function. So OpenGL should be enabled before this.
		ProgramObject& setPainterAttrib(PathAttrib& attrib);

		//////////////////////////////////////////////////////////////////////////

		void setPointSize(ProgramObject& obj, GLfloat pointSize);
		void setColorAndOpacity(ProgramObject& obj, GLfloat r, GLfloat g, GLfloat b, GLfloat opacity);
		//void setRotation(ProgramObject& obj, GLfloat normX, GLfloat normY);
		void setBrushStyle(ProgramObject& obj, PathAttrib::BrushStyle);

		
		static void genInterpolationPoints(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, Vec2f& canvasSize, GLfloat stride);
		static void genInterpolationPointsAndDirections(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, std::vector<Vec2f>& dirPoints, Vec2f& canvasSize, GLfloat stride);

		static void genInterpolationPointsAndRandomDir(const std::vector<Vec2f>& srcPoints, std::vector<Vec2f>& resultPoints, std::vector<Vec1f>& dir, Vec2f& canvasSize, GLfloat stride);

		static inline  float calcRadianByVec(Vec2f& start, Vec2f& end);
		static inline float calcRadianByVec(float x, float y);
//		static GLfloat getMaxPointSize() { GLfloat n; glGetFloatv(GL_POINT_SIZE_MAX, &n); return n; }

	protected:
		void setPaintEnv();
		virtual bool usePainterBlendMode();
		

	protected:

		static CGEConstString paramDraw2ScreenTextureName;
		static CGEConstString paramPointSizeName;
		static CGEConstString paramColorOpacityName;
		static CGEConstString paramRotationRadName;
		static CGEConstString paramRotationNormalName;
		static CGEConstString paramBrushTextureName;
		static CGEConstString paramModelViewProjectionName;
		static CGEConstString paramBlendIntensityName;
		static CGEConstString paramBlendTextureName;

	protected:

		CGETextureBlendMode m_blendMode, m_programBlendMode;
		ProgramObject m_paintDefaultProgram, m_paintRotWithNormProgram, m_paintRotWithRadProgram;
		ProgramObject* m_draw2ScreenProgram;
		ProgramObject* m_blendProgram;
		GLuint m_canvasTex;
		GLuint m_frameBuffer;
		Vec2f m_canvasSize;

		Mat4 m_mvp;

		std::vector<PathAttrib> m_vecPath;
		std::vector<PathAttrib>::size_type m_pathIndex;

		GLfloat m_opacity;

	};

	class GPUPainterLayerLighter : public GPUPainterLayer
	{
	public:
		virtual bool init();
	};


	//特殊的Painter，与上述painter的混合方法不兼容，无法进行特殊设定。
	class GPUPainterMosaicLayer : public GPUPainterLayer
	{
	public:
		virtual bool init();
		// 色块大小 range: >= 1 pixel， 1为原图
		void setBlurRadius(float value);
		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);
		void setBlendMode(CGETextureBlendMode mode) {}
	protected:
		static CGEConstString paramBlurRadiusName;
		static CGEConstString paramSamplerStepsName;

		virtual bool usePainterBlendMode();
	};

}

#endif