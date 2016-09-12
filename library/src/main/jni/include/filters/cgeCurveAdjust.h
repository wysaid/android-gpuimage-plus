/*
* cgeCurveAdjust.h
*
*  Created on: 2014-1-2
*      Author: Wang Yang
*/

#ifndef _CGECURVE_ADJUST_H_
#define _CGECURVE_ADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGECurveInterface
	{
		//It's a helper.
		//You should not make instances of this class.
	protected:
		CGECurveInterface() {}
		virtual ~CGECurveInterface() {}

	public:
		typedef struct CurvePoint
		{
			float x, y;
			//You may call any function like std::sort,
			// to order them by increasing sequence;
            bool operator<(const CurvePoint& cp) const
			{
				return x < cp.x;
			}
		}CurvePoint;

		typedef struct CurveData
		{
			float data[3];

			inline float& operator[](int index)
			{
				return data[index];
			}

			inline const float& operator[](int index) const
			{
				return data[index];
			}
		}CurveData;

		enum { CHANNEL_R, CHANNEL_G, CHANNEL_B};

		static void resetCurve(std::vector<float>& vec, size_t precision);
		static bool genCurve(std::vector<float>& vec, const CurvePoint* pnts, size_t cnt);
		static bool mergeCurveConst(std::vector<float>& dst, const std::vector<float>& late, const std::vector<float>& early);
		//Allow mergeCurve to scale the early and late to the same length.
		static bool mergeCurve(std::vector<float>& dst, std::vector<float>& late, std::vector<float>& early);
		static void scaleCurve(std::vector<float>& vec, size_t precision);
		static bool loadCurve(std::vector<float>& vec, const float* curve, size_t cnt);

		//////////////////////////////////////////////////////////////////////////

		static void resetCurve(std::vector<CurveData>& vec, size_t precision);
		static bool genCurve(std::vector<CurveData>& vec, const CurvePoint* pntsR, size_t cntR, 
			const CurvePoint* pntsG, size_t cntG, const CurvePoint* pntsB, size_t cntB);

		static bool genCurve(std::vector<CurveData>& vec, const CurvePoint* pnts, size_t cnt, size_t channel);

		static bool mergeCurveConst(std::vector<CurveData>& dst, const std::vector<CurveData>& late, const std::vector<CurveData>& early);
		//Allow mergeCurve to scale the early and late to the same length.
		static bool mergeCurve(std::vector<CurveData>& dst, std::vector<CurveData>& late, std::vector<CurveData>& early);
		static bool mergeCurve(std::vector<CurveData>& dst, std::vector<float>& late, std::vector<CurveData>& early, unsigned channel);
		static void scaleCurve(std::vector<CurveData>& vec, size_t precision);

		//curveRGB's length is 3 times as cnt, ordered by r-g-b
		static bool loadCurve(std::vector<CurveData>& vec, const float* curve, size_t cnt, size_t dstChannel, size_t srcChannel, size_t stride);

		static int getPrecision();
		static inline CurvePoint makeCurvePoint(float x, float y)
		{
			CurvePoint pnt;
			pnt.x = x;
			pnt.y = y;
			return pnt;
		}

		//Sets the curve points(the useful points on the curve line.
		//If you only need the special channel(s), set the other(s) to "NULL"
		virtual void setPoints(const CurvePoint* r, size_t nr,
			const CurvePoint* g, size_t ng,
			const CurvePoint* b, size_t nb);

		virtual void setPointsRGB(const CurvePoint* pnts, size_t cnt);

		virtual void setPointsR(const CurvePoint* pnts, size_t cnt);
		virtual void setPointsG(const CurvePoint* pnts, size_t cnt);
		virtual void setPointsB(const CurvePoint* pnts, size_t cnt);

		virtual void loadCurves(const float* curveR, size_t nR,
			const float* curveG, size_t nG,
			const float* curveB, size_t nB);

		//The return value is an array ordered by r-g-b sequence
		inline const float* getCurveRGB(size_t* len = nullptr) const { if(len != nullptr) *len = m_curve.size(); return &m_curve[0][0]; };

		inline std::vector<CurveData>& getCurveData() { return m_curve; }
		inline void resetCurve() { CGECurveInterface::resetCurve(m_curve, CGE_CURVE_PRECISION); }

	protected:
		std::vector<CurveData> m_curve;

		//Be sure that the size of "curve" is right.
		//channel: 1或3
		//stride: 0,1,2
		static bool _genCurve(float* curve, const CurvePoint* pnts, size_t cnt, unsigned channel = 1, unsigned stride = 0);

		static void _assignCurveArrays(ProgramObject& program, CGEConstString name, std::vector<CurveData>& data);
		static void _assignCurveSampler(GLuint& texID, std::vector<CurveData>& data);
	};

	class CGECurveTexFilter : public CGEImageFilterInterface, public CGECurveInterface
	{
	public:
		CGECurveTexFilter() : m_curveTexture(0){}
		virtual ~CGECurveTexFilter(){glDeleteTextures(1, &m_curveTexture); m_curveTexture = 0;}
		bool init();
		//////////////////////////////////////////////////////////////////////////

		void setPoints(const CurvePoint* r, size_t nr,
			const CurvePoint* g, size_t ng,
			const CurvePoint* b, size_t nb);

		void setPointsRGB(const CurvePoint* pnts, size_t cnt);

		void setPointsR(const CurvePoint* pnts, size_t cnt);
		void setPointsG(const CurvePoint* pnts, size_t cnt);
		void setPointsB(const CurvePoint* pnts, size_t cnt);

		void loadCurves(const float* curveR, size_t nR,
			const float* curveG, size_t nG,
			const float* curveB, size_t nB);

	protected:
		void initSampler();
		void flush();

	protected:
		GLuint m_curveTexture;
	};

	class CGECurveFilter  : public CGEImageFilterInterface, public CGECurveInterface
	{
	public:
		CGECurveFilter(){}
		~CGECurveFilter() {}

		bool init();
		//////////////////////////////////////////////////////////////////////////

		void setPoints(const CurvePoint* r, size_t nr,
			const CurvePoint* g, size_t ng,
			const CurvePoint* b, size_t nb);

		void setPointsRGB(const CurvePoint* pnts, size_t cnt);

		void setPointsR(const CurvePoint* pnts, size_t cnt);
		void setPointsG(const CurvePoint* pnts, size_t cnt);
		void setPointsB(const CurvePoint* pnts, size_t cnt);

		void loadCurves(const float* curveR, size_t nR,
			const float* curveG, size_t nG,
			const float* curveB, size_t nB);
	protected:
		void flush();

	};


	//////////////////////////////////////////////////////////////////////////

	//See more information in "BasicAdjusts::CGECurveInterface"
	class CGEMoreCurveFilter : public CGEImageFilterInterface, public CGECurveInterface
	{
	public:
		CGEMoreCurveFilter(){}

		virtual bool init();

		void pushPoints(const CurvePoint* r, size_t nr,
			const CurvePoint* g, size_t ng,
			const CurvePoint* b, size_t nb);

		void pushPointsRGB(const CurvePoint* rgb, size_t nrgb);
		void pushPointsR(const CurvePoint* r, size_t nr);
		void pushPointsG(const CurvePoint* g, size_t ng);
		void pushPointsB(const CurvePoint* b, size_t nb);

		void pushCurves(const float* curveR, size_t nR,
			const float* curveG, size_t nG,
			const float* curveB, size_t nB);

		//You must call "flush" after you finishing pushing all curves.
		virtual void flush();
	};


	//为了结构层次清晰， 与CGECurveTexFilter 存在部分相似代码
	class CGEMoreCurveTexFilter : public CGEMoreCurveFilter
	{
	public:
		CGEMoreCurveTexFilter() : m_curveTexture(0){}
		virtual ~CGEMoreCurveTexFilter(){glDeleteTextures(1, &m_curveTexture); m_curveTexture = 0;}

		bool init();

		//You must call "flush" after you finishing pushing all curves.
		virtual void flush();

	protected:
		void initSampler();

	protected:
		GLuint m_curveTexture;
	};
}

#endif 
