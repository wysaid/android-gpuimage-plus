/*
* cgeLiquidation.h
*
*  Created on: 2014-5-15
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGELIQUIDATION_H_
#define _CGELIQUIDATION_H_

#include "cgeAdvancedEffectsCommon.h"
#include "cgeVec.h"

#define CGE_DEFORM_SHOW_MESH

namespace CGE
{

	class CGELiquidationFilter : public CGEImageFilterInterface
	{
	public:
		CGELiquidationFilter();
		~CGELiquidationFilter();

		//'ratio': imageWidth / imageHeight.
		//'stride' range: (0.001, 0.2]. The mesh would be larger as stride is smaller and the result would be more accurate but may take longer time for processing.
		bool initWithMesh(float ratio, float stride);


		//'width', 'height': The real size of the picture in pixels.
		//'stride': Set the mesh in real pixels, the mesh size would be (width / stride, height / stride)
		bool initWithMesh(float width, float height, float stride);

		void restoreMesh();

		//intensity range: [0, 1]. 0 for no effect and 1 for origin.
		//default: 1
		void restoreMeshWithIntensity(float intensity);

		//'start', 'end': the real position of the cursor between two operations.
		//'pnt': the real position of the cursor.
		//'w', 'h': the canvas size. (the max 'x' and 'y' of the cursor)
		//radius: the deform radius in real pixels.
		//intensity: range (0, 1], 0 for origin. Better not more than 0.5
		virtual void forwardDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity); //向前变形工具
		virtual void pushLeftDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity, float angle = 3.14159f / 2.0f);//(左)推动工具：将路径上的网格向路径的angle方向（路径的左侧）推动。 末尾参数angle默认为左侧，可设置为任意值
		virtual void restoreMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity);//重建工具
		virtual void bloatMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity);//膨胀工具
		virtual void wrinkleMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity);//褶皱工具		

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void setUndoSteps(unsigned n);
		bool canUndo();
		bool canRedo();
		bool undo();
		bool redo();
		bool pushMesh();

		void showMesh(bool bShow);


	protected:
		static CGEConstString paramTextureVertexName;

		bool initBuffers();
		bool updateBuffers();

	protected:
		unsigned m_undoSteps, m_currentMeshIndex;
		GLuint m_meshVBO, m_meshIndexVBO, m_textureVBO;
		CGESizei m_meshSize;
		std::vector<Vec2f> m_mesh;
		std::vector<std::vector<Vec2f> > m_vecMeshes;
		int m_meshIndexSize;
		bool m_doingRestore;
#ifdef CGE_DEFORM_SHOW_MESH
		ProgramObject m_programMesh;
		bool m_bShowMesh;
#endif
	};

	class CGELiquidationNicerFilter : public CGELiquidationFilter
	{
	public:
		void forwardDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity);//see in 'CGELiquidationFilter'
		virtual void pushLeftDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity, float angle = 3.14159f / 2.0f);//see in 'CGELiquidationFilter'
	};

}

#endif