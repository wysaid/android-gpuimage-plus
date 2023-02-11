
#ifndef _FALSECOLORFILTER_H_
#define _FALSECOLORFILTER_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEFalseColorFilter : public CGEImageFilterInterface
	{
	public:

		bool init();

        void setIntensity(float intensity);

	protected:
		static CGEConstString paramIntensity;

	};

}
#endif