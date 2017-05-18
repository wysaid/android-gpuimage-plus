/*
* cgeLiquidation.cpp
*
*  Created on: 2014-5-15
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeLiquidationFilter.h"
#include "cgeMat.h"

static CGEConstString s_vshDeform = CGE_SHADER_STRING
(
attribute vec2 vPosition;
attribute vec2 vTexture;
varying vec2 textureCoordinate;
void main()
{
	//An opportunism code. Do not use it unless you know what it means.
	gl_Position = vec4(vPosition * 2.0 - 1.0, 0.0, 1.0);
	
	textureCoordinate = vTexture;//(vPosition.xy + 1.0) / 2.0;
}
);

static CGEConstString s_fshDeform = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
void main()
{
	gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
}
);

#ifdef CGE_DEFORM_SHOW_MESH

static CGEConstString s_fshMesh = CGE_SHADER_STRING_PRECISION_L
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
void main()
{
	gl_FragColor.rgb = 1.0 - texture2D(inputImageTexture, textureCoordinate).rgb;
	gl_FragColor.a = 1.0;
}
);

#endif

namespace CGE
{

	CGEConstString CGELiquidationFilter::paramTextureVertexName = "vTexture";

	CGELiquidationFilter::CGELiquidationFilter() : m_meshVBO(0), m_meshIndexVBO(0), m_textureVBO(0), m_currentMeshIndex(0), m_doingRestore(false)
	{
#ifdef CGE_DEFORM_SHOW_MESH
		m_program.bindAttribLocation(paramTextureVertexName, 1);
		m_programMesh.bindAttribLocation(paramPositionIndexName, 0);
		m_programMesh.bindAttribLocation(paramTextureVertexName, 1);
		if(!m_programMesh.initWithShaderStrings(s_vshDeform, s_fshMesh))
		{
			CGE_LOG_ERROR("Init Mesh Program Failed!\n");
		}

		m_bShowMesh = false;

#endif

		setUndoSteps(10);
	}

	CGELiquidationFilter::~CGELiquidationFilter()
	{
		
		glDeleteBuffers(1, &m_meshVBO);
		glDeleteBuffers(1, &m_meshIndexVBO);
		glDeleteBuffers(1, &m_textureVBO);
	}

	bool CGELiquidationFilter::initWithMesh(float width, float height, float stride)
	{
		return initWithMesh(width / height, stride / CGE_MAX(width, height));
	}

	bool CGELiquidationFilter::initWithMesh(float ratio, float stride)
	{
		if(!initShadersFromString(s_vshDeform, s_fshDeform))
			return false;

		stride = CGE_MID(stride, 0.001f, 0.2f);

		CGE_LOG_CODE(
			if(ratio < 0.0f)
			{
				CGE_LOG_ERROR("DeformProcessor::initWithMesh Ratio must > 0!\n");
				return false;
			}
		);

		float len = 1.0f / stride;
		CGESizef sz;
		if(ratio > 1.0f)
		{
			sz.width = len;
			sz.height = len / ratio;
		}
		else
		{
			sz.width = len * ratio;
			sz.height = len;
		}

		m_meshSize.width = (int)sz.width;
		m_meshSize.height = (int)sz.height;

		CGE_LOG_CODE(
			if(m_meshSize.width < 2 || m_meshSize.width > 5000 ||
				m_meshSize.height < 2 || m_meshSize.height > 5000)
			{
				CGE_LOG_ERROR("Invalid Mesh Size!\n");
				return false;
			}
			);

		m_mesh.resize(m_meshSize.width * m_meshSize.height);
		restoreMesh();

		return initBuffers();
	}

	void CGELiquidationFilter::restoreMesh()
	{
		if(m_mesh.size() != m_meshSize.width * m_meshSize.height || m_mesh.empty())
		{
			CGE_LOG_ERROR("Invalid Mesh!\n");
		}

		const float widthStep = 1.0f / (m_meshSize.width - 1.0f);
		const float heightStep = 1.0f / (m_meshSize.height - 1.0f);

		for(int i = 0; i != m_meshSize.height; ++i)
		{
			const float heightI = i * heightStep;
			int index = m_meshSize.width * i;
			for(int j = 0; j != m_meshSize.width; ++j)
			{
				const float widthJ = j * widthStep;
				m_mesh[index] = Vec2f(widthJ, heightI);
				++index;
			}
		}

		m_vecMeshes.clear();

		updateBuffers();
	}

	void CGELiquidationFilter::restoreMeshWithIntensity(float intensity)
	{
		if(m_mesh.size() != m_meshSize.width * m_meshSize.height || m_mesh.empty())
		{
			CGE_LOG_ERROR("Invalid Mesh!\n");
			return ;
		}

		if(!m_doingRestore && !pushMesh())
		{
			CGE_LOG_ERROR("DeformProcessor::restoreMeshWithIntensity failed!\n");
			return ;
		}

		const std::vector<Vec2f>& v2Mesh = m_vecMeshes[m_currentMeshIndex];

		const float widthStep = 1.0f / (m_meshSize.width - 1.0f);
		const float heightStep = 1.0f / (m_meshSize.height - 1.0f);

		const float revIntensity = 1.0f - intensity;

		for(int i = 0; i != m_meshSize.height; ++i)
		{
			const float heightI = i * heightStep;
			int index = m_meshSize.width * i;
			for(int j = 0; j != m_meshSize.width; ++j)
			{
				const float widthJ = j * widthStep;
				const Vec2f v(widthJ, heightI);
				m_mesh[index] = v2Mesh[index] * revIntensity + v * intensity;
				++index;
			}
		}

		updateBuffers();

		m_doingRestore = true;
	}

	bool CGELiquidationFilter::initBuffers()
	{
		glDeleteBuffers(1, &m_meshVBO);
		glGenBuffers(1, &m_meshVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh.size() * sizeof(m_mesh[0]), m_mesh.data(), GL_STREAM_DRAW);

		glDeleteBuffers(1, &m_textureVBO);
		glGenBuffers(1, &m_textureVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureVBO);
		glBufferData(GL_ARRAY_BUFFER, m_mesh.size() * sizeof(m_mesh[0]), m_mesh.data(), GL_STATIC_DRAW);


		int index = 0;
		std::vector<unsigned short> meshIndexes;
		m_meshIndexSize = (m_meshSize.width-1) * (m_meshSize.height-1) * 2;
		meshIndexes.resize(m_meshIndexSize * 3);

		for(int i = 0; i < m_meshSize.height - 1; ++i)
		{
			int pos1 = i * m_meshSize.width;
			int pos2 = (i + 1) * m_meshSize.width;

#ifdef CGE_DEFORM_SHOW_MESH

			if(i%2)
			{
				for(int j = 0; j < m_meshSize.width - 1; ++j)
				{
					meshIndexes[index] = pos1 + j;
					meshIndexes[index + 1] = pos1 + j + 1;
					meshIndexes[index + 2] = pos2 + j;
					meshIndexes[index + 3] = pos2 + j;
					meshIndexes[index + 4] = pos1 + j + 1;
					meshIndexes[index + 5] = pos2 + j + 1;
					index += 6;
				}
			}
			else
			{
				for(int j = m_meshSize.width - 2; j >= 0; --j)
				{
					meshIndexes[index] = pos1 + j + 1;
					meshIndexes[index + 1] = pos2 + j + 1;
					meshIndexes[index + 2] = pos2 + j;
					meshIndexes[index + 3] = pos1 + j;
					meshIndexes[index + 4] = pos1 + j + 1;
					meshIndexes[index + 5] = pos2 + j;		
					index += 6;
				}
			}

#else
			for(int j = 0; j < m_meshSize.width - 1; ++j)
			{
				meshIndexes[index] = pos1 + j;
				meshIndexes[index + 1] = pos1 + j + 1;
				meshIndexes[index + 2] = pos2 + j;
				meshIndexes[index + 3] = pos2 + j;
				meshIndexes[index + 4] = pos1 + j + 1;
				meshIndexes[index + 5] = pos2 + j + 1;
				index += 6;
			}
#endif
		}

		glDeleteBuffers(1, &m_meshIndexVBO);
		glGenBuffers(1, &m_meshIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndexes.size() * sizeof(meshIndexes[0]), meshIndexes.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return true;
	}

	bool CGELiquidationFilter::updateBuffers()
	{
		if(m_meshVBO != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_meshVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_mesh[0]) * m_mesh.size(), m_mesh.data(), GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			return true;
		}
		return false;
	}

	void CGELiquidationFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		if(m_meshVBO == 0 || m_mesh.empty())
		{
			CGE_LOG_ERROR("DeformProcessor::render2Texture - Invalid Mesh!\n");
			handler->swapBufferFBO();
			return ;
		}
		
		handler->setAsTarget();
		m_program.bind();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		if(m_uniformParam != nullptr)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			glBindBuffer(GL_ARRAY_BUFFER, m_meshVBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, m_textureVBO);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshIndexVBO);

		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

 		glDrawElements(GL_TRIANGLES, m_meshIndexSize * 3, GL_UNSIGNED_SHORT, 0);

#ifdef CGE_DEFORM_SHOW_MESH
		if(m_bShowMesh)
		{
			m_programMesh.bind();
			glDrawElements(GL_LINE_STRIP, m_meshIndexSize * 3, GL_UNSIGNED_SHORT, 0);
		}
#endif
		
		cgeCheckGLError("glDrawElements");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void CGELiquidationFilter::forwardDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity)
	{
		m_doingRestore = false;

		CGE_LOG_CODE(
		clock_t t = clock();
		);

		float loopStartY = CGE_MAX(CGE_MIN(start[1], end[1]) - radius, -radius);
		float loopEndY = CGE_MIN(CGE_MAX(start[1], end[1]) + radius, h + radius);
		float loopStartX = CGE_MAX(CGE_MIN(start[0], end[0]) - radius, -radius);
		float loopEndX = CGE_MIN(CGE_MAX(start[0], end[0]) + radius, w + radius);

		CGE_LOG_INFO("Canvas Size: %g, %g\nBoundBox: left:%g, top: %g, right: %g, bottom: %g\n", w, h, loopStartX, loopStartY, loopEndX, loopEndY);

		Vec2f sz(w, h);
		Vec2f motion = (end - start) / sz;

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const Vec2f v0 = m_mesh[lines + j] * sz;

				if(v0[0] < loopStartX || v0[0] > loopEndX ||
					v0[1] < loopStartY || v0[1] > loopEndY)
					continue;

				const Vec2f v = v0 - start;
				float dis = v.length();
				if(dis > radius)
					continue;
				float percent = 1.0f - dis / radius;
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[lines + j] += motion * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", float(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::pushLeftDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity, float angle)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		float loopStartY = CGE_MAX(CGE_MIN(start[1], end[1]) - radius, -radius);
		float loopEndY = CGE_MIN(CGE_MAX(start[1], end[1]) + radius, h + radius);
		float loopStartX = CGE_MAX(CGE_MIN(start[0], end[0]) - radius, -radius);
		float loopEndX = CGE_MIN(CGE_MAX(start[0], end[0]) + radius, w + radius);

		CGE_LOG_INFO("Canvas Size: %g, %g\nBoundBox: left:%g, top: %g, right: %g, bottom: %g\n", w, h, loopStartX, loopStartY, loopEndX, loopEndY);

		Vec2f sz(w, h);
		Vec2f motion = (end - start) / sz;
		
		//coordinate transformation
		const float cosRad = cosf(-angle);
		const float sinRad = sinf(-angle);
		motion = Mat2(cosRad, sinRad, -sinRad, cosRad) * motion;

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const Vec2f v0 = m_mesh[lines + j] * sz;

				if(v0[0] < loopStartX || v0[0] > loopEndX ||
					v0[1] < loopStartY || v0[1] > loopEndY)
					continue;

				const Vec2f v = v0 - start;
				float dis = v.length();
				if(dis > radius)
					continue;
				float percent = 1.0f - dis / radius;
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[lines + j] += motion * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", float(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::restoreMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		Vec2f sz(w, h);
		const float widthStep = 1.0f / (m_meshSize.width - 1.0f);
		const float heightStep = 1.0f / (m_meshSize.height - 1.0f);

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			const float heightI = i * heightStep;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const int index = lines + j;
				const Vec2f v0 = m_mesh[index] * sz - pnt;
				float dis = v0.length();
				if(dis > radius)
					continue;

				const float widthJ = j * widthStep;
				const Vec2f v(widthJ, heightI);
				float percent = 1.0f - dis / radius;
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[index] = m_mesh[index] * (1.0f - percent) + v * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", float(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::bloatMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		Vec2f sz(w, h);

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const int index = lines + j;
				const Vec2f v = m_mesh[index] * sz - pnt;
				float dis = v.length();
				if(dis > radius)
					continue;

				float percent = 1.0f - dis / radius;
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[index] += v / sz * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", float(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::wrinkleMeshWithPoint(Vec2f pnt, float w, float h, float radius, float intensity)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		Vec2f sz(w, h);

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const int index = lines + j;
				const Vec2f v = pnt - m_mesh[index] * sz;
				float dis = v.length();
				if(dis > radius)
					continue;

				float percent = 1.0f - dis / radius;
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[index] += v / sz * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", (double)(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::setUndoSteps(unsigned n)
	{
		m_undoSteps = n;
		if(n == 0)
			m_vecMeshes.clear();
		else if(m_currentMeshIndex > n)
		{
			m_currentMeshIndex = n;
			m_vecMeshes.erase(m_vecMeshes.begin() + m_currentMeshIndex, m_vecMeshes.end());
		}
	}

	bool CGELiquidationFilter::canUndo()
	{
		return !m_vecMeshes.empty() && m_currentMeshIndex > 0;
	}

	bool CGELiquidationFilter::canRedo()
	{
		return !m_vecMeshes.empty() && m_currentMeshIndex < m_vecMeshes.size() - 1;
	}

	bool CGELiquidationFilter::undo()
	{
		if(!canUndo())
			return false;

		--m_currentMeshIndex;
		m_mesh = m_vecMeshes[m_currentMeshIndex];

		updateBuffers();
		m_doingRestore = false;
		return true;
	}

	bool CGELiquidationFilter::redo()
	{
		if(!canRedo())
			return false;

		++m_currentMeshIndex;
		m_mesh = m_vecMeshes[m_currentMeshIndex];

		updateBuffers();
		return true;
	}

	bool CGELiquidationFilter::pushMesh()
	{
		if(m_undoSteps <= 0)
			return false;


		if(!m_vecMeshes.empty() && m_currentMeshIndex < m_vecMeshes.size() - 1)
			m_vecMeshes.erase(m_vecMeshes.begin() + m_currentMeshIndex + 1, m_vecMeshes.end());

		m_vecMeshes.push_back(m_mesh);

		if(m_vecMeshes.size() > m_undoSteps)
			m_vecMeshes.erase(m_vecMeshes.begin(), m_vecMeshes.end() - m_undoSteps);

		m_currentMeshIndex = (unsigned)m_vecMeshes.size() - 1;
		return true;
	}

	//Algorithm created by Wang Yang. Ask me if you wanna know.
	void CGELiquidationNicerFilter::forwardDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		float loopStartY = CGE_MAX(CGE_MIN(start[1], end[1]) - radius, -radius);
		float loopEndY = CGE_MIN(CGE_MAX(start[1], end[1]) + radius, h + radius);
		float loopStartX = CGE_MAX(CGE_MIN(start[0], end[0]) - radius, -radius);
		float loopEndX = CGE_MIN(CGE_MAX(start[0], end[0]) + radius, w + radius);

		CGE_LOG_INFO("Canvas Size: %g, %g\nBoundBox: left:%g, top: %g, right: %g, bottom: %g\n", w, h, loopStartX, loopStartY, loopEndX, loopEndY);

		const Vec2f sz(w, h);
		Vec2f motion = (end - start) / sz;

		Vec2f v2Min = start, v2Max = end;
		if(v2Min[0] > v2Max[0]) std::swap(v2Min[0], v2Max[0]);
		if(v2Min[1] > v2Max[1]) std::swap(v2Min[1], v2Max[1]);

		//直线方程系数， 直线由起始点跟终止点构成。
		float eqA, eqB, eqC, eqD, eqD2;

		//计算直线方程
		{
			float a = start[1] - end[1], b = start[0] - end[0], c = start[0] * end[1] - start[1] * end[0];			

			if(CGE_FLOATCOMP0(b))
			{
				eqA = 1.0f;
				eqB = 0.0f;
				eqC = -start[0];
			}
			else
			{
				eqA = a / b;
				eqB = -1.0f;
				eqC = c / b;
			}
			eqD2 = eqA * eqA + eqB * eqB;
			eqD = sqrtf(eqD2);
		}

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const Vec2f v = m_mesh[lines + j] * sz;
				
				if(v[0] < loopStartX || v[0] > loopEndX ||
					v[1] < loopStartY || v[1] > loopEndY)
					continue;

				//点到直线距离
				float dis1 = fabsf(eqA * v[0] + eqB * v[1] + eqC) / eqD;

				if(dis1 > radius)
				{
					continue;
				}

				//点到端点距离
				float dis2 = (v - start).length();
				float dis3 = (v - end).length();
				//点在直线方向上的投影坐标
				const Vec2f projV(
					(eqB * eqB * v[0] - eqA * eqB * v[1] - eqA * eqC) / eqD2, 
					(eqA * eqA * v[1] - eqB * eqC - eqA * eqB * v[0]) / eqD2);

				//实际计算距离，若点在投影上则使用点到直线距离，否则使用较近端点距离
				float dis;

				if(projV[0] < v2Min[0] || projV[0] > v2Max[0] ||
					projV[1] < v2Min[1] || projV[1] > v2Max[1])					
				{
					if(dis2 > radius &&	dis3 > radius)
					{
						continue;
					}
					else
					{
						dis = CGE_MIN(dis2, dis3);
					}					
				}
				else dis = dis1;

				float percent = (1.0f - dis / radius);
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[lines + j] += motion * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", (double)(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationNicerFilter::pushLeftDeformMesh(Vec2f start, Vec2f end, float w, float h, float radius, float intensity, float angle)
	{
		

		m_doingRestore = false;

		CGE_LOG_CODE(
			clock_t t = clock();
		);

		float loopStartY = CGE_MAX(CGE_MIN(start[1], end[1]) - radius, -radius);
		float loopEndY = CGE_MIN(CGE_MAX(start[1], end[1]) + radius, h + radius);
		float loopStartX = CGE_MAX(CGE_MIN(start[0], end[0]) - radius, -radius);
		float loopEndX = CGE_MIN(CGE_MAX(start[0], end[0]) + radius, w + radius);

		CGE_LOG_INFO("Canvas Size: %g, %g\nBoundBox: left:%g, top: %g, right: %g, bottom: %g\n", w, h, loopStartX, loopStartY, loopEndX, loopEndY);

		const Vec2f sz(w, h);
		Vec2f motion = (end - start) / sz;

		//coordinate transformation
		const float cosRad = cosf(-angle);
		const float sinRad = sinf(-angle);
		motion = Mat2(cosRad, sinRad, -sinRad, cosRad) * motion;

		Vec2f v2Min = start, v2Max = end;
		if(v2Min[0] > v2Max[0]) std::swap(v2Min[0], v2Max[0]);
		if(v2Min[1] > v2Max[1]) std::swap(v2Min[1], v2Max[1]);

		//直线方程系数， 直线由起始点跟终止点构成。
		float eqA, eqB, eqC, eqD, eqD2;

		//计算直线方程
		{
			float a = start[1] - end[1], b = start[0] - end[0], c = start[0] * end[1] - start[1] * end[0];			

			if(CGE_FLOATCOMP0(b))
			{
				eqA = 1.0f;
				eqB = 0.0f;
				eqC = -start[0];
			}
			else
			{
				eqA = a / b;
				eqB = -1.0f;
				eqC = c / b;
			}
			eqD2 = eqA * eqA + eqB * eqB;
			eqD = sqrtf(eqD2);
		}

		for(int i = 0; i < m_meshSize.height; ++i)
		{
			const int lines = i * m_meshSize.width;
			for(int j = 0; j < m_meshSize.width; ++j)
			{
				const Vec2f v = m_mesh[lines + j] * sz;

				if(v[0] < loopStartX || v[0] > loopEndX ||
					v[1] < loopStartY || v[1] > loopEndY)
					continue;

				//点到直线距离
				float dis1 = fabsf(eqA * v[0] + eqB * v[1] + eqC) / eqD;

				if(dis1 > radius)
				{
					continue;
				}

				//点到端点距离
				float dis2 = (v - start).length();
				float dis3 = (v - end).length();
				//点在直线方向上的投影坐标
				const Vec2f projV(
					(eqB * eqB * v[0] - eqA * eqB * v[1] - eqA * eqC) / eqD2, 
					(eqA * eqA * v[1] - eqB * eqC - eqA * eqB * v[0]) / eqD2);

				//实际计算距离，若点在投影上则使用点到直线距离，否则使用较近端点距离
				float dis;

				if(projV[0] < v2Min[0] || projV[0] > v2Max[0] ||
					projV[1] < v2Min[1] || projV[1] > v2Max[1])					
				{
					if(dis2 > radius &&	dis3 > radius)
					{
						continue;
					}
					else
					{
						dis = CGE_MIN(dis2, dis3);
					}					
				}
				else dis = dis1;

				float percent = (1.0f - dis / radius);
				percent = percent * percent * (3.0f - 2.0f * percent) * intensity;
				m_mesh[lines + j] += motion * percent;
			}
		}

		updateBuffers();

		CGE_LOG_CODE(
			CGE_LOG_INFO("##########Deform mesh take time: %gs #####\n", float(clock()-t) / CLOCKS_PER_SEC);
		);
	}

	void CGELiquidationFilter::showMesh(bool bShow)
	{
#ifdef CGE_DEFORM_SHOW_MESH
		m_bShowMesh = bShow;
#endif
	}

}