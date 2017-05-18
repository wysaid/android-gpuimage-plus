/*
* cgeCurveAdjust.cpp
*
*  Created on: 2014-1-2
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeCurveAdjust.h"
#include "cgeCommonDefine.h"

#define CURVE_BIND_TEXTURE_ID 0


/*
const static char* const s_fshCurveMap = SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D curveTexture; //We do not use sampler1D because GLES dosenot support that.
uniform float intensity;

void main()
{
	vec3 src = texture2D(inputImageTexture, textureCoordinate).rgb;
	vec3 dst = vec3(texture2D(curveTexture, vec2(src.r, 0.0)).r,
					texture2D(curveTexture, vec2(src.g, 0.0)).g,
					texture2D(curveTexture, vec2(src.b, 0.0)).b);
	gl_FragColor = vec4(mix(src, dst, intensity), 1.0);
}
);
 */

namespace CGE
{
	void CGECurveInterface::loadCurves(const float* curveR, size_t nR, const float* curveG, size_t nG, const float* curveB, size_t nB)
	{
		if(curveR != nullptr)
			CGECurveInterface::loadCurve(m_curve, curveR, nR, CHANNEL_R, 1u, 0u);
		if(curveG != nullptr)
			CGECurveInterface::loadCurve(m_curve, curveG, nG, CHANNEL_G, 1u, 0u);
		if(curveB != nullptr)
			CGECurveInterface::loadCurve(m_curve, curveB, nB, CHANNEL_B, 1u, 0u);
	}

	bool CGECurveInterface::loadCurve(std::vector<float>& vec, const float* curve, size_t cnt)
	{
		if(curve == nullptr || cnt <= 1)
		{
			CGECurveInterface::resetCurve(vec, CGE_CURVE_PRECISION);
			return false;
		}
		if(vec.size() != cnt)
			vec.resize(cnt);
		vec.resize(cnt);
		for(int i = 0; i != cnt; ++i)
		{
			vec[i] = curve[i];
		}
		return true;
	}

	bool CGECurveInterface::loadCurve(std::vector<CurveData>& vec, const float* curve, size_t cnt, size_t dstChannel, size_t srcChannel, size_t stride)
	{
		if(curve == nullptr || cnt <= 1 || srcChannel == 0 || dstChannel >= 3)
		{
			CGECurveInterface::resetCurve(vec, CGE_CURVE_PRECISION);
			return false;
		}
		if(vec.size() != cnt)
			vec.resize(cnt);
		for(int i = 0; i != cnt; ++i)
		{
			vec[i][(int)dstChannel] = curve[i*srcChannel + stride];
		}
		return true;
	}

	int CGECurveInterface::getPrecision()
	{
		return CGE_CURVE_PRECISION;
	}

	void CGECurveInterface::setPoints(const CurvePoint* r, size_t nr, const CurvePoint* g, size_t ng, const CurvePoint* b, size_t nb)
	{
		CGECurveInterface::genCurve(m_curve, r, nr, g, ng, b, nb);
	}

	void CGECurveInterface::setPointsRGB(const CurvePoint* pnts, size_t cnt)
	{
		std::vector<float> curve(CGE_CURVE_PRECISION);
		if(CGECurveInterface::genCurve(curve, pnts, cnt))
		{
			CGECurveInterface::loadCurve(m_curve, curve.data(), curve.size(), CHANNEL_R, 1u, 0u);
			CGECurveInterface::loadCurve(m_curve, curve.data(), curve.size(), CHANNEL_G, 1u, 0u);
			CGECurveInterface::loadCurve(m_curve, curve.data(), curve.size(), CHANNEL_B, 1u, 0u);
		}
	}

	void CGECurveInterface::setPointsR(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::genCurve(m_curve, pnts, cnt, CHANNEL_R);
	}

	void CGECurveInterface::setPointsG(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::genCurve(m_curve, pnts, cnt, CHANNEL_G);
	}

	void CGECurveInterface::setPointsB(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::genCurve(m_curve, pnts, cnt, CHANNEL_B);
	}

	void CGECurveInterface::resetCurve(std::vector<float>& vec, size_t precision)
	{
		if(vec.size() != precision) vec.resize(precision);
		for(int i = 0; i != precision; ++i)
		{
			vec[i] = float(i) / (precision - 1);
		}
	}

	void CGECurveInterface::resetCurve(std::vector<CurveData>& vec, size_t precision)
	{
		if(vec.size() != precision) vec.resize(precision);
		for(int i = 0; i != precision; ++i)
		{
			const float tmp = float(i) / (precision - 1);
			vec[i][0] = tmp;
			vec[i][1] = tmp;
			vec[i][2] = tmp;
		}
	}

	void CGECurveInterface::scaleCurve(std::vector<float>& vec, size_t precision)
	{
		const std::vector<float>::size_type sz = vec.size();
		if(sz == precision) return;
		if(sz == 0)
		{
			CGECurveInterface::resetCurve(vec, precision);
			return;
		}

		std::vector<float> vTmp(precision);
		float scale = float(sz - 1) / (precision - 1);
		for(std::vector<float>::size_type t = 0; t != precision; ++t)
		{
			const size_t index = CGE_MID(size_t(t * scale), size_t(0), (sz - 1));
			vTmp[t] = vec[index];
		}
		vec = vTmp;
	}

	void CGECurveInterface::scaleCurve(std::vector<CurveData>& vec, size_t precision)
	{
		const std::vector<float>::size_type sz = vec.size();
		if(sz == precision) return;
		if(sz == 0)
		{
			CGECurveInterface::resetCurve(vec, precision);
			return;
		}

		std::vector<CurveData> vTmp(precision);
		float scale = float(sz - 1) / (precision - 1);
		for(std::vector<CurveData>::size_type t = 0; t != precision; ++t)
		{
			const size_t index = CGE_MID(size_t(t * scale), size_t(0), (sz - 1));
			vTmp[t][0] = vec[index][0];
			vTmp[t][1] = vec[index][1];
			vTmp[t][2] = vec[index][2];
		}
		vec = vTmp;
	}

	bool CGECurveInterface::mergeCurveConst(std::vector<float>& dst, const std::vector<float>& late, const std::vector<float>& early)
	{
		if(early.empty() || early.size() != late.size())
			return false; //You should call "scaleCurve" to make the sizes equal. (none-zero)

		const std::vector<float>::size_type sz = early.size();
		if(dst.size() != sz) dst.resize(sz);
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const size_t index = CGE_MID(size_t(early[t] * (sz - 1)), size_t(0), (sz - 1));
			dst[t] = late[index];
		}
		return true;
	}

	bool CGECurveInterface::mergeCurveConst(std::vector<CurveData>& dst, const std::vector<CurveData>& late, const std::vector<CurveData>& early)
	{
		if(early.empty() || early.size() != late.size())
			return false; //You should call "scaleCurve" to make the sizes equal. (none-zero)

		const std::vector<float>::size_type sz = early.size();
		if(dst.size() != sz) dst.resize(sz);
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const size_t indexR = CGE_MID(size_t(early[t][0] * (sz - 1)), size_t(0), (sz - 1));
			dst[t][0] = late[indexR][0];

			const size_t indexG = CGE_MID(size_t(early[t][1] * (sz - 1)), size_t(0), (sz - 1));
			dst[t][1] = late[indexG][1];

			const size_t indexB = CGE_MID(size_t(early[t][2] * (sz - 1)), size_t(0), (sz - 1));
			dst[t][2] = late[indexB][2];
		}
		return true;
	}

	bool CGECurveInterface::mergeCurve(std::vector<float>& dst, std::vector<float>& late, std::vector<float>& early)
	{
		if(early.size() != late.size())
		{
			scaleCurve(late, CGE_CURVE_PRECISION);
			scaleCurve(early, CGE_CURVE_PRECISION);
		}
		return mergeCurveConst(dst, late, early);
	}

	bool CGECurveInterface::mergeCurve(std::vector<CurveData>& dst, std::vector<CurveData>& late, std::vector<CurveData>& early)
	{
		if(early.size() != late.size())
		{
			scaleCurve(late, CGE_CURVE_PRECISION);
			scaleCurve(early, CGE_CURVE_PRECISION);
		}
		return mergeCurveConst(dst, late, early);
	}

	bool CGECurveInterface::mergeCurve(std::vector<CurveData>& dst, std::vector<float>& late, std::vector<CurveData>& early, unsigned channel)
	{
		if(early.empty() || channel >= 3)
			return false;

		if(early.size() != late.size())
		{
			scaleCurve(late, CGE_CURVE_PRECISION);
			scaleCurve(early, CGE_CURVE_PRECISION);
		}

		const std::vector<float>::size_type sz = early.size();
		if(dst.size() != sz) dst.resize(sz);
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const size_t index = CGE_MID(size_t(early[t][channel] * (sz - 1)), size_t(0), (sz - 1));
			dst[t][channel] = late[index];
		}
		return true;
	}

	//Be sure that the size of "curve" is right.(CURVE_PRECISION * channel)
	bool CGECurveInterface::_genCurve(float* curve, const CurvePoint* pnts, size_t cnt, unsigned channel, unsigned stride)
	{
		if(curve == nullptr)
			return false;

		std::vector<float> u(cnt - 1), ypp(cnt);
		ypp[0] = u[0] = 0.0f;
		for(int i=1; i != cnt - 1; ++i)
		{
			float sig = (pnts[i].x - pnts[i - 1].x) / (pnts[i + 1].x - pnts[i - 1].x);
			float p = sig * ypp[i - 1] + 2.0f;
			ypp[i] = (sig - 1.0f) / p;
			u[i] = ((pnts[i + 1].y - pnts[i].y)/ (pnts[i + 1].x - pnts[i].x) - (pnts[i].y - pnts[i - 1].y) / (pnts[i].x - pnts[i - 1].x));
			u[i] = (6.0f * u[i] / (pnts[i + 1].x - pnts[i - 1].x) - sig * u[i - 1]) / p;
		}
		ypp[cnt - 1] = 0.0;
		for(int i = (int)(cnt - 2); i >= 0; --i)
		{
			ypp[i] = ypp[i] * ypp[i+1] + u[i];
		}
		int kL = -1, kH = 0;
		for(int i = 0; i != CGE_CURVE_PRECISION; ++i)
		{
			const float t = (float)i/(CGE_CURVE_PRECISION - 1);
			while(kH < (int)cnt && t > pnts[kH].x)
			{
				kL = kH;
				++kH;
			}
			if(kH == cnt)
			{
				curve[i * channel + stride] = pnts[cnt-1].y;
				continue;
			}
			if(kL == -1)
			{
				curve[i * channel + stride] = pnts[0].y;
				continue;
			}
			const float h = pnts[kH].x - pnts[kL].x;
			const float a = (pnts[kH].x - t) / h;
			const float b = (t - pnts[kL].x) / h;
			const float g = a * pnts[kL].y + b*pnts[kH].y + ((a*a*a - a)*ypp[kL] + (b*b*b - b) * ypp[kH]) * (h*h) / 6.0f;
			curve[i * channel + stride] = CGE_MID(g, 0.0f, 1.0f);
		}
		return true;
	}

	bool CGECurveInterface::genCurve(std::vector<float>& vec, const CurvePoint* pnts, size_t cnt)
	{
		if(vec.size() != CGE_CURVE_PRECISION) vec.resize(CGE_CURVE_PRECISION);
		if(cnt <= 1 || pnts == nullptr)
		{
			resetCurve(vec, CGE_CURVE_PRECISION);
			CGE_LOG_ERROR("Invalid Curve Points! Ptr: %p, Count: %d", pnts, (int)cnt);
			return false;
		}
		return _genCurve(vec.data(), pnts, cnt);
	}

	bool CGECurveInterface::genCurve(std::vector<CurveData>& vec, const CurvePoint* pntsR, size_t cntR, const CurvePoint* pntsG, size_t cntG, const CurvePoint* pntsB, size_t cntB)
	{
		if(vec.size() != CGE_CURVE_PRECISION) vec.resize(CGE_CURVE_PRECISION);
		if(cntR <= 1 || pntsR == nullptr ||
			cntG <= 1 || pntsG == nullptr ||
			cntB <= 1 || pntsB == nullptr)
		{
			resetCurve(vec, CGE_CURVE_PRECISION);
			CGE_LOG_ERROR("Invalid Curve Points!\nR: %p, Count: %d\nG: %p, Count: %d\nB: %p, Count: %d\n", pntsR, (int)cntR, pntsG, (int)cntG, pntsB, (int)cntB);
			return false;
		}

		return _genCurve(&vec[0][0], pntsR, cntR, 3, 0) &&
			_genCurve(&vec[0][0], pntsG, cntG, 3, 1) &&
			_genCurve(&vec[0][0], pntsB, cntB, 3, 2);
	}

	bool CGECurveInterface::genCurve(std::vector<CurveData>& vec, const CurvePoint* pnts, size_t cnt, size_t channel)
	{
		if(vec.size() != CGE_CURVE_PRECISION) vec.resize(CGE_CURVE_PRECISION);
		if(cnt <= 1 || pnts == nullptr || channel > 3)
		{
			resetCurve(vec, CGE_CURVE_PRECISION);
			CGE_LOG_ERROR("Invalid Curve Points! Ptr: %p, Count: %d\n", pnts, (int)cnt);
			return false;
		}
		return _genCurve(&vec[0][0], pnts, cnt, 3, (int)channel);
	}

	void CGECurveInterface::_assignCurveArrays(ProgramObject& program, CGEConstString name, std::vector<CurveData>& data)
	{
		program.bind();
		GLint index = program.uniformLocation(name);
		if(index < 0)
		{
			CGE_LOG_ERROR("CGECurveFilter: Failed when assignCurveArray()\n");
			return;
		}
		glUniform3fv(index, (GLsizei)data.size(), &data[0][0]);
	}

	void CGECurveInterface::_assignCurveSampler(GLuint& texID, std::vector<CurveData>& data)
	{
		std::vector<float>::size_type sz = data.size();
		if(sz != CGE_CURVE_PRECISION)
		{
			scaleCurve(data, CGE_CURVE_PRECISION);
			sz = CGE_CURVE_PRECISION;
		}

#ifdef CGE_NOT_OPENGL_ES

		
		const GLenum dataType = GL_FLOAT;
		const void* samplerData = &data[0][0];		

#else

		const GLenum dataType = GL_UNSIGNED_BYTE;

		const CurveData* curveData = data.data();
		unsigned char samplerData[CGE_CURVE_PRECISION * 3];
		for(int i = 0; i != CGE_CURVE_PRECISION; ++i)
		{
			const int index = i * 3;
			samplerData[index] = (unsigned char)(curveData[i][0] * 255.0f);
			samplerData[index + 1] = (unsigned char)(curveData[i][1] * 255.0f);
			samplerData[index + 2] = (unsigned char)(curveData[i][2] * 255.0f);
		}

#endif

		if(texID == 0)
        {
			texID = cgeGenTextureWithBuffer(samplerData, int(sz), 1, GL_RGB, dataType, 3, 0, GL_LINEAR, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (int)sz, 1, GL_RGB, dataType, samplerData);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGECurveTexFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, g_fshCurveMapNoIntensity))
		{
			initSampler();
			return true;
		}
		return false;
	}

	void CGECurveTexFilter::flush()
	{
		_assignCurveSampler(m_curveTexture, m_curve);
	}

	void CGECurveTexFilter::initSampler()
	{		
		flush();
		UniformParameters* param;
		if(m_uniformParam == nullptr) param = new UniformParameters;
		else param = m_uniformParam;
		param->pushSampler2D(g_paramCurveMapTextureName, &m_curveTexture, CURVE_BIND_TEXTURE_ID);
		setAdditionalUniformParameter(param);
	}

	void CGECurveTexFilter::setPoints(const CurvePoint* r, size_t nr, const CurvePoint* g, size_t ng, const CurvePoint* b, size_t nb)
	{
		CGECurveInterface::setPoints(r, nr, g, ng, b, nb);
		flush();
	}

	void CGECurveTexFilter::setPointsRGB(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsRGB(pnts, cnt);
		flush();
	}

	void CGECurveTexFilter::setPointsR(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsR(pnts, cnt);
		flush();
	}

	void CGECurveTexFilter::setPointsG(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsG(pnts, cnt);
		flush();
	}

	void CGECurveTexFilter::setPointsB(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsB(pnts, cnt);
		flush();
	}

	void CGECurveTexFilter::loadCurves(const float* curveR, size_t nR, const float* curveG, size_t nG, const float* curveB, size_t nB)
	{
		CGECurveInterface::loadCurves(curveR, nR, curveG, nG, curveB, nB);
		flush();
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGECurveFilter::init()
	{
		resetCurve(m_curve, CGE_CURVE_PRECISION);

		return initShadersFromString(g_vshDefaultWithoutTexCoord, g_fshFastAdjust);
	}

	void CGECurveFilter::setPoints(const CurvePoint* r, size_t nr, const CurvePoint* g, size_t ng, const CurvePoint* b, size_t nb)
	{
		CGECurveInterface::setPoints(r, nr, g, ng, b, nb);
		flush();
	}

	void CGECurveFilter::setPointsRGB(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsRGB(pnts, cnt);
		flush();
	}

	void CGECurveFilter::setPointsR(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsR(pnts, cnt);
		flush();
	}
	void CGECurveFilter::setPointsG(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsG(pnts, cnt);
		flush();
	}
	void CGECurveFilter::setPointsB(const CurvePoint* pnts, size_t cnt)
	{
		CGECurveInterface::setPointsB(pnts, cnt);
		flush();
	}

	void CGECurveFilter::loadCurves(const float* curveR, size_t nR, const float* curveG, size_t nG,	const float* curveB, size_t nB)
	{
		CGECurveInterface::loadCurves(curveR, nR, curveG, nG, curveB, nB);
		flush();
	}

	void CGECurveFilter::flush()
	{
		_assignCurveArrays(m_program, g_paramFastAdjustArrayName, m_curve);
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGEMoreCurveFilter::init()
	{
		resetCurve(m_curve, CGE_CURVE_PRECISION);
		return initShadersFromString(g_vshDefaultWithoutTexCoord, g_fshFastAdjust);
	}

	void CGEMoreCurveFilter::pushPoints(const CurvePoint* r, size_t nr, const CurvePoint* g, size_t ng, const CurvePoint* b, size_t nb)
	{
		pushPointsR(r, nr);
		pushPointsG(g, ng);
		pushPointsB(b, nb);
	}

	void CGEMoreCurveFilter::pushPointsRGB(const CurvePoint* rgb, size_t nrgb)
	{
		std::vector<float> vecRGB;
		if(rgb != nullptr && nrgb >= 2)
		{
			genCurve(vecRGB, rgb, nrgb);
			mergeCurve(m_curve, vecRGB, m_curve, 0);
			mergeCurve(m_curve, vecRGB, m_curve, 1);
			mergeCurve(m_curve, vecRGB, m_curve, 2);
		}
	}

	void CGEMoreCurveFilter::pushPointsR(const CurvePoint* r, size_t nr)
	{
		std::vector<float> vecR;
		if(r != nullptr && nr >= 2)
		{
			genCurve(vecR, r, nr);
			mergeCurve(m_curve, vecR, m_curve, 0);
		}
	}

	void CGEMoreCurveFilter::pushPointsG(const CurvePoint* g, size_t ng)
	{
		std::vector<float> vecG;
		if(g != nullptr && ng >= 2)
		{
			genCurve(vecG, g, ng);
			mergeCurve(m_curve, vecG, m_curve, 1);
		}
	}

	void CGEMoreCurveFilter::pushPointsB(const CurvePoint* b, size_t nb)
	{
		std::vector<float> vecB;
		if(b != nullptr && nb >= 2)
		{
			genCurve(vecB, b, nb);
			mergeCurve(m_curve, vecB, m_curve, 2);
		}
	}

	void CGEMoreCurveFilter::pushCurves(const float* curveR, size_t nR, const float* curveG, size_t nG, const float* curveB, size_t nB)
	{

		if(curveR != nullptr && nR > 0)
		{
			std::vector<float> vecR(curveR, curveR + nR);
			mergeCurve(m_curve, vecR, m_curve, 0);
		}
		if(curveG != nullptr && nG > 0)
		{
			std::vector<float> vecG(curveG, curveG + nG);
			mergeCurve(m_curve, vecG, m_curve, 1);
		}
		if(curveB != nullptr && nB > 0)
		{
			std::vector<float> vecB(curveB, curveB + nB);
			mergeCurve(m_curve, vecB, m_curve, 2);
		}
	}

	void CGEMoreCurveFilter::flush()
	{
		_assignCurveArrays(m_program, g_paramFastAdjustArrayName, m_curve);
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGEMoreCurveTexFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, g_fshCurveMapNoIntensity))
		{
			initSampler();
			return true;
		}
		return false;
	}

	void CGEMoreCurveTexFilter::flush()
	{
		_assignCurveSampler(m_curveTexture, m_curve);
	}

	void CGEMoreCurveTexFilter::initSampler()
	{
		flush();
		UniformParameters* param;
		if(m_uniformParam == nullptr) param = new UniformParameters;
		else param = m_uniformParam;
		param->pushSampler2D(g_paramCurveMapTextureName, &m_curveTexture, 0);
		setAdditionalUniformParameter(param);
	}

}