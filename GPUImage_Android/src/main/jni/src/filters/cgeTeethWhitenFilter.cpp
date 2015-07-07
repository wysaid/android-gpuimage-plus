/*
* cgeTeethWhitenFilter.cpp
*
*  Created on: 2014-4-23
*      Author: Wang Yang
*/

#include "cgeTeethWhitenFilter.h"


static CGEConstString s_fshTeethWhiten = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec3 dst;
	if(src.r < src.g)
	{
		if(src.r < src.b)
		{
			dst.r = src.r + 0.587 * (src.g - src.r) + 0.114 * (src.b - src.r);
			dst.g = max(src.g, dst.r);
			dst.b = max(src.b, dst.r);
		}
		else
		{
			dst = src.rgb;
		}
	}
	else
	{
		if(src.g < src.b)
		{
			dst.g = src.g + 0.299 * (src.r - src.g) + 0.114 * (src.b - src.g);
			dst.r = max(src.r, dst.g);
			dst.b = max(src.b, dst.g);
		}
		else
		{
			dst.b = src.b + 0.299 * (src.r - src.b) + 0.587 * (src.g - src.b);
			dst.r = max(src.r, dst.b);
			dst.g = max(src.g, dst.b);
		}
	}
	gl_FragColor = vec4(dst, src.a);
}
);

namespace CGE
{
	bool CGETeethWhitenFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshTeethWhiten);
	}

}