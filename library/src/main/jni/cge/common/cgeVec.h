/*
Author: wysaid
Blog: blog.wysaid.org
Data: 2013-10-31
Description: Provide some cpu math algorithms like glsl shaders.
*/

#ifndef _CGE_VEC_H_
#define _CGE_VEC_H_

#include <cmath>
#include "cgeStaticAssert.h"

#ifndef CGE_CLAMP

#define CGE_CLAMP(n, low, high) do{\
if(n < low) n = low;\
else if(n > high) n = high;\
}while(0)\

#endif

#ifdef __OBJC__

#import <CoreGraphics/CoreGraphics.h>

#endif

namespace CGE
{
	//Only for class Vec.
	template<typename V, int DIM>
	struct VecAlgorithmHelp;  //Check your code, when this is called.

	template<typename V>
	struct VecAlgorithmHelp<V, 1>
	{
		static inline V _plus(const V& v1, const V& v2)
		{
			return V(v1[0] + v2[0]);
		}

		static inline V _minus(const V& v1, const V& v2)
		{
			return V(v1[0] - v2[0]);
		}

		static inline V _times(const V& v1, const V& v2)
		{
			return V(v1[0] * v2[0]);
		}

		static inline V _divide(const V& v1, const V& v2)
		{
			return V(v1[0] / v2[0]);
		}

		static inline V& _inc(V& v1, const V& v2)
		{
			v1[0] += v2[0];
			return v1;
		}

		static inline V& _sub(V& v1, const V& v2)
		{
			v1[0] -= v2[0];
			return v1;
		}

		static inline V& _mul(V& v1, const V& v2)
		{
			v1[0] *= v2[0];
			return v1;
		}

		static inline V& _div(V& v1, const V& v2)
		{
			v1[0] /= v2[0];
			return v1;
		}

		static inline V& _assign(V& v1, const V& v2)
		{
			v1[0] = v2[0];
			return v1;
		}

		//////////////////////////////////////////////////////////////////////////

		static inline V _plus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] + t);
		}

		static inline V _minus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] - t);
		}

		static inline V _times(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] * t);
		}

		static inline V _divide(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] / t);
		}

		static inline V& _inc(V& v1, typename V::VecDataType t)
		{
			v1[0] += t;
			return v1;
		}

		static inline V& _sub(V& v1, typename V::VecDataType t)
		{
			v1[0] -= t;
			return v1;
		}

		static inline V& _mul(V& v1, typename V::VecDataType t)
		{
			v1[0] *= t;
			return v1;
		}

		static inline V& _div(V& v1, typename V::VecDataType t)
		{
			v1[0] /= t;
			return v1;
		}

		static inline V& _assign(V& v1, typename V::VecDataType t)
		{
			v1[0] = t;
			return v1;
		}

		static inline typename V::VecDataType _dot(const V& v1, const V& v2)
		{
			return v1[0]*v2[0];
		}

		static inline typename V::VecDataType _dot(const V& v)
		{
			return v[0]*v[0];
		}

		static inline float _len(const V& v)
		{
            return v[0];
		}

		static inline V& _norm(V& v)
		{
			const float len = 1.0f / _len(v);
			v[0] *= len;
			return v;
		}

		static inline void clamp(V& v, typename V::VecDataType low, typename V::VecDataType high)
		{
			CGE_CLAMP(v[0], low, high);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename V>
	struct VecAlgorithmHelp<V, 2>
	{
		static inline V _plus(const V& v1, const V& v2)
		{
			return V(v1[0] + v2[0], v1[1] + v2[1]);
		}

		static inline V _minus(const V& v1, const V& v2)
		{
			return V(v1[0] - v2[0], v1[1] - v2[1]);
		}

		static inline V _times(const V& v1, const V& v2)
		{
			return V(v1[0] * v2[0], v1[1] * v2[1]);
		}

		static inline V _divide(const V& v1, const V& v2)
		{
			return V(v1[0] / v2[0], v1[1] / v2[1]);
		}

		static inline V& _inc(V& v1, const V& v2)
		{
			v1[0] += v2[0];
			v1[1] += v2[1];
			return v1;
		}

		static inline V& _sub(V& v1, const V& v2)
		{
			v1[0] -= v2[0];
			v1[1] -= v2[1];
			return v1;
		}

		static inline V& _mul(V& v1, const V& v2)
		{
			v1[0] *= v2[0];
			v1[1] *= v2[1];
			return v1;
		}

		static inline V& _div(V& v1, const V& v2)
		{
			v1[0] /= v2[0];
			v1[1] /= v2[1];
			return v1;
		}

		static inline V& _assign(V& v1, const V& v2)
		{
			v1[0] = v2[0];
			v1[1] = v2[1];
			return v1;
		}

		//////////////////////////////////////////////////////////////////////////

		static inline V _plus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] + t, v1[1] + t);
		}

		static inline V _minus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] - t, v1[1] - t);
		}

		static inline V _times(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] * t, v1[1] * t);
		}

		static inline V _divide(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] / t, v1[1] / t);
		}

		static inline V& _inc(V& v1, typename V::VecDataType t)
		{
			v1[0] += t;
			v1[1] += t;
			return v1;
		}

		static inline V& _sub(V& v1, typename V::VecDataType t)
		{
			v1[0] -= t;
			v1[1] -= t;
			return v1;
		}

		static inline V& _mul(V& v1, typename V::VecDataType t)
		{
			v1[0] *= t;
			v1[1] *= t;
			return v1;
		}

		static inline V& _div(V& v1, typename V::VecDataType t)
		{
			v1[0] /= t;
			v1[1] /= t;
			return v1;
		}

		static inline V& _assign(V& v1, typename V::VecDataType t)
		{
			v1[0] = t;
			v1[1] = t;
			return v1;
		}

		static inline typename V::VecDataType _dot(const V& v1, const V& v2)
		{
			return v1[0]*v2[0] + v1[1]*v2[1];
		}

		static inline typename V::VecDataType _dot(const V& v)
		{
			return v[0]*v[0] + v[1]*v[1];
		}

		static inline float _len(const V& v)
		{
			return sqrtf(v[0]*v[0] + v[1]*v[1]);
		}

		static inline V& _norm(V& v)
		{
			const float len = 1.0f / _len(v);
			v[0] *= len;
			v[1] *= len;
			return v;
		}

		static inline void clamp(V& v, typename V::VecDataType low, typename V::VecDataType high)
		{
			CGE_CLAMP(v[0], low, high);
			CGE_CLAMP(v[1], low, high);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename V>
	struct VecAlgorithmHelp<V, 3>
	{
		static inline V _plus(const V& v1, const V& v2)
		{
			return V(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
		}

		static inline V _minus(const V& v1, const V& v2)
		{
			return V(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
		}

		static inline V _times(const V& v1, const V& v2)
		{
			return V(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
		}

		static inline V _divide(const V& v1, const V& v2)
		{
			return V(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
		}

		static inline V& _inc(V& v1, const V& v2)
		{
			v1[0] += v2[0];
			v1[1] += v2[1];
			v1[2] += v2[2];			
			return v1;
		}

		static inline V& _sub(V& v1, const V& v2)
		{
			v1[0] -= v2[0];
			v1[1] -= v2[1];
			v1[2] -= v2[2];			
			return v1;
		}

		static inline V& _mul(V& v1, const V& v2)
		{
			v1[0] *= v2[0];
			v1[1] *= v2[1];
			v1[2] *= v2[2];			
			return v1;
		}

		static inline V& _div(V& v1, const V& v2)
		{
			v1[0] /= v2[0];
			v1[1] /= v2[1];
			v1[2] /= v2[2];			
			return v1;
		}

		static inline V& _assign(V& v1, const V& v2)
		{
			v1[0] = v2[0];
			v1[1] = v2[1];
			v1[2] = v2[2];			
			return v1;
		}

		//////////////////////////////////////////////////////////////////////////

		static inline V _plus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] + t, v1[1] + t, v1[2] + t);
		}

		static inline V _minus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] - t, v1[1] - t, v1[2] - t);
		}

		static inline V _times(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] * t, v1[1] * t, v1[2] * t);
		}

		static inline V _divide(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] / t, v1[1] / t, v1[2] / t);
		}

		static inline V& _inc(V& v1, typename V::VecDataType t)
		{
			v1[0] += t;
			v1[1] += t;
			v1[2] += t;			
			return v1;
		}

		static inline V& _sub(V& v1, typename V::VecDataType t)
		{
			v1[0] -= t;
			v1[1] -= t;
			v1[2] -= t;			
			return v1;
		}

		static inline V& _mul(V& v1, typename V::VecDataType t)
		{
			v1[0] *= t;
			v1[1] *= t;
			v1[2] *= t;			
			return v1;
		}

		static inline V& _div(V& v1, typename V::VecDataType t)
		{
			v1[0] /= t;
			v1[1] /= t;
			v1[2] /= t;			
			return v1;
		}

		static inline V& _assign(V& v1, typename V::VecDataType t)
		{
			v1[0] = t;
			v1[1] = t;
			v1[2] = t;			
			return v1;
		}

		static inline typename V::VecDataType _dot(const V& v1, const V& v2)
		{
			return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
		}

		static inline typename V::VecDataType _dot(const V& v)
		{
			return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
		}

		static inline float _len(const V& v)
		{
			return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		}

		static inline V& _norm(V& v)
		{
			const float len = 1.0f / _len(v);
			v[0] *= len;
			v[1] *= len;
			v[2] *= len;			
			return v;
		}

		static inline void clamp(V& v, typename V::VecDataType low, typename V::VecDataType high)
		{
			CGE_CLAMP(v[0], low, high);
			CGE_CLAMP(v[1], low, high);
			CGE_CLAMP(v[2], low, high);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	template<typename V>
	struct VecAlgorithmHelp<V, 4>
	{
		static inline V _plus(const V& v1, const V& v2)
		{
			return V(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2], v1[3] + v2[3]);
		}

		static inline V _minus(const V& v1, const V& v2)
		{
			return V(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2], v1[3] - v2[3]);
		}

		static inline V _times(const V& v1, const V& v2)
		{
			return V(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2], v1[3] * v2[3]);
		}

		static inline V _divide(const V& v1, const V& v2)
		{
			return V(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2], v1[3] / v2[3]);
		}

		static inline V& _inc(V& v1, const V& v2)
		{
			v1[0] += v2[0];
			v1[1] += v2[1];
			v1[2] += v2[2];
			v1[3] += v2[3];
			return v1;
		}

		static inline V& _sub(V& v1, const V& v2)
		{
			v1[0] -= v2[0];
			v1[1] -= v2[1];
			v1[2] -= v2[2];
			v1[3] -= v2[3];
			return v1;
		}

		static inline V& _mul(V& v1, const V& v2)
		{
			v1[0] *= v2[0];
			v1[1] *= v2[1];
			v1[2] *= v2[2];
			v1[3] *= v2[3];
			return v1;
		}

		static inline V& _div(V& v1, const V& v2)
		{
			v1[0] /= v2[0];
			v1[1] /= v2[1];
			v1[2] /= v2[2];
			v1[3] /= v2[3];
			return v1;
		}

		static inline V& _assign(V& v1, const V& v2)
		{
			v1[0] = v2[0];
			v1[1] = v2[1];
			v1[2] = v2[2];
			v1[3] = v2[3];
			return v1;
		}

		//////////////////////////////////////////////////////////////////////////

		static inline V _plus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] + t, v1[1] + t, v1[2] + t, v1[3] + t);
		}

		static inline V _minus(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] - t, v1[1] - t, v1[2] - t, v1[3] - t);
		}

		static inline V _times(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] * t, v1[1] * t, v1[2] * t, v1[3] * t);
		}

		static inline V _divide(const V& v1, typename V::VecDataType t)
		{
			return V(v1[0] / t, v1[1] / t, v1[2] / t, v1[3] / t);
		}

		static inline V& _inc(V& v1, typename V::VecDataType t)
		{
			v1[0] += t;
			v1[1] += t;
			v1[2] += t;
			v1[3] += t;
			return v1;
		}

		static inline V& _sub(V& v1, typename V::VecDataType t)
		{
			v1[0] -= t;
			v1[1] -= t;
			v1[2] -= t;
			v1[3] -= t;
			return v1;
		}

		static inline V& _mul(V& v1, typename V::VecDataType t)
		{
			v1[0] *= t;
			v1[1] *= t;
			v1[2] *= t;
			v1[3] *= t;
			return v1;
		}

		static inline V& _div(V& v1, typename V::VecDataType t)
		{
			v1[0] /= t;
			v1[1] /= t;
			v1[2] /= t;
			v1[3] /= t;
			return v1;
		}

		static inline V& _assign(V& v1, typename V::VecDataType t)
		{
			v1[0] = t;
			v1[1] = t;
			v1[2] = t;
			v1[3] = t;
			return v1;
		}

		static inline typename V::VecDataType _dot(const V& v1, const V& v2)
		{
			return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
		}

		static inline typename V::VecDataType _dot(const V& v)
		{
			return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
		}

		static inline float _len(const V& v)
		{
			return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
		}

		static inline V& _norm(V& v)
		{
			const float len = 1.0f / _len(v);
			v[0] *= len;
			v[1] *= len;
			v[2] *= len;
			v[3] *= len;
			return v;
		}

		static inline void clamp(V& v, typename V::VecDataType low, typename V::VecDataType high)
		{
			CGE_CLAMP(v[0], low, high);
			CGE_CLAMP(v[1], low, high);
			CGE_CLAMP(v[2], low, high);
			CGE_CLAMP(v[3], low, high);
		}

	};

}

namespace CGE
{

	template<typename Type, int DIM>
	class Vec
	{
	public:
		enum { VEC_DIM = DIM };
		typedef Type VecDataType;

		Vec() { cgeStaticAssert(DIM > 0 && DIM <= 4);}
		Vec(const Vec& v) { VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_assign(*this, v); }
		Vec(Type x)
		{
			cgeStaticAssert(DIM == 1); //DIM should be 1
			m_data[0] = x;
		}

		Vec(Type x, Type y)
		{
			cgeStaticAssert(DIM == 2); //DIM should be 2
			m_data[0] = x;
			m_data[1] = y;
		}

		Vec(Type x, Type y, Type z)
		{
			cgeStaticAssert(DIM == 3); //DIM should be 3
			m_data[0] = x;
			m_data[1] = y;
			m_data[2] = z;
		}
		Vec(Type x, Type y, Type z, Type w)
		{
			cgeStaticAssert(DIM == 4); //DIM should be 4
			m_data[0] = x;
			m_data[1] = y;
			m_data[2] = z;
			m_data[3] = w;
		}

		inline Type& operator[](int index)
		{
			return m_data[index];
		}

		inline const Type& operator[](int index) const
		{
			return m_data[index];
		}

		inline Vec operator+(const Vec& v)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_plus(*this, v);
		}

		inline Vec operator+(Type t) const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_plus(*this, t);
		}

		inline friend Vec operator+(Type t, Vec& v)
		{
			return v + t;
		}

		inline Vec& operator+=(const Vec& v)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_inc(*this, v);
		}

		inline Vec& operator+=(Type t)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_inc(*this, t);
		}

		inline Vec operator-(const Vec& v)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_minus(*this, v);
		}

		inline Vec operator-(Type t)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_minus(*this, t);
		}

		inline Vec& operator-=(const Vec& v)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_sub(*this, v);
		}

		inline Vec& operator-=(Type t)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_sub(*this, t);
		}

		inline Vec operator*(const Vec& v)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_times(*this, v);
		}

		inline Vec operator*(Type t)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_times(*this, t);
		}

		inline Vec& operator*=(const Vec& v)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_mul(*this, v);
		}

		inline Vec& operator*=(Type t)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_mul(*this, t);
		}

		inline Vec operator/(const Vec& v)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_divide(*this, v);
		}

		inline Vec operator/(Type t)const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_divide(*this, t);
		}

		inline Vec& operator/=(const Vec& v)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_div(*this, v);
		}

		inline Vec& operator/=(Type t)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_div(*this, t);
		}

		inline Vec& operator=(const Vec& v)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_assign(*this, v);
		}

		inline Vec& operator=(Type t)
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_assign(*this, t);
		}

		inline Type dot(const Vec& v) const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_dot(*this, v);
		}

		inline Type dot() const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_dot(*this);
		}

		inline Type length() const
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_len(*this);
		}

		inline Vec& normalize()
		{
			return VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_norm(*this);
		}

		inline void clamp(Type low, Type high)
		{
			VecAlgorithmHelp<Vec<Type, DIM>, DIM>::_clamp(*this, low, high);
		}

		inline Type& x() { cgeStaticAssert(DIM >= 1); return m_data[0]; }
		inline Type& y() { cgeStaticAssert(DIM >= 2); return m_data[1]; }
		inline Type& z() { cgeStaticAssert(DIM >= 3); return m_data[2]; }
		inline Type& w() { cgeStaticAssert(DIM >= 4); return m_data[3]; }
		inline Type& r() { cgeStaticAssert(DIM >= 1); return m_data[0]; }
		inline Type& g() { cgeStaticAssert(DIM >= 2); return m_data[1]; }
		inline Type& b() { cgeStaticAssert(DIM >= 3); return m_data[2]; }
		inline Type& a() { cgeStaticAssert(DIM >= 4); return m_data[3]; }

		inline const Type& x() const { cgeStaticAssert(DIM >= 1); return m_data[0]; }
		inline const Type& y() const { cgeStaticAssert(DIM >= 2); return m_data[1]; }
		inline const Type& z() const { cgeStaticAssert(DIM >= 3); return m_data[2]; }
		inline const Type& w() const { cgeStaticAssert(DIM >= 4); return m_data[3]; }
		inline const Type& r() const { cgeStaticAssert(DIM >= 1); return m_data[0]; }
		inline const Type& g() const { cgeStaticAssert(DIM >= 2); return m_data[1]; }
		inline const Type& b() const { cgeStaticAssert(DIM >= 3); return m_data[2]; }
		inline const Type& a() const { cgeStaticAssert(DIM >= 4); return m_data[3]; }

		inline Type& get(int index) { return m_data[index]; }
		inline const Type& get(int index) const { return m_data[index]; }

		inline Vec<Type, 1> subvec(int fst) const { return Vec<Type, 1>(m_data[fst]); }
		inline Vec<Type, 2> subvec(int fst, int scd) const { return Vec<Type, 2>(m_data[fst], m_data[scd]); }
		inline Vec<Type, 3> subvec(int fst, int scd, int trd) const { return Vec<Type, 3>(m_data[fst], m_data[scd], m_data[trd]); }
		inline Vec<Type, 4> subvec(int fst, int scd, int trd, int fth) const { return Vec<Type, 4>(m_data[fst], m_data[scd], m_data[trd], m_data[fth]); }


#ifdef __OBJC__
        
        inline operator CGPoint() const { cgeStaticAssert(DIM == 2); return CGPointMake(m_data[0], m_data[1]); }
        inline operator CGSize() const { cgeStaticAssert(DIM == 2); return CGSizeMake(m_data[0], m_data[1]); }
        inline Vec& operator=(const CGPoint& p) { cgeStaticAssert(DIM == 2); m_data[0] = p.x; m_data[1] = p.y; }
        
#endif
        
	
	private:
		Type m_data[DIM];
	};

	typedef Vec<char, 1> Vec1b;
	typedef Vec<unsigned char, 1> Vec1ub;
	typedef Vec<short, 1> Vec1s;
	typedef Vec<unsigned short, 1> Vec1us;
	typedef Vec<int, 1> Vec1i;
	typedef Vec<unsigned, 1> Vec1ui;
	typedef Vec1ui Vec1u;
	typedef Vec<long, 1> Vec1l;
	typedef Vec<unsigned long, 1> Vec1ul;
	typedef Vec<long long, 1> Vec1ll;
	typedef Vec<unsigned long long, 1> Vec1ull;
	typedef Vec<float, 1> Vec1f;
	typedef Vec<double, 1> Vec1d;
	typedef Vec<long double, 1> Vec1ld;

	typedef Vec<char, 2> Vec2b;
	typedef Vec<unsigned char, 2> Vec2ub;
	typedef Vec<short, 2> Vec2s;
	typedef Vec<unsigned short, 2> Vec2us;
	typedef Vec<int, 2> Vec2i;
	typedef Vec<unsigned, 2> Vec2ui;
	typedef Vec2ui Vec2u;
	typedef Vec<long, 2> Vec2l;
	typedef Vec<unsigned long, 2> Vec2ul;
	typedef Vec<long long, 2> Vec2ll;
	typedef Vec<unsigned long long, 2> Vec2ull;
	typedef Vec<float, 2> Vec2f;
	typedef Vec<double, 2> Vec2d;
	typedef Vec<long double, 2> Vec2ld;

	typedef Vec<char, 3> Vec3b;
	typedef Vec<unsigned char, 3> Vec3ub;
	typedef Vec<short, 3> Vec3s;
	typedef Vec<unsigned short, 3> Vec3us;
	typedef Vec<int, 3> Vec3i;
	typedef Vec<unsigned, 3> Vec3ui;
	typedef Vec3ui Vec3u;
	typedef Vec<long, 3> Vec3l;
	typedef Vec<unsigned long, 3> Vec3ul;
	typedef Vec<long long, 3> Vec3ll;
	typedef Vec<unsigned long long, 3> Vec3ull;
	typedef Vec<float, 3> Vec3f;
	typedef Vec<double, 3> Vec3d;
	typedef Vec<long double, 3> Vec3ld;

	typedef Vec<char, 4> Vec4b;
	typedef Vec<unsigned char, 4> Vec4ub;
	typedef Vec<short, 4> Vec4s;
	typedef Vec<unsigned short, 4> Vec4us;
	typedef Vec<int, 4> Vec4i;
	typedef Vec<unsigned, 4> Vec4ui;
	typedef Vec4ui Vec4u;
	typedef Vec<long, 4> Vec4l;
	typedef Vec<unsigned long, 4> Vec4ul;
	typedef Vec<long long, 4> Vec4ll;
	typedef Vec<unsigned long long, 4> Vec4ull;
	typedef Vec<float, 4> Vec4f;
	typedef Vec<double, 4> Vec4d;
	typedef Vec<long double, 4> Vec4ld;

	inline Vec3f crossV3f(Vec3f left, Vec3f right)
	{
		return Vec3f(left[1] * right[2] - left[2] * right[1],
			left[2] * right[0] - left[0] * right[2],
			left[0] * right[1] - left[1] * right[0]);
	}

	inline Vec3f projectV3f(Vec3f v3ToProj, Vec3f projVec)
	{
		return projVec * (projVec.dot(v3ToProj) / projVec.dot());
	}

}

#endif
