/*
* cgeColorMappingFilter.h
*
*  Created on: 2016-8-5
*      Author: Wang Yang
* Description: 色彩映射
*/

#ifndef _CGE_COLOR_MAPPING_FILTER_H_
#define _CGE_COLOR_MAPPING_FILTER_H_

#include "cgeGLFunctions.h"
#include <vector>
#include "cgeVec.h"

namespace CGE
{
	class CGEColorMappingFilter : public CGEImageFilterInterface
	{
	public:
		CGEColorMappingFilter();
		~CGEColorMappingFilter();

		enum MapingMode
		{
			MAPINGMODE_DEFAULT = 0,
			MAPINGMODE_BUFFERED_AREA = 0,
			MAPINGMODE_SINGLE = 1,
		};

		struct MappingArea
		{
			Vec4f area;
			float weight;

			bool operator<(const MappingArea& m) const
			{
				return weight < m.weight;
			}

		};

		static CGEColorMappingFilter* createWithMode(MapingMode mode = MAPINGMODE_DEFAULT);

		virtual void pushMapingArea(const MappingArea& area);
		virtual void endPushing(); //pushMapingArea 结束之后调用

		//texWith, texHeight 表示纹理大小
		//texUnitWidth, texUnitHeight 表示每个映射单元映射后的分辨率
		virtual void setupMapping(GLuint mappingTex, int texWidth, int texHeight, int texUnitWidth, int texUnitHeight);


	protected:

		GLuint m_mappingTexture;
		CGESizei m_texSize;  //纹理像素尺寸
		CGESizei m_texUnitResolution;
		std::vector<MappingArea> m_mappingAreas;
	};

}

#endif // !_CGE_COLOR_MAPPING_FILTER_H_
