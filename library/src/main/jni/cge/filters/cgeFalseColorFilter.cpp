/*
 * cgeFalseColorFilter.cpp
 */

#include "cgeFalseColorFilter.h"

static CGEConstString s_fshFalseColor = CGE_SHADER_STRING_PRECISION_M
(
    varying vec2 textureCoordinate;

    uniform sampler2D inputImageTexture;

    uniform float intensity;

    const float perPixel = 1.0 / 255.0;

    //const vec3 W = vec3(0.2125, 0.7154, 0.0721);

    void main()
        {
        /*
            float luminance = texture2D(inputImageTexture, textureCoordinate).r;
            gl_FragColor = texture2D(lookupTexture, vec2(luminance, 1.0));
        */
             vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);

             //float luminance = dot(texture2D(inputImageTexture, textureCoordinate).rgb, W);

             float luminance = dot(textureColor.rgb, vec3(0.299, 0.587, 0.114));

             vec3 replaceColor;

             vec3 changeColor[13];

             if (luminance >= 0.0 && luminance <= 23.2 * perPixel)
             {//紫色
                changeColor[0] = vec3(196.0,21.0,148.0) * perPixel;
             }
             else if (luminance > 23.2 * perPixel && luminance <= 35.5 * perPixel)
             {//深蓝色
                changeColor[1] = vec3(8.0,76.0,141.0) * perPixel;
             }
             else if (luminance > 35.5 * perPixel && luminance <= 51.3 * perPixel)
             {//蓝色
                changeColor[2] = vec3(5.0,104.0,143.0) * perPixel;
             }
             else if (luminance > 51.3 * perPixel && luminance <= 69.0 * perPixel)
             {//浅蓝色
                changeColor[3] = vec3(40.0,154.0,164.0) * perPixel;
             }
    /*
             else if (luminance > 69.0 * perPixel && luminance <= 101.0 * perPixel)
             {//深灰色
                changeColor[4] = vec3(174.0,154.0,153.0) * perPixel;
             }

             else if (luminance > 101.0 * perPixel && luminance <= 110.5 * perPixel)
             {//绿色
                changeColor[5] = vec3(116.0,197.0,17.0) * perPixel;
             }
             else if (luminance > 110.5 * perPixel && luminance <= 137.8 * perPixel)
             {//灰色
                changeColor[6] = vec3(164.0,136.0,136.0) * perPixel;
             }
    */
             else if (luminance > 69.0 * perPixel && luminance <= 101.0 * perPixel)
             {//深灰色
                changeColor[4] = vec3(174.0,154.0,153.0) * perPixel;
             }

             else if (luminance > 101.0 * perPixel && luminance <= 110.0 * perPixel)
             {//绿色
                changeColor[5] = vec3(116.0,197.0,17.0) * perPixel;
             }
             else if (luminance > 110.0 * perPixel && luminance <= 139.0 * perPixel)
             {//灰色
                changeColor[6] = vec3(164.0,136.0,136.0) * perPixel;
             }

             else if (luminance > 139.0 * perPixel && luminance <= 148.0 * perPixel)
             {//粉色
                changeColor[7] = vec3(247.0,103.0,95.0) * perPixel;
             }
             else if (luminance > 148.0 * perPixel && luminance <= 185.0 * perPixel)
             {//浅灰色
                changeColor[8] = vec3(249.0,226.0,220.0) * perPixel;
             }
             else if (luminance > 185.0 * perPixel && luminance <= 204.0 * perPixel)
             {//浅黄色
                changeColor[9] = vec3(250.0,250.0,160.0) * perPixel;
             }
             else if (luminance > 204.0 * perPixel && luminance <= 220.0 * perPixel)
             {//黄色
                changeColor[10] = vec3(255.0,255.0,0.0) * perPixel;
             }

             else if (luminance > 220.0 * perPixel && luminance <= 231.5 * perPixel)
             {
                changeColor[11] = vec3(253.0,118.0,0.0) * perPixel;
             }
             else if (luminance > 231.5 * perPixel && luminance <= 255.0 * perPixel)
             {//红色
                changeColor[12] = vec3(255.0,0.0,10.0) * perPixel;
             }

            replaceColor = changeColor[0] + changeColor[1] + changeColor[2] + changeColor[3] + changeColor[4]
                         + changeColor[5] + changeColor[6] + changeColor[7] + changeColor[8] + changeColor[9]
                         + changeColor[10] + changeColor[11] + changeColor[12];

            gl_FragColor = vec4(mix(textureColor.rgb,replaceColor, intensity), textureColor.a);

        }
/*     {

         //float luminance = texture2D(inputImageTexture, textureCoordinate).r;
         //gl_FragColor = texture2D(lookupTexture, vec2(luminance, 1.0));

          vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);

          float luminance = dot(texture2D(inputImageTexture, textureCoordinate).rgb, W);

          vec3 replaceColor;

          vec3 changeColor[13];

          if (luminance >= 0.0 && luminance <= 9.0 * perPixel)
          {//紫色
             changeColor[0] = vec3(196.0,21.0,148.0) * perPixel;
          }
          else if (luminance > 9.0 * perPixel && luminance <= 23.0 * perPixel)
          {//深蓝色
             changeColor[1] = vec3(8.0,76.0,141.0) * perPixel;
          }
          else if (luminance > 23.0 * perPixel && luminance <= 41.0 * perPixel)
          {//蓝色
             changeColor[2] = vec3(5.0,104.0,143.0) * perPixel;
          }
          else if (luminance > 41.0 * perPixel && luminance <= 63.0 * perPixel)
          {//浅蓝色
             changeColor[3] = vec3(40.0,154.0,164.0) * perPixel;
          }
          else if (luminance > 63.0 * perPixel && luminance <= 104.0 * perPixel)
          {//深灰色
             changeColor[4] = vec3(174.0,154.0,153.0) * perPixel;
          }

          else if (luminance > 104.0 * perPixel && luminance <= 107.0 * perPixel)
          {//绿色
             changeColor[5] = vec3(116.0,197.0,17.0) * perPixel;
          }
          else if (luminance > 107.0 * perPixel && luminance <= 137.8 * perPixel)
          {//灰色
             changeColor[6] = vec3(164.0,136.0,136.0) * perPixel;
          }
          else if (luminance > 137.8 * perPixel && luminance <= 156.0 * perPixel)
          {//粉色
             changeColor[7] = vec3(247.0,103.0,95.0) * perPixel;
          }
          else if (luminance > 156.0 * perPixel && luminance <= 192.0 * perPixel)
          {//浅灰色
             changeColor[8] = vec3(249.0,226.0,220.0) * perPixel;
          }
          else if (luminance > 192.0 * perPixel && luminance <= 208.0 * perPixel)
          {//浅黄色
             changeColor[9] = vec3(250.0,250.0,160.0) * perPixel;
          }

          else if (luminance > 208.0 * perPixel && luminance <= 229.0 * perPixel)
          {//黄色
             changeColor[10] = vec3(255.0,255.0,0.0) * perPixel;
          }
          else if (luminance > 229.0 * perPixel && luminance <= 249.5 * perPixel)
          {
             changeColor[11] = vec3(253.0,118.0,0.0) * perPixel;
          }
          else if (luminance > 249.5 * perPixel && luminance <= 255.0 * perPixel)
          {//红色
             changeColor[12] = vec3(255.0,0.0,10.0) * perPixel;
          }

         replaceColor = changeColor[0] + changeColor[1] + changeColor[2] + changeColor[3] + changeColor[4]
                      + changeColor[5] + changeColor[6] + changeColor[7] + changeColor[8] + changeColor[9]
                      + changeColor[10] + changeColor[11] + changeColor[12];

         gl_FragColor = vec4(mix(textureColor.rgb,replaceColor, intensity), textureColor.a);

     }
*/
);

namespace CGE
{
	CGEConstString CGEFalseColorFilter::paramIntensity = "intensity";

	bool CGEFalseColorFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshFalseColor)){
            setIntensity(1.0);
		    return true;
		}

		return false;
	}

	void CGEFalseColorFilter::setIntensity(float i)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensity, i);
	}
}