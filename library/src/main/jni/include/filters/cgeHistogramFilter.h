#ifndef _HISTOGRAMFILTER_H_
#define _HISTOGRAMFILTER_H_

#include "cgeImageFilter.h"

namespace CGE
{
    class CGEHistogramFilter : public CGEImageFilterInterface
    {
    public:

        /**
         * 坐标系原点, 默认 (0.5, 0.5)
         */
        void setCenter(float x, float y);
        /**
         * 宽度
         */
        void setXValue(float value);
        /**
         * 高度
         */
        void setYValue(float value);
        /**
         * 背景颜色
         */
		void setColor(float r, float b, float g);

        bool init();

	protected:
		static CGEConstString paramCenter;
		static CGEConstString paramXValue;
		static CGEConstString paramYValue;
		static CGEConstString paramColor;
    };
}

#endif