/*
 * cgeColorMappingFilter.h
 *
 *  Created on: 2016-8-5
 *      Author: Wang Yang
 * Description: Color mapping
 */

#ifndef _CGE_COLOR_MAPPING_FILTER_H_
#define _CGE_COLOR_MAPPING_FILTER_H_

#include "cgeGLFunctions.h"
#include "cgeVec.h"

#include <vector>

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
    virtual void endPushing(); // Call after pushMapingArea ends

    // texWith, texHeight represents texture size
    // texUnitWidth, texUnitHeight represents the resolution of each mapping unit after mapping
    virtual void setupMapping(GLuint mappingTex, int texWidth, int texHeight, int texUnitWidth, int texUnitHeight);

protected:
    GLuint m_mappingTexture;
    CGESizei m_texSize; // Texture pixel size
    CGESizei m_texUnitResolution;
    std::vector<MappingArea> m_mappingAreas;
};

} // namespace CGE

#endif // !_CGE_COLOR_MAPPING_FILTER_H_
