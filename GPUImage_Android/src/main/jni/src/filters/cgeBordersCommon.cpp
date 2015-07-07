/*
* cgeBordersCommon.cpp
*
*  Created on: 2014-1-23
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeBordersCommon.h"
#include <cmath>

#define BORDERBIND_ID 0

#define BLENDBORDER_PREMULTIPLIED "vec3 blend(vec3 src, vec3 dst, float alpha){"\
	"return src * (1.0 - alpha) + dst;}"

#define BLENDBORDER_NOT_PREMULTIPLIED "vec3 blend(vec3 src, vec3 dst, float alpha){"\
	"return mix(src, dst, alpha);}"

static CGEConstString s_fshBorderS = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 frame = texture2D(borderTexture, textureCoordinate);
	src.rgb = blend(src.rgb, frame.rgb, frame.a);
	gl_FragColor = src;
}
);

static CGEConstString s_fshBorderM = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec2 borderThickness;
uniform vec2 borderLengthHV;

//leftTop, Top, rightTop, right, rightBottom, bottom, leftBottom, left
uniform vec4 borderBlock[8];)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;

    if(textureCoordinate.y <= borderThickness.y)
    {
        if(textureCoordinate.x <= borderThickness.x)
        {
            coord = textureCoordinate / borderThickness * borderBlock[0].zw + borderBlock[0].xy;
            vec4 frameLeftTop = texture2D(borderTexture, coord);
            src.rgb = blend(src.rgb, frameLeftTop.rgb, frameLeftTop.a);
        }
        else if(textureCoordinate.x <= 1.0 - borderThickness.x)
        {
            coord = vec2(mod((textureCoordinate.x - borderThickness.x), borderLengthHV.x) / borderLengthHV.x, textureCoordinate.y / borderThickness.y) * borderBlock[1].zw + borderBlock[1].xy;
            vec4 frameTop = texture2D(borderTexture, coord);
            src.rgb = blend(src.rgb, frameTop.rgb, frameTop.a);
        }
        else
        {
            coord = vec2((textureCoordinate.x - 1.0 + borderThickness.x) / borderThickness.x, textureCoordinate.y / borderThickness.y) * borderBlock[2].zw + borderBlock[2].xy;
            vec4 frameRightTop = texture2D(borderTexture, coord);
            src.rgb = blend(src.rgb, frameRightTop.rgb, frameRightTop.a);
        }
    }
    else if(textureCoordinate.y <= 1.0 - borderThickness.y)
    {
        if(textureCoordinate.x >= 1.0 - borderThickness.x)
        {
            coord = vec2((textureCoordinate.x - 1.0 + borderThickness.x) / borderThickness.x, mod(textureCoordinate.y - borderThickness.y, borderLengthHV.y) / borderLengthHV.y) * borderBlock[3].zw + borderBlock[3].xy;
            vec4 frameRight = texture2D(borderTexture, coord);
            src.rgb = blend(src.rgb, frameRight.rgb, frameRight.a);
        }
        else if(textureCoordinate.x <= borderThickness.x)
        {
            coord = vec2(textureCoordinate.x / borderThickness.x, mod(textureCoordinate.y - borderThickness.y, borderLengthHV.y) / borderLengthHV.y) * borderBlock[7].zw + borderBlock[7].xy;
            vec4 frameLeft = texture2D(borderTexture, coord);
            src.rgb = blend(src.rgb, frameLeft.rgb, frameLeft.a);
        }
    }	
	else if(textureCoordinate.y >= 1.0 - borderThickness.y)
	{
		if(textureCoordinate.x <= borderThickness.x)
		{
			coord = vec2(textureCoordinate.x / borderThickness.x, (textureCoordinate.y - 1.0 + borderThickness.y) / borderThickness.y) * borderBlock[6].zw + borderBlock[6].xy;
			vec4 frameLeftBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameLeftBottom.rgb, frameLeftBottom.a);
		}
		else if(textureCoordinate.x <= 1.0 - borderThickness.x)
		{
			coord = vec2(mod(textureCoordinate.x - borderThickness.x, borderLengthHV.x) / borderLengthHV.x, (textureCoordinate.y - 1.0 + borderThickness.y) / borderThickness.y) * borderBlock[5].zw + borderBlock[5].xy;
			vec4 frameBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameBottom.rgb, frameBottom.a);
		}
		else
		{
			coord = (textureCoordinate - 1.0 + borderThickness) / borderThickness * borderBlock[4].zw + borderBlock[4].xy;
			vec4 frameRightBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameRightBottom.rgb, frameRightBottom.a);
		}
	}
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverSKR = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform float aspectRatio;)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;
	if(aspectRatio > 1.0)
	{
		coord = textureCoordinate * vec2(aspectRatio, 1.0) - vec2((aspectRatio - 1.0)/2.0, 0.0);
	}
	else
	{
		coord = textureCoordinate / vec2(1.0, aspectRatio) - vec2(0.0, (1.0 / aspectRatio - 1.0)/2.0);
	}
	vec4 tex = texture2D(borderTexture, coord);
	src.rgb = blend(src.rgb, tex.rgb, tex.a);
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverCorners = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec2 borderThickness;
uniform vec4 borderBlocks[4];// leftTop, rightTop, leftBottom, rightBottom;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;

	if(textureCoordinate.x <= borderThickness.x)
	{
		if(textureCoordinate.y <= borderThickness.y)
		{
			coord = textureCoordinate / borderThickness * borderBlocks[0].zw + borderBlocks[0].xy;
			vec4 tex = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, tex.rgb, tex.a);
		}
		else if(textureCoordinate.y >= 1.0 - borderThickness.y)
		{
			coord = vec2(textureCoordinate.x / borderThickness.x, (textureCoordinate.y - 1.0 + borderThickness.y) / borderThickness.y) * borderBlocks[2].zw + borderBlocks[2].xy;
			vec4 tex = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, tex.rgb, tex.a);
		}

	}
	else if(textureCoordinate.x >= 1.0 - borderThickness.x)
	{
		if(textureCoordinate.y <= borderThickness.y)
		{
			coord = vec2((textureCoordinate.x - 1.0 + borderThickness.x) / borderThickness.x, textureCoordinate.y / borderThickness.y) * borderBlocks[1].zw + borderBlocks[1].xy;
			vec4 tex = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, tex.rgb, tex.a);
		}
		else if(textureCoordinate.y >= 1.0 - borderThickness.y)
		{
			coord = (textureCoordinate - 1.0 + borderThickness) / borderThickness * borderBlocks[3].zw + borderBlocks[3].xy;
			vec4 tex = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, tex.rgb, tex.a);
		}
	}
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverUDT = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec4 borderBlocks[3];// upCoverCoord, downCoverCoord, textCoord
uniform vec4 texArea; // = vec4(0.0, 0.6, 0.45215, 0.21);
uniform float lengthH;
uniform float borderThickness;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;
	if(textureCoordinate.y <= borderThickness)
	{
		coord = vec2(mod(textureCoordinate.x, lengthH) / lengthH, textureCoordinate.y / borderThickness) * borderBlocks[0].zw + borderBlocks[0].xy;
		vec4 frameTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameTop.rgb, frameTop.a);
	}
	else if(textureCoordinate.y >= 1.0 - borderThickness)
	{
		coord = vec2(mod(textureCoordinate.x, lengthH) / lengthH, (textureCoordinate.y - 1.0 + borderThickness) / borderThickness) * borderBlocks[1].zw + borderBlocks[1].xy;
		vec4 frameBottom = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameBottom.rgb, frameBottom.a);
	}

	if(textureCoordinate.x >= texArea.x && textureCoordinate.x <= texArea.x + texArea.z
		&& textureCoordinate.y >= texArea.y && textureCoordinate.y <= texArea.y + texArea.w)
	{
		coord = (textureCoordinate - texArea.xy) / texArea.zw * borderBlocks[2].zw + borderBlocks[2].xy;
		vec4 texText = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, texText.rgb, texText.a);
	}

	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverSWithTex = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec4 borderBlocks[2];
uniform vec4 texArea; // = vec4(0.0, 0.6, 0.45215, 0.21);
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	if(textureCoordinate.x >= texArea.x && textureCoordinate.x <= texArea.x + texArea.z
		&& textureCoordinate.y >= texArea.y && textureCoordinate.y <= texArea.y + texArea.w)
	{
		vec2 coord = (textureCoordinate - texArea.xy) / texArea.zw * borderBlocks[1].zw + borderBlocks[1].xy;
		vec4 tex = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, tex.rgb, tex.a);
	}

	{
		vec2 coord = textureCoordinate * borderBlocks[0].zw + borderBlocks[0].xy;
		vec4 tex = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, tex.rgb, tex.a);
	}

	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverSWithTexAndRatio = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec4 borderBlocks[2];
uniform vec4 texArea;
uniform float aspectRatio;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	if(textureCoordinate.x >= texArea.x && textureCoordinate.x <= texArea.x + texArea.z
		&& textureCoordinate.y >= texArea.y && textureCoordinate.y <= texArea.y + texArea.w)
	{
		vec2 coord = (textureCoordinate - texArea.xy) / texArea.zw * borderBlocks[1].zw + borderBlocks[1].xy;
		vec4 texText = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, texText.rgb, texText.a);//src* (1.0 - texText.a) + texText.rgb;
	}

	{
		vec2 coord = textureCoordinate;
		if(aspectRatio > 1.0)
		{
			coord.y = (coord.y - 0.5) / aspectRatio + 0.5;
		}
		else
		{
			coord.x = (coord.x - 0.5) * aspectRatio + 0.5;
		}
		coord = coord * borderBlocks[0].zw + borderBlocks[0].xy;
		vec4 texText = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, texText.rgb, texText.a);//src * (1.0 - texText.a) + texText.rgb;
	}

	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverMWithTex = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec2 borderThickness;
uniform vec2 borderLengthHV;
//leftTop, Top, rightTop, right, rightBottom, bottom, leftBottom, left
uniform vec4 borderBlock[8];
uniform vec4 texBlock;
uniform vec4 texArea;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;
	if(textureCoordinate.x >= texArea.x && textureCoordinate.x <= texArea.x + texArea.z
		&& textureCoordinate.y >= texArea.y && textureCoordinate.y <= texArea.y + texArea.w)
	{
		coord = (textureCoordinate - texArea.xy) / texArea.zw * texBlock.zw + texBlock.xy;
		vec4 tex = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, tex.rgb, tex.a);
	}
	if(textureCoordinate.x <= borderThickness.x && textureCoordinate.y <= borderThickness.y)
	{
		coord = textureCoordinate / borderThickness * borderBlock[0].zw + borderBlock[0].xy;
		vec4 frameLeftTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameLeftTop.rgb, frameLeftTop.a);
	}
	else if(textureCoordinate.x <= 1.0 - borderThickness.x && textureCoordinate.y <= borderThickness.y)
	{
		coord = vec2(mod((textureCoordinate.x - borderThickness.x), borderLengthHV.x) / borderLengthHV.x, textureCoordinate.y / borderThickness.y) * borderBlock[1].zw + borderBlock[1].xy;
		vec4 frameTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameTop.rgb, frameTop.a);
	}
	else if(textureCoordinate.y <= borderThickness.y)
	{
		coord = vec2((textureCoordinate.x - 1.0 + borderThickness.x) / borderThickness.x, textureCoordinate.y / borderThickness.y) * borderBlock[2].zw + borderBlock[2].xy;
		vec4 frameRightTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameRightTop.rgb, frameRightTop.a);
	}
	else if(textureCoordinate.y <= 1.0 - borderThickness.y && textureCoordinate.x >= 1.0 - borderThickness.x)
	{
		coord = vec2((textureCoordinate.x - 1.0 + borderThickness.x) / borderThickness.x, mod(textureCoordinate.y - borderThickness.y, borderLengthHV.y) / borderLengthHV.y) * borderBlock[3].zw + borderBlock[3].xy;
		vec4 frameRight = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameRight.rgb, frameRight.a);
	}
	else if(textureCoordinate.y <= 1.0 - borderThickness.y && textureCoordinate.x <= borderThickness.x)
	{
		coord = vec2(textureCoordinate.x / borderThickness.x, mod(textureCoordinate.y - borderThickness.y, borderLengthHV.y) / borderLengthHV.y) * borderBlock[7].zw + borderBlock[7].xy;
		vec4 frameLeft = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameLeft.rgb, frameLeft.a);
	}
	else if(textureCoordinate.y >= 1.0 - borderThickness.y)
	{
		if(textureCoordinate.x <= borderThickness.x)
		{
			coord = vec2(textureCoordinate.x / borderThickness.x, (textureCoordinate.y - 1.0 + borderThickness.y) / borderThickness.y) * borderBlock[6].zw + borderBlock[6].xy;
			vec4 frameLeftBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameLeftBottom.rgb, frameLeftBottom.a);
		}
		else if(textureCoordinate.x <= 1.0 - borderThickness.x)
		{
			coord = vec2(mod(textureCoordinate.x - borderThickness.x, borderLengthHV.x) / borderLengthHV.x, (textureCoordinate.y - 1.0 + borderThickness.y) / borderThickness.y) * borderBlock[5].zw + borderBlock[5].xy;
			vec4 frameBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameBottom.rgb, frameBottom.a);
		}
		else
		{
			coord = (textureCoordinate - 1.0 + borderThickness) / borderThickness * borderBlock[4].zw + borderBlock[4].xy;
			vec4 frameRightBottom = texture2D(borderTexture, coord);
			src.rgb = blend(src.rgb, frameRightBottom.rgb, frameRightBottom.a);
		}
	}
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverRTAndLBWithTex = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec4 textCoord;
uniform vec4 textArea;
uniform vec4 texRTCoord;
uniform vec4 texRTArea;
uniform vec4 texLBCoord;
uniform vec4 texLBArea;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	if(textureCoordinate.x >= textArea.x && textureCoordinate.x <= textArea.x + textArea.z
		&& textureCoordinate.y >= textArea.y && textureCoordinate.y <= textArea.y + textArea.w)
	{
		vec2 coord = (textureCoordinate - textArea.xy) / textArea.zw * textCoord.zw + textCoord.xy;
		vec4 texText = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, texText.rgb, texText.a);//src* (1.0 - texText.a) + texText.rgb;
	}
	else
	{
		if(textureCoordinate.x >= texRTArea.x && textureCoordinate.x <= texRTArea.x + texRTArea.z
			&& textureCoordinate.y >= texRTArea.y && textureCoordinate.y <= texRTArea.y + texRTArea.w)
		{
			vec2 coord = (textureCoordinate - texRTArea.xy) / texRTArea.zw * texRTCoord.zw + texRTCoord.xy;
			if(coord.x < textCoord.x || coord.x > textCoord.x + textCoord.z
				|| coord.y < textCoord.y || coord.y > textCoord.y + textCoord.w)
			{
				vec4 texRT = texture2D(borderTexture, coord);
				src.rgb = blend(src.rgb, texRT.rgb, texRT.a);//src * (1.0 - texRT.a) + texRT.rgb;
			}
		}

		if(textureCoordinate.x >= texLBArea.x && textureCoordinate.x <= texLBArea.x + texLBArea.z
			&& textureCoordinate.y >= texLBArea.y && textureCoordinate.y <= texLBArea.y + texLBArea.w)
		{
			vec2 coord = (textureCoordinate - texLBArea.xy) / texLBArea.zw * texLBCoord.zw + texLBCoord.xy;
			if(coord.x < textCoord.x || coord.x > textCoord.x + textCoord.z
				|| coord.y < textCoord.y || coord.y > textCoord.y + textCoord.w)
			{
				vec4 texLB = texture2D(borderTexture, coord);
				src.rgb = blend(src.rgb, texLB.rgb, texLB.a);//src * (1.0 - texLB.a) + texLB.rgb;
			}
		}
	}
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverBottomKR = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform vec2 texCoord; // x for thickness and y for lengthH

)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
CGE_SHADER_STRING(

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	if(textureCoordinate.y >= 1.0 - texCoord.x)
	{
		vec2 coord = vec2(mod(textureCoordinate.x, texCoord.y) / texCoord.y, (textureCoordinate.y + texCoord.x - 1.0) / texCoord.x);
		vec4 tex = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, tex.rgb, tex.a);//src * (1.0 - tex.a) + tex.rgb;
	}
	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverUpDown = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform float lengthH;
uniform float borderThickness;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
	CGE_SHADER_STRING(
	void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;
	if(textureCoordinate.y <= borderThickness)
	{
		coord = vec2(mod(textureCoordinate.x, lengthH) / lengthH, textureCoordinate.y / borderThickness);
		vec4 frameTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameTop.rgb, frameTop.a);
	}
	else if(textureCoordinate.y >= 1.0 - borderThickness)
	{
		coord = vec2(mod(textureCoordinate.x, lengthH) / lengthH, (textureCoordinate.y - 1.0 + borderThickness) / borderThickness);
		vec4 frameBottom = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameBottom.rgb, frameBottom.a);
	}

	gl_FragColor = src;
}
);

static CGEConstString s_fshCoverLeftRight = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;
uniform float lengthV;
uniform float borderThickness;
)
#if defined(CGE_TEXTURE_PREMULTIPLIED) && CGE_TEXTURE_PREMULTIPLIED
	BLENDBORDER_PREMULTIPLIED
#else
	BLENDBORDER_NOT_PREMULTIPLIED
#endif
	CGE_SHADER_STRING(
	void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 coord;
	if(textureCoordinate.x <= borderThickness)
	{
		coord = vec2(textureCoordinate.x / borderThickness, mod(textureCoordinate.y, lengthV) / lengthV);
		vec4 frameTop = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameTop.rgb, frameTop.a);
	}
	else if(textureCoordinate.x >= 1.0 - borderThickness)
	{
		coord = vec2((textureCoordinate.x - 1.0 + borderThickness) / borderThickness, mod(textureCoordinate.y, lengthV) / lengthV);
		vec4 frameBottom = texture2D(borderTexture, coord);
		src.rgb = blend(src.rgb, frameBottom.rgb, frameBottom.a);
	}

	gl_FragColor = src;
}
);

//////////////////////////////////////////////////////////////////////////


namespace CGE
{
	CGEBorderTexAlign getTexAlignByName(const char* pName)
	{
		if(strcmp(pName, "LB") == 0)
			return LB;
		else if(strcmp(pName, "CB") == 0)
			return CB;
		else if(strcmp(pName, "RB") == 0)
			return RB;
		else if(strcmp(pName, "LT") == 0)
			return LT;
		else if(strcmp(pName, "CT") == 0)
			return CT;
		else if(strcmp(pName, "RT") == 0)
			return RT;
		else if(strcmp(pName, "CX") == 0)
			return CX;
		else if(strcmp(pName, "XC") == 0)
			return XC;
		else if(strcmp(pName, "CC") == 0)
			return CC;
		else if(strcmp(pName, "XX") != 0)
		{
			CGE_LOG_ERROR("Unrecognised align param: %s\n", pName);
		}
		return XX;
	}

	//////////////////////////////////////////////////////////////////////////
	

	CGEConstString CGEBorderFilterInterface::paramThicknessName = "borderThickness";
	CGEConstString CGEBorderFilterInterface::paramTextureName = "borderTexture";

	void CGEBorderFilterInterface::assignBorderSampler()
	{
		UniformParameters* param;
		if(m_uniformParam == NULL) param = new UniformParameters;
		else param = m_uniformParam;
		param->pushSampler2D(paramTextureName, &m_texture, BORDERBIND_ID);
		setAdditionalUniformParameter(param);
	}

	void CGEBorderFilterInterface::setSamplerID(GLuint samplerID)
	{
		m_texture = samplerID;
	}

	//////////////////////////////////////////////////////////////////////////

	void CGEBorderTexAlignHelper::setTexAlign(CGEBorderTexAlign align, GLfloat w, GLfloat h)
	{
		m_align = align;
		m_texTWidth = w;
		m_texTHeight = h;
		m_texX = -1.0f;
		m_texY = -1.0f;
	}

	void CGEBorderTexAlignHelper::setTexAlign(CGEBorderTexAlign align, GLfloat x, GLfloat y, GLfloat w, GLfloat h)
	{
		m_align = align;
		m_texTWidth = w;
		m_texTHeight = h;
		m_texX = x;
		m_texY = y;
	}

	void CGEBorderTexAlignHelper::setTexAlign(const char* align, GLfloat w, GLfloat h)
	{
		setTexAlign(getTexAlignByName(align), w, h);
	}

	void CGEBorderTexAlignHelper::setTexAlign(const char* align, GLfloat x, GLfloat y, GLfloat w, GLfloat h)
	{
		setTexAlign(getTexAlignByName(align), x, y, w, h);
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGEBorderSFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBorderS);
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString CGEBorderMFilter::paramBorderBlockName = "borderBlock";
	CGEConstString CGEBorderMFilter::paramBorderLengthHVName = "borderLengthHV";

	bool CGEBorderMFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBorderM);
	}

	void CGEBorderMFilter::setBlocks(GLfloat* blockArea, float texRatio)
	{
		m_program.bind();
		GLint location = glGetUniformLocation(m_program.programID(), paramBorderBlockName);
		if(location < 0)
		{
			CGE_LOG_ERROR("CGEBorderMFilter: uniformName %s doesnot exist!\n", paramBorderBlockName);
			return ;
		}
		glUniform4fv(location, 8, blockArea);
		//Not magic code. Using the 2nd blockarea and 4th blockarea.
		m_lengthHParam = blockArea[6] / blockArea[14];
		m_lengthVParam = blockArea[15] / blockArea[7];
		m_cornerRatio = blockArea[2] / blockArea[3] * texRatio;
	}

	void CGEBorderMFilter::setThickness(float thickness)
	{
		m_thickness = thickness;
	}

	void CGEBorderMFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());
		
		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float thicknessX = m_thickness, thicknessY = m_thickness;

			if(m_cornerRatio < 1.0f)
				thicknessX *= m_cornerRatio;
			else
				thicknessY /= m_cornerRatio;

			float srcRatio = float(sz.width) / sz.height;
			if(srcRatio < 1.0f)
				thicknessY *= srcRatio;
			else
				thicknessX /= srcRatio;
			
            m_program.sendUniformf(paramThicknessName, thicknessX, thicknessY);
            
            float lengthH = thicknessX * m_lengthHParam;
            float lengthV = thicknessY * m_lengthVParam;

            float thickLengthX = 1.0f - 2.0f * thicknessX;
            float thickLengthY = 1.0f - 2.0f * thicknessY;

            lengthH = thickLengthX / ceilf(thickLengthX / lengthH);
            lengthV = thickLengthY / ceilf(thickLengthY / lengthV);

			m_program.sendUniformf(paramBorderLengthHVName, lengthH, lengthV);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString CGEBorderCoverSKRFilter::paramStepsRatioName = "aspectRatio";

	bool CGEBorderCoverSKRFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverSKR);
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverCornersFilter::paramBorderBlockName = "borderBlocks";

	bool CGEBorderCoverCornersFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverCorners))
		{
			setThickness(0.1f);
			return true;
		}
		return false;
	}

	void CGEBorderCoverCornersFilter::setBlocks(GLfloat* blocks)
	{
		m_program.bind();
		GLint location = glGetUniformLocation(m_program.programID(), paramBorderBlockName);
		if(location < 0)
		{
			CGE_LOG_ERROR("CGEBorderCoverCornersFilter: uniformName %s doesnot exist!\n", paramBorderBlockName);
			return ;
		}
		glUniform4fv(location, 4, blocks);
	}

	void CGEBorderCoverCornersFilter::setThickness(float thickness)
	{
		m_thickness = thickness;
	}

	void CGEBorderCoverCornersFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float thicknessX, thicknessY;
			if(sz.width < sz.height)
			{
				thicknessX = m_thickness;
				thicknessY = m_thickness * sz.width / sz.height;
			}
			else
			{
				thicknessY = m_thickness;
				thicknessX = m_thickness * sz.height / sz.width;
			}
			m_program.sendUniformf(paramThicknessName, thicknessX, thicknessY);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverUDTFilter::paramBorderBlockName = "borderBlocks";
	CGEConstString CGEBorderCoverUDTFilter::paramTexAreaName = "texArea";
	CGEConstString CGEBorderCoverUDTFilter::paramLengthHName = "lengthH";

	bool CGEBorderCoverUDTFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverUDT);
	}

	void CGEBorderCoverUDTFilter::setBlocks(GLfloat* blocks, GLfloat w, GLfloat h)
	{
		m_program.bind();
		GLint location = glGetUniformLocation(m_program.programID(), paramBorderBlockName);
		if(location < 0)
		{
			CGE_LOG_ERROR("CGEBorderCoverUDTFilter: uniformName %s doesnot exist!\n", paramBorderBlockName);
			return ;
		}
		glUniform4fv(location, 3, blocks);
		m_texAspectRatio = w / h;
		m_TexUDAspectRatio = blocks[2] / blocks[3];
	}

	void CGEBorderCoverUDTFilter::setThickness(float thickness)
	{
		m_thickness = thickness;
	}

	void CGEBorderCoverUDTFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float aspectRatio = float(sz.width) / sz.height;

			float thickness = m_thickness;

			if(aspectRatio < 1.0f)
				thickness *= aspectRatio;			

			float w = m_texTWidth, h = m_texTHeight;
			m_program.sendUniformf(paramThicknessName, thickness);
			m_program.sendUniformf(paramLengthHName, thickness / aspectRatio * m_TexUDAspectRatio * m_texAspectRatio);
			if(aspectRatio >= 1.0f)
				w /= aspectRatio;
			else h *= aspectRatio;
			float x, y;
			switch (m_align)
			{
			case LB:
				x = 0.0f;
				y = 1.0f - thickness - h;
				break;
			case CB:
				x = 0.5f - w / 2.0f;
				y = 1.0f - thickness - h;
				break;
			case RB:
				x = 1.0f - w;
				y = 1.0f - thickness - h;
				break;
			case LT:
				x = 0.0f;
				y = thickness;
				break;
			case CT:
				x = 0.5f - w / 2.0f;
				y = thickness;
				break;
			case RT:
				x = 1.0f - w;
				y = thickness;
				break;
			case CX:
				x = 0.5f - w / 2.0f;
				y = m_texY;
				//assert(m_texY >= 0.0f);
				break;
			case XC:
				x = m_texX;
				y = 0.5f - h / 2.0f;
				break;
			case CC:
				x = 0.5f - w / 2.0f;
				y = 0.5f - h / 2.0f;
				break;
			case XX:
				x = m_texX;
				y = m_texY;
				//assert(m_texX >= 0.0f && m_texY >= 0.0f);
				break;
			default:
				x = 0.0f;
				y = 0.0f;
			}
			m_program.sendUniformf(paramTexAreaName, x, y, w, h);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderSWithTexFilter::paramBorderBlockName = "borderBlocks";
	CGEConstString CGEBorderSWithTexFilter::paramTexAreaName = "texArea";

	bool CGEBorderSWithTexFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverSWithTex);
	}

	void CGEBorderSWithTexFilter::setBlocks(GLfloat* blocks, GLfloat w, GLfloat h)
	{
		m_program.bind();
		GLint location = glGetUniformLocation(m_program.programID(), paramBorderBlockName);
		if(location < 0)
		{
			CGE_LOG_ERROR("CGEBorderSWithTexFilter: uniformName %s doesnot exist!\n", paramBorderBlockName);
			return ;
		}
		glUniform4fv(location, 2, blocks);
		m_texAspectRatio = w / h;
	}

	void CGEBorderSWithTexFilter::setBorderSize(GLfloat borderSize)
	{
		m_borderSize = borderSize;
	}

	void CGEBorderSWithTexFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float aspectRatio = float(sz.width) / sz.height;

			float w = m_texTWidth, h = m_texTHeight;
			if(aspectRatio >= 1.0f)
				w /= aspectRatio;
			else h *= aspectRatio;
			float x, y;
			switch (m_align)
			{
			case LB:
				x = 0.0f;
				y = 1.0f - m_borderSize - h;
				break;
			case CB:
				x = 0.5f - w / 2.0f;
				y = 1.0f - m_borderSize - h;
				break;
			case RB:
				x = 1.0f - w;
				y = 1.0f - m_borderSize - h;
				break;
			case LT:
				x = 0.0f;
				y = m_borderSize;
				break;
			case CT:
				x = 0.5f - w / 2.0f;
				y = m_borderSize;
				break;
			case RT:
				x = 1.0f - w;
				y = m_borderSize;
				break;
			case CX:
				x = 0.5f - w / 2.0f;
				y = m_texY;
				//assert(m_texY >= 0.0f);
				break;
			case XC:
				x = m_texX;
				y = 0.5f - h / 2.0f;
				break;
			case CC:
				x = 0.5f - w / 2.0f;
				y = 0.5f - h / 2.0f;
				break;
			case XX:
				x = m_texX;
				y = m_texY;
				//assert(m_texX >= 0.0f && m_texY >= 0.0f);
				break;
			default:
				x = 0.0f;
				y = 0.0f;
			}
			m_program.sendUniformf(paramTexAreaName, x, y, w, h);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderSWithTexKRFilter::paramAspectRatioName = "aspectRatio";

	bool CGEBorderSWithTexKRFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverSWithTexAndRatio))
		{
			UniformParameters* param = m_uniformParam;
			if(param == NULL)
				param = new UniformParameters;
			param->requireStepsRatio(paramAspectRatioName);
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderMWithTexFilter::paramTexAreaName = "texArea";
	CGEConstString CGEBorderMWithTexFilter::paramTexBlockName = "texBlock";

	bool CGEBorderMWithTexFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverMWithTex);
	}

	void CGEBorderMWithTexFilter::setTexBlock(GLfloat* block, float texRatio)
	{
		m_program.bind();
		GLint location = glGetUniformLocation(m_program.programID(), paramTexBlockName);
		if(location < 0)
		{
			CGE_LOG_ERROR("CGEBorderMWithTexFilter: uniformName %s doesnot exist!\n", paramTexBlockName);
			return ;
		}
		glUniform4fv(location, 1, block);
		m_texAspectRatio = texRatio;
	}

	void CGEBorderMWithTexFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float aspectRatio = float(sz.width) / sz.height;

			float w = m_texTWidth, h = m_texTHeight;
			if(aspectRatio >= 1.0f)
				w /= aspectRatio;
			else h *= aspectRatio;
			float x, y;
			float thicknessX, thicknessY;
			if(sz.width < sz.height)
			{
				thicknessX = m_thickness;
				thicknessY = m_thickness * sz.width / sz.height;
			}
			else
			{
				thicknessY = m_thickness;
				thicknessX = m_thickness * sz.height / sz.width;
			}

			switch (m_align)
			{
			case LB:
				x = m_borderSizeX;
				y = 1.0f - m_borderSizeY - h;
				break;
			case CB:
				x = 0.5f - w / 2.0f;
				y = 1.0f - m_borderSizeY - h;
				break;
			case RB:
				x = 1.0f - w - m_borderSizeX;
				y = 1.0f - m_borderSizeY - h;
				break;
			case LT:
				x = m_borderSizeX;
				y = m_borderSizeY;
				break;
			case CT:
				x = 0.5f - w / 2.0f;
				y = m_borderSizeX;
				break;
			case RT:
				x = 1.0f - w - m_borderSizeX;
				y = m_borderSizeY;
				break;
			case CX:
				x = 0.5f - w / 2.0f;
				y = m_texY;
				//assert(m_texY >= 0.0f);
				break;
			case XC:
				x = m_texX;
				y = 0.5f - h / 2.0f;
				break;
			case CC:
				x = 0.5f - w / 2.0f;
				y = 0.5f - h / 2.0f;
				break;
			case XX:
				x = m_texX;
				y = m_texY;
				//assert(m_texX >= 0.0f && m_texY >= 0.0f);
				break;
			default:
				x = 0.0f;
				y = 0.0f;
			}
			m_program.sendUniformf(paramTexAreaName, x, y, w, h);
			m_program.sendUniformf(paramThicknessName, thicknessX, thicknessY);
			m_program.sendUniformf(paramBorderLengthHVName, thicknessX * m_lengthHParam * m_texAspectRatio, thicknessY * m_lengthVParam / m_texAspectRatio);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramTexAreaName = "textArea";
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramBlockTexName= "textCoord";
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramBlockRTName= "texRTCoord";
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramBlockLBName= "texLBCoord";
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramAreaRTName= "texRTArea";
	CGEConstString CGEBorderCoverRTAndLBWithTexFilter::paramAreaLBName= "texLBArea";

	bool CGEBorderCoverRTAndLBWithTexFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverRTAndLBWithTex);
	}

	void CGEBorderCoverRTAndLBWithTexFilter::setBlocks(GLfloat* blocks)
	{
		for(int i = 0; i != 12; ++i)
			m_blocks[i] = blocks[i];
	}

	void CGEBorderCoverRTAndLBWithTexFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			GLfloat aspectRatio = GLfloat(sz.width) / sz.height;
			m_program.sendUniformf(paramBlockRTName, m_blocks[0], m_blocks[1], m_blocks[2], m_blocks[3]);
			m_program.sendUniformf(paramBlockLBName, m_blocks[4], m_blocks[5], m_blocks[6], m_blocks[7]);
			m_program.sendUniformf(paramBlockTexName, m_blocks[8], m_blocks[9], m_blocks[10], m_blocks[11]);

			float w = 0.5f, h = 0.5f;

			GLfloat x, y;
			GLfloat wt = m_texTWidth;
			GLfloat ht = m_texTHeight;

			switch (m_align)
			{
			case LB:
				x = m_borderSize;
				y = 1.0f - m_borderSize - ht;
				break;
			case CB:
				x = 0.5f - wt / 2.0f;
				y = 1.0f - m_borderSize - ht;
				break;
			case RB:
				x = 1.0f - wt - m_borderSize;
				y = 1.0f - m_borderSize - ht;
				break;
			case LT:
				x = m_borderSize;
				y = m_borderSize;
				break;
			case CT:
				x = 0.5f - wt / 2.0f;
				y = m_borderSize;
				break;
			case RT:
				x = 1.0f - wt - m_borderSize;
				y = m_borderSize;
				break;
			case CX:
				x = 0.5f - wt / 2.0f;
				y = m_texY;
				//assert(m_texY >= 0.0f);
				break;
			case XC:
				x = m_texX;
				y = 0.5f - ht / 2.0f;
				break;
			case CC:
				x = 0.5f - wt / 2.0f;
				y = 0.5f - ht / 2.0f;
				break;
			case XX:
				x = m_texX;
				y = m_texY;
				//assert(m_texX >= 0.0f && m_texY >= 0.0f);
				break;
			default:
				x = 0.0f;
				y = 0.0f;
			}

			if(aspectRatio > 1.0f)
			{
				w /= aspectRatio;
				wt /= aspectRatio;
				x /= aspectRatio;
			}
			else
			{
				h *= aspectRatio;
				ht *= aspectRatio;
				y *= aspectRatio;
			}

			m_program.sendUniformf(paramAreaRTName, 1.0f - w, 0.0f, w, h);
			m_program.sendUniformf(paramAreaLBName, 0.0f, 1.0f - h, w, h);
			m_program.sendUniformf(paramTexAreaName, x, y, wt, ht);			
			
			
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverBottomKRFilter::paramTexCoordName = "texCoord";

	bool CGEBorderCoverBottomKRFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverBottomKR);
	}

	void CGEBorderCoverBottomKRFilter::setBlockShape(GLfloat w, GLfloat h)
	{
		m_thickness = h / w;
	}

	void CGEBorderCoverBottomKRFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			m_program.sendUniformf(paramTexCoordName, m_thickness, float(sz.height) / sz.width);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverUpDownFilter::paramLengthHName = "lengthH";

	bool CGEBorderCoverUpDownFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverUpDown);
	}

	void CGEBorderCoverUpDownFilter::setThickness(float thickness)
	{
		m_thickness = thickness;
	}

	void CGEBorderCoverUpDownFilter::setShapeRatio(float shapeRatio)
	{
		m_shapeRatio = shapeRatio;
	}

	void CGEBorderCoverUpDownFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float aspectRatio = float(sz.width) / sz.height;

			float thickness = m_thickness;

			if(aspectRatio < 1.0f)
				thickness *= aspectRatio;			

			//float w = m_texTWidth, h = m_texTHeight;
			m_program.sendUniformf(paramThicknessName, thickness);
			m_program.sendUniformf(paramLengthHName, thickness / aspectRatio * m_shapeRatio);

		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGEBorderCoverLeftRightFilter::paramLengthVName = "lengthV";

	bool CGEBorderCoverLeftRightFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCoverLeftRight);
	}

	void CGEBorderCoverLeftRightFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float aspectRatio = float(sz.width) / sz.height;

			float thickness = m_thickness;

			if(aspectRatio > 1.0f)
				thickness /= aspectRatio;			

			m_program.sendUniformf(paramThicknessName, thickness);
			m_program.sendUniformf(paramLengthVName, thickness / m_shapeRatio * aspectRatio);

		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

}
