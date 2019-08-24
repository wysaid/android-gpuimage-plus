/*
Author: wysaid
Blog: blog.wysaid.org
Data: 2013-10-31
Description: Provide some cpu math algorithms like glsl shaders.
@             Some algorithms are from http://www.opengl.org/
@Attention: I'm using radian, not degree!
*/

#ifndef _CGE_MAT_H_
#define _CGE_MAT_H_

#include <cmath>
#include <algorithm>
#include "cgeStaticAssert.h"
#include "cgeVec.h"

#ifndef M_PI
#define M_PI 3.141592653589793f
#endif

namespace CGE
{
	inline void normalize(float& x, float& y, float& z)
	{
		const float scale = 1.0f / sqrtf(x*x + y*y + z*z);
		x *= scale;
		y *= scale;
		z *= scale;
	}

	inline void normalize(float& x, float& y, float& z, float& w)
	{
		const float scale = 1.0f / sqrtf(x*x + y*y + z*z + w*w);
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}

	struct Mat2
	{
		const static Mat2& makeIdentity()
		{
			const static Mat2 sIdentity =  Mat2(1.0f, 0.0f,
				0.0f, 1.0f);
			return sIdentity;
		}

		static inline Mat2& makeMatrix(Mat2& m, float m00, float m01,
			float m10, float m11)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[1][0] = m10;
			m[1][1] = m11;
			return m;
		}

		static inline Mat2 makeRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat2(cosRad, sinRad,	-sinRad, cosRad);
		}

		//////////////////////////////////////////////////////////////////////////

		Mat2() {}
		Mat2(float m00, float m01, float m10, float m11) 
		{
			makeMatrix(*this, m00, m01, m10, m11);
		}

		inline void rotate(float rad)
		{
			*this *= makeRotation(rad);
		}

		inline float* operator[](int index)
		{
			return data[index];
		}

		inline const float* operator[](int index) const
		{
			return data[index];
		}

		inline Vec2f operator*(const Vec2f& v) const
		{
			return Vec2f(data[0][0] * v[0] + data[1][0] * v[1],
				data[0][1] * v[0] + data[1][1] * v[1]);
		}

		inline Mat2 operator*(const Mat2& m) const
		{
			return Mat2(data[0][0] * m[0][0] + data[1][0] * m[0][1],
				data[0][1] * m[0][0] + data[1][1] * m[0][1],
				data[0][0] * m[1][0] + data[1][0] * m[1][1],				
				data[0][1] * m[1][0] + data[1][1] * m[1][1]
			);
		}

		inline Mat2& operator*=(const Mat2& m)
		{
			return *this = *this * m;
		}

		inline void loadIdentity()
		{
			*this = makeIdentity();
		}

		float data[2][2];
	};

	struct Mat3
	{
		const static Mat3& makeIdentity()
		{
			const static Mat3 sIdentity =  Mat3(1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f);
			return sIdentity;
		}

		static inline Mat3& makeMatrix(Mat3& m, float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			return m;
		}

		static inline Mat3 makeRotation(float rad, float x, float y, float z)
		{
			normalize(x, y, z);
			const float cosRad = cosf(rad);
			const float cosp = 1.0f - cosRad;
			const float sinRad = sinf(rad);		

			return Mat3(cosRad + cosp * x * x,
				cosp * x * y + z * sinRad,
				cosp * x * z - y * sinRad,
				cosp * x * y - z * sinRad,
				cosRad + cosp * y * y,
				cosp * y * z + x * sinRad,
				cosp * x * z + y * sinRad,
				cosp * y * z - x * sinRad,
				cosRad + cosp * z * z);
		}

		static inline Mat3 makeXRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat3(1.0f, 0.0f, 0.0f,
				0.0f, cosRad, sinRad,
				0.0f, -sinRad, cosRad);
		}

		static inline Mat3 makeYRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat3(cosRad, 0.0f, -sinRad,
				0.0f, 1.0f, 0.0f,
				sinRad, 0.0f, cosRad);
		}

		static inline Mat3 makeZRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat3(cosRad, sinRad, 0.0f,
				-sinRad, cosRad, 0.0f,
				0.0f, 0.0f, 1.0f);
		}

		//////////////////////////////////////////////////////////////////////////

		Mat3() {}
		Mat3(float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22)
		{
			makeMatrix(*this, m00, m01, m02,
				m10, m11, m12,
				m20, m21, m22);
		}

		inline float* operator[](int index)
		{
			return data[index];
		}

		inline const float* operator[](int index) const
		{
			return data[index];
		}

		inline Vec3f operator*(const Vec3f& v) const
		{
			return Vec3f(data[0][0] * v[0] + data[1][0] * v[1] + data[2][0] * v[2],
				data[0][1] * v[0] + data[1][1] * v[1] + data[2][1] * v[2],
				data[0][2] * v[0] + data[1][2] * v[1] + data[2][2] * v[2]);
		}

		inline Mat3 operator*(const Mat3& m) const
		{
			return Mat3(data[0][0] * m[0][0] + data[1][0] * m[0][1] + data[2][0] * m[0][2],
				data[0][1] * m[0][0] + data[1][1] * m[0][1] + data[2][1] * m[0][2],
				data[0][2] * m[0][0] + data[1][2] * m[0][1] + data[2][2] * m[0][2],

				data[0][0] * m[1][0] + data[1][0] * m[1][1] + data[2][0] * m[1][2],
				data[0][1] * m[1][0] + data[1][1] * m[1][1] + data[2][1] * m[1][2],
				data[0][2] * m[1][0] + data[1][2] * m[1][1] + data[2][2] * m[1][2],

				data[0][0] * m[2][0] + data[1][0] * m[2][1] + data[2][0] * m[2][2],
				data[0][1] * m[2][0] + data[1][1] * m[2][1] + data[2][1] * m[2][2],
				data[0][2] * m[2][0] + data[1][2] * m[2][1] + data[2][2] * m[2][2]);
		}

		inline Mat3& operator*=(const Mat3& m)
		{
			return *this = *this * m;
		}

		inline void rotate(float rad, float x, float y, float z)
		{
			*this *= makeRotation(rad, x, y, z);
		}

		inline void rotateX(float rad)
		{
			*this *= makeXRotation(rad); //待优化
		}

		inline void rotateY(float rad)
		{
			*this *= makeYRotation(rad); //待优化
		}

		inline void rotateZ(float rad)
		{
			*this *= makeZRotation(rad); //待优化
		}

		inline void loadIdentity()
		{
			*this = makeIdentity();
		}

		float data[3][3];
	};

	struct Mat4
	{
		const static inline Mat4& makeIdentity()
		{
			const static Mat4 sIdentity =  Mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			return sIdentity;
		}

		static inline Mat4& makeMatrix(Mat4& m, float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[0][3] = m03;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[1][3] = m13;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			m[2][3] = m23;
			m[3][0] = m30;
			m[3][1] = m31;
			m[3][2] = m32;
			m[3][3] = m33;
			return m;
		}

		static inline Mat4& makeMatrixTranspose(Mat4& m, float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			m[0][0] = m00;
			m[0][1] = m10;
			m[0][2] = m20;
			m[0][3] = m30;
			m[1][0] = m01;
			m[1][1] = m11;
			m[1][2] = m21;
			m[1][3] = m31;
			m[2][0] = m02;
			m[2][1] = m12;
			m[2][2] = m22;
			m[2][3] = m32;
			m[3][0] = m03;
			m[3][1] = m13;
			m[3][2] = m23;
			m[3][3] = m33;
			return m;
		}

		static inline Mat4 makeMatrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{	
			return Mat4(m00,  m01,  m02,  m03,
				m10,  m11,  m12,  m13,
				m20,  m21,  m22,  m23,
				m30,  m31,  m32,  m33);
		}

		static inline Mat4 makeMatrixTranspose(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			return Mat4(m00,  m10,  m20,  m30,
				m01,  m11,  m21,  m31,
				m02,  m12,  m22,  m32,
				m03,  m13,  m23,  m33);
		}

		static inline Mat4 makeTranslation(float x, float y, float z)
		{
			return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x, y, z, 1.0f);
		}

		static inline Mat4 makeScale(float x, float y, float z)
		{
			return Mat4(x, 0.0f, 0.0f, 0.0f,
				0.0f, y, 0.0f, 0.0f,
				0.0f, 0.0f, z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		static inline Mat4 makeRotation(float rad, float x, float y, float z)
		{
			normalize(x, y, z);
			const float cosRad = cosf(rad);
			const float cosp = 1.0f - cosRad;
			const float sinRad = sinf(rad);		

			return Mat4(cosRad + cosp * x * x,
				cosp * x * y + z * sinRad,
				cosp * x * z - y * sinRad,
				0.0f,
				cosp * x * y - z * sinRad,
				cosRad + cosp * y * y,
				cosp * y * z + x * sinRad,
				0.0f,
				cosp * x * z + y * sinRad,
				cosp * y * z - x * sinRad,
				cosRad + cosp * z * z,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f);
		}

		static inline Mat4 makeXRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, cosRad, sinRad, 0.0f,
				0.0f, -sinRad, cosRad, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		static inline Mat4 makeYRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat4(cosRad, 0.0f, -sinRad, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				sinRad, 0.0f, cosRad, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		static inline Mat4 makeZRotation(float rad)
		{
			const float cosRad = cosf(rad);
			const float sinRad = sinf(rad);
			return Mat4(cosRad, sinRad, 0.0f, 0.0f,
				-sinRad, cosRad, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		static inline Mat4 makePerspective(float fovyRad, float aspect, float nearZ, float farZ)
		{
			const float cotan = 1.0f / tanf(fovyRad / 2.0f);
			return Mat4(cotan / aspect, 0.0f, 0.0f, 0.0f,
				0.0f, cotan, 0.0f, 0.0f,
				0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
				0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f);
		}

		static inline Mat4 makeFrustum(float left, float right, float bottom, float top, float nearZ, float farZ)
		{
			const float ral = right + left;
			const float rsl = right - left;
			const float tsb = top - bottom;
			const float tab = top + bottom;
			const float fan = farZ + nearZ;
			const float fsn = farZ - nearZ;

			return Mat4(2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f,
				ral / rsl, tab / tsb, -fan / fsn, -1.0f,
				0.0f, 0.0f, (-2.0f * farZ * nearZ) / fsn, 0.0f);
		}

		static inline Mat4 makeOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
		{
			const float ral = right + left;
			const float rsl = right - left;
			const float tsb = top - bottom;
			const float tab = top + bottom;
			const float fan = farZ + nearZ;
			const float fsn = farZ - nearZ;

			return Mat4(2.0f / rsl, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / tsb, 0.0f, 0.0f,
				0.0f, 0.0f, -2.0f / fsn, 0.0f,
				-ral / rsl, -tab / tsb, -fan / fsn, 1.0f);
		}

		static inline Mat4 makeLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ,	float upX, float upY, float upZ)
		{
			Vec3f ev(eyeX, eyeY, eyeZ);
			Vec3f cv(centerX, centerY, centerZ);
			Vec3f uv(upX, upY, upZ);
			return makeLookAt(ev, cv, uv);
		}

		static inline Mat4 makeLookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up)
		{
			Vec3f forward((eye - center).normalize());
			Vec3f side(crossV3f(up, forward).normalize());
			Vec3f upVector(crossV3f(forward, side));

			return Mat4(side[0], upVector[0], forward[0], 0.0f,
				side[1], upVector[1], forward[1], 0.0f,
				side[2], upVector[2], forward[2], 0.0f,
				-side.dot(eye),
				-upVector.dot(eye),
				-forward.dot(eye),
				1.0f);
		}

		static inline Mat4 mat4Rotate(Mat4& m, float rad, float x, float y, float z)
		{
			return m * makeRotation(rad, x, y, z);
		}

		static inline Mat4 mat4RotateX(Mat4& m, float rad)
		{
			return m * makeXRotation(rad);
		}

		static inline Mat4 mat4RotateY(Mat4& m, float rad)
		{
			return m * makeYRotation(rad);
		}

		static inline Mat4 mat4RotateZ(Mat4& m, float rad)
		{
			return m * makeZRotation(rad);
		}

		static inline Vec3f mat4MulVec3WithTranslation(Mat4& m, Vec3f& v)
		{
			return Vec3f(
				m[0][0] * v[0] + m[0][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
				m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
				m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3]);
		}

		static inline Vec3f mat4MulAndProjVec3(Mat4& m, Vec3f& v3)
		{
			Vec4f v4 = m * Vec4f(v3[0], v3[1], v3[2], 1.0f);
			return Vec3f(v4[0], v4[1], v4[2]) / v4[3];
		}

		static inline bool projectM4fPerspective(const Vec3f& obj, const Mat4& modelView, const Mat4& proj, const Vec4f& viewPort, Vec3f& winCoord)
		{
			//Transformation vectors
			float fTempo[8];
			//modelView transform
			fTempo[0]=modelView[0][0]*obj[0]+modelView[1][0]*obj[1]+modelView[2][0]*obj[2]+modelView[3][0];  //w is always 1
			fTempo[1]=modelView[0][1]*obj[0]+modelView[1][1]*obj[1]+modelView[2][1]*obj[2]+modelView[3][1];
			fTempo[2]=modelView[0][2]*obj[0]+modelView[1][2]*obj[1]+modelView[2][2]*obj[2]+modelView[3][2];
			fTempo[3]=modelView[0][3]*obj[0]+modelView[1][3]*obj[1]+modelView[2][3]*obj[2]+modelView[3][3];
			//proj transform, the final row of proj matrix is always [0.0, -1.0]
			//so we optimize for that.
			fTempo[4]=proj[0][0]*fTempo[0]+proj[1][0]*fTempo[1]+proj[2][0]*fTempo[2]+proj[3][0]*fTempo[3];
			fTempo[5]=proj[0][1]*fTempo[0]+proj[1][1]*fTempo[1]+proj[2][1]*fTempo[2]+proj[3][1]*fTempo[3];
			fTempo[6]=proj[0][2]*fTempo[0]+proj[1][2]*fTempo[1]+proj[2][2]*fTempo[2]+proj[3][2]*fTempo[3];
			fTempo[7]=-fTempo[2];
			//The result normalizes between -1 and 1
			if(fTempo[7]==0.0f)	//The w value
				return false;
			fTempo[7]=1.0f/fTempo[7];
			//Perspective division
			fTempo[4]*=fTempo[7];
			fTempo[5]*=fTempo[7];
			fTempo[6]*=fTempo[7];
			//Window coordinates
			//Map x, y to range 0-1
			winCoord[0]=(fTempo[4]*0.5f+0.5f)*viewPort[2]+viewPort[0];
			winCoord[1]=(fTempo[5]*0.5f+0.5f)*viewPort[3]+viewPort[1];
			//This is only correct when glDepthRange(0.0, 1.0)
			winCoord[2]=(1.0f+fTempo[6])*0.5f;	//Between 0 and 1
			return true;
		}

		static inline bool projectM4fPerspective(const Vec3f& obj, const Mat4& modelView, const Mat4& proj, const Vec4f& viewPort, Vec2f& winCoord)
		{
			//Transformation vectors
			float fTempo[8];
			//modelView transform
			fTempo[0]=modelView[0][0]*obj[0]+modelView[1][0]*obj[1]+modelView[2][0]*obj[2]+modelView[3][0];  //w is always 1
			fTempo[1]=modelView[0][1]*obj[0]+modelView[1][1]*obj[1]+modelView[2][1]*obj[2]+modelView[3][1];
			fTempo[2]=modelView[0][2]*obj[0]+modelView[1][2]*obj[1]+modelView[2][2]*obj[2]+modelView[3][2];
			fTempo[3]=modelView[0][3]*obj[0]+modelView[1][3]*obj[1]+modelView[2][3]*obj[2]+modelView[3][3];
			//proj transform, the final row of proj matrix is always [0.0, -1.0]
			//so we optimize for that.
			fTempo[4]=proj[0][0]*fTempo[0]+proj[1][0]*fTempo[1]+proj[2][0]*fTempo[2]+proj[3][0]*fTempo[3];
			fTempo[5]=proj[0][1]*fTempo[0]+proj[1][1]*fTempo[1]+proj[2][1]*fTempo[2]+proj[3][1]*fTempo[3];
			fTempo[6]=proj[0][2]*fTempo[0]+proj[1][2]*fTempo[1]+proj[2][2]*fTempo[2]+proj[3][2]*fTempo[3];
			fTempo[7]=-fTempo[2];
			//The result normalizes between -1 and 1
			if(fTempo[7]==0.0f)	//The w value
				return false;
			fTempo[7]=1.0f/fTempo[7];
			//Perspective division
			fTempo[4]*=fTempo[7];
			fTempo[5]*=fTempo[7];
			fTempo[6]*=fTempo[7];
			//Window coordinates
			//Map x, y to range 0-1
			winCoord[0]=(fTempo[4]*0.5f+0.5f)*viewPort[2]+viewPort[0];
			winCoord[1]=(fTempo[5]*0.5f+0.5f)*viewPort[3]+viewPort[1];
			return true;
		}

		static inline bool projectM4f(const Vec3f& obj, const Mat4& modelView, const Mat4& proj, const Vec4f& viewport, Vec3f& winCoord)
		{
			Vec4f result = (modelView * Vec4f(obj[0], obj[1], obj[2], 1.0f)) * proj;

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];
			result[2] /= result[3];

			winCoord[0] = viewport[0] + (1.0f + result[0]) * viewport[2] / 2.0f;
			winCoord[1] = viewport[1] + (1.0f + result[1]) * viewport[3] / 2.0f;

			winCoord[2] = (1.0f + result[2]) / 2.0f;
			return true;
		}

		static inline bool projectM4f(const Vec3f& obj, const Mat4& modelView, const Mat4& proj, const Vec4f& viewport, Vec2f& winCoord)
		{
			Vec4f result = (modelView * Vec4f(obj[0], obj[1], obj[2], 1.0f)) * proj;

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];
			result[2] /= result[3];

			winCoord[0] = viewport[0] + (1.0f + result[0]) * viewport[2] / 2.0f;
			winCoord[1] = viewport[1] + (1.0f + result[1]) * viewport[3] / 2.0f;
			return true;
		}

		static inline bool projectM4f(const Vec2f& obj, const Mat4& modelView, const Mat4& proj, const Vec4f& viewport, Vec2f& winCoord)
		{
			Vec4f result = (modelView * Vec4f(obj[0], obj[1], 0.0f, 1.0f)) * proj;

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];
			result[2] /= result[3];

			winCoord[0] = viewport[0] + (1.0f + result[0]) * viewport[2] / 2.0f;
			winCoord[1] = viewport[1] + (1.0f + result[1]) * viewport[3] / 2.0f;
			return true;
		}

		static inline bool projectM4f(const Vec3f& obj, const Mat4& modelViewProjection, const Vec4f& viewport, Vec3f& winCoord)
		{
			Vec4f result = modelViewProjection * Vec4f(obj[0], obj[1], obj[2], 1.0f);

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];
			result[2] /= result[3];

			winCoord[0] = viewport[0] + (1.0f + result[0]) * viewport[2] / 2.0f;
			winCoord[1] = viewport[1] + (1.0f + result[1]) * viewport[3] / 2.0f;

			winCoord[2] = (1.0f + result[2]) / 2.0f;
			return true;
		}

		static inline bool projectM4f(const Vec3f& obj, const Mat4& modelViewProjection, const Vec4f& viewport, Vec2f& winCoord)
		{
			Vec4f result = modelViewProjection * Vec4f(obj[0], obj[1], obj[2], 1.0f);

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];
			result[2] /= result[3];

			winCoord[0] = viewport[0] + (1.0f + result[0]) * viewport[2] / 2.0f;
			winCoord[1] = viewport[1] + (1.0f + result[1]) * viewport[3] / 2.0f;
			return true;
		}

		static inline bool projectM4f(const Vec3f& obj, const Mat4& modelView, const Mat4& proj, Vec2f& winCoord, float width = 1.0f, float height = 1.0f)
		{
			Vec4f result = (modelView * Vec4f(obj[0], obj[1], obj[2], 1.0f)) * proj;

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];

			winCoord[0] = (1.0f + result[0]) * width / 2.0f;
			winCoord[1] = (1.0f + result[1]) * height / 2.0f;
			return true;
		}

		//mvp = projectionMatrix * modelviewMatrix
		static inline bool projectM4f(const Vec3f& obj, const Mat4& mvp, Vec2f& winCoord, float width = 1.0f, float height = 1.0f)
		{
			Vec4f result = mvp * Vec4f(obj[0], obj[1], obj[2], 1.0f);

			if (result[3] == 0.0f)
				return false;

			result[0] /= result[3];
			result[1] /= result[3];

			winCoord[0] = (1.0f + result[0]) * width / 2.0f;
			winCoord[1] = (1.0f + result[1]) * height / 2.0f;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////

		inline float* operator[](int index)
		{
			return data[index];
		}

		inline const float* operator[](int index) const
		{
			return data[index];
		}

		Mat4() {}
		Mat4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			makeMatrix(*this,  m00,  m01,  m02,  m03,
				m10,  m11,  m12,  m13,
				m20,  m21,  m22,  m23,
				m30,  m31,  m32,  m33);
		}

		Mat4(float values[16])
		{
			makeMatrix(*this, values[0], values[1], values[2], values[3], 
				values[4], values[5], values[6], values[7],
				values[8], values[9], values[10], values[11],
				values[12], values[13], values[14], values[15]);
		}

		Mat4(Vec4f row0, Vec4f row1, Vec4f row2, Vec4f row3)
		{
			makeMatrix(*this, row0[0], row0[1], row0[2], row0[3],
				row1[0], row1[1], row1[2], row1[3],
				row2[0], row2[1], row2[2], row2[3],
				row3[0], row3[1], row3[2], row3[3]);
		}

		Mat4(Mat3& m, float v)
		{
			makeMatrix(*this, m[0][0], m[0][1], m[0][2], v,
				m[1][0], m[1][1], m[1][2], v,
				m[2][0], m[2][1], m[2][2], v,
				v, v, v, v);
		}

		inline const Mat4& transPose()
		{
			return makeMatrix(*this, data[0][0], data[1][0], data[2][0], data[3][0],
				data[0][1], data[1][1], data[2][1], data[3][1],
				data[0][2], data[1][2], data[2][2], data[3][2],
				data[0][3], data[1][3], data[2][3], data[3][3]);
		}

		inline void loadIdentity()
		{
			*this = makeIdentity();
		}

		inline const Mat4& initWithQuaternion(float x, float y, float z, float w)
		{
			normalize(x, y, z, w);
			const float _2x = x + x;
			const float _2y = y + y;
			const float _2z = z + z;
			const float _2w = w + w;
			makeMatrix(*this, 1.0f - _2y * y - _2z * z,
				_2x * y + _2w * z,
				_2x * z - _2w * y,
				0.0f,
				_2x * y - _2w * z,
				1.0f - _2x * x - _2z * z,
				_2y * z + _2w * x,
				0.0f,
				_2x * z + _2w * y,
				_2y * z - _2w * x,
				1.0f - _2x * x - _2y * y,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f);
			return *this;
		}

		inline Mat3 getMat3()
		{
			return Mat3(data[0][0], data[0][1], data[0][2],
				data[1][0], data[1][1], data[1][2],
				data[2][0], data[2][1], data[2][2]);
		}

		inline Mat2 getMat2()
		{
			return Mat2(data[0][0], data[0][1],
				data[1][0], data[1][1]);
		}

		inline Mat4 operator*(const Mat4& m) const
		{

			return Mat4(data[0][0] * m[0][0] + data[1][0] * m[0][1] + data[2][0] * m[0][2] + data[3][0] * m[0][3],
				data[0][1] * m[0][0] + data[1][1] * m[0][1] + data[2][1] * m[0][2] + data[3][1] * m[0][3],
				data[0][2] * m[0][0] + data[1][2] * m[0][1] + data[2][2] * m[0][2] + data[3][2] * m[0][3],
				data[0][3] * m[0][0] + data[1][3] * m[0][1] + data[2][3] * m[0][2] + data[3][3] * m[0][3],
				data[0][0] * m[1][0] + data[1][0] * m[1][1] + data[2][0] * m[1][2] + data[3][0] * m[1][3],
				data[0][1] * m[1][0] + data[1][1] * m[1][1] + data[2][1] * m[1][2] + data[3][1] * m[1][3],
				data[0][2] * m[1][0] + data[1][2] * m[1][1] + data[2][2] * m[1][2] + data[3][2] * m[1][3],
				data[0][3] * m[1][0] + data[1][3] * m[1][1] + data[2][3] * m[1][2] + data[3][3] * m[1][3],
				data[0][0] * m[2][0] + data[1][0] * m[2][1] + data[2][0] * m[2][2] + data[3][0] * m[2][3],
				data[0][1] * m[2][0] + data[1][1] * m[2][1] + data[2][1] * m[2][2] + data[3][1] * m[2][3],
				data[0][2] * m[2][0] + data[1][2] * m[2][1] + data[2][2] * m[2][2] + data[3][2] * m[2][3],
				data[0][3] * m[2][0] + data[1][3] * m[2][1] + data[2][3] * m[2][2] + data[3][3] * m[2][3],
				data[0][0] * m[3][0] + data[1][0] * m[3][1] + data[2][0] * m[3][2] + data[3][0] * m[3][3],			
				data[0][1] * m[3][0] + data[1][1] * m[3][1] + data[2][1] * m[3][2] + data[3][1] * m[3][3],			
				data[0][2] * m[3][0] + data[1][2] * m[3][1] + data[2][2] * m[3][2] + data[3][2] * m[3][3],			
				data[0][3] * m[3][0] + data[1][3] * m[3][1] + data[2][3] * m[3][2] + data[3][3] * m[3][3]);
		}

		//特殊用法， 将mat3 直接转换为mat4 并与之相乘。
		inline Mat4 operator*(const Mat3& m) const
		{
			return Mat4(data[0][0] * m[0][0] + data[1][0] * m[0][1] + data[2][0] * m[0][2],
				data[0][1] * m[0][0] + data[1][1] * m[0][1] + data[2][1] * m[0][2],
				data[0][2] * m[0][0] + data[1][2] * m[0][1] + data[2][2] * m[0][2],
				data[0][3] * m[0][0] + data[1][3] * m[0][1] + data[2][3] * m[0][2],
				data[0][0] * m[1][0] + data[1][0] * m[1][1] + data[2][0] * m[1][2],
				data[0][1] * m[1][0] + data[1][1] * m[1][1] + data[2][1] * m[1][2],
				data[0][2] * m[1][0] + data[1][2] * m[1][1] + data[2][2] * m[1][2],
				data[0][3] * m[1][0] + data[1][3] * m[1][1] + data[2][3] * m[1][2],
				data[0][0] * m[2][0] + data[1][0] * m[2][1] + data[2][0] * m[2][2],
				data[0][1] * m[2][0] + data[1][1] * m[2][1] + data[2][1] * m[2][2],
				data[0][2] * m[2][0] + data[1][2] * m[2][1] + data[2][2] * m[2][2],
				data[0][3] * m[2][0] + data[1][3] * m[2][1] + data[2][3] * m[2][2],
				data[3][0],
				data[3][1],
				data[3][2],
				data[3][3]);
		}

		inline Mat4& operator*=(const Mat4& m)
		{
			*this = *this * m;
			return *this;
		}

		inline Vec4f operator*(const Vec4f& v) const
		{
			return Vec4f(
				data[0][0] * v[0] + data[1][0] * v[1] + data[2][0] * v[2] + data[3][0] * v[3],
				data[0][1] * v[0] + data[1][1] * v[1] + data[2][1] * v[2] + data[3][1] * v[3],
				data[0][2] * v[0] + data[1][2] * v[1] + data[2][2] * v[2] + data[3][2] * v[3],
				data[0][3] * v[0] + data[1][3] * v[1] + data[2][3] * v[2] + data[3][3] * v[3]);
		}

		inline Vec3f operator*(const Vec3f& v) const
		{
			return Vec3f(data[0][0] * v[0] + data[1][0] * v[1] + data[2][0] * v[2],
				data[0][1] * v[0] + data[1][1] * v[1] + data[2][1] * v[2],
				data[0][2] * v[0] + data[1][2] * v[1] + data[2][2] * v[2]);
		}

		friend inline Vec4f operator*(const Vec4f& v, const Mat4& m)
		{
			return m * v;
		}

		friend inline Vec3f operator*(const Vec3f& v, const Mat4& m)
		{
			return m * v;
		}

		inline Mat4 operator+(const Mat4& m) const
		{
			return Mat4( data[0][0] + m[0][0],
				data[0][1] + m[0][1],
				data[0][2] + m[0][2],
				data[0][3] + m[0][3],

				data[1][0] + m[1][0],
				data[1][1] + m[1][1],
				data[1][2] + m[1][2],
				data[1][3] + m[1][3],

				data[2][0] + m[2][0],
				data[2][1] + m[2][1],
				data[2][2] + m[2][2],
				data[2][3] + m[2][3],

				data[3][0] + m[3][0],
				data[3][1] + m[3][1],
				data[3][2] + m[3][2],
				data[3][3] + m[3][3]);
		}

		inline Mat4& operator+=(const Mat4& m)
		{
			*this = *this + m;
			return *this;
		}

		inline Mat4 operator-(const Mat4& m) const
		{
			return Mat4(data[0][0] - m[0][0],
				data[0][1] - m[0][1],
				data[0][2] - m[0][2],
				data[0][3] - m[0][3],

				data[1][0] - m[1][0],
				data[1][1] - m[1][1],
				data[1][2] - m[1][2],
				data[1][3] - m[1][3],

				data[2][0] - m[2][0],
				data[2][1] - m[2][1],
				data[2][2] - m[2][2],
				data[2][3] - m[2][3],

				data[3][0] - m[3][0],
				data[3][1] - m[3][1],
				data[3][2] - m[3][2],
				data[3][3] - m[3][3]);
		}

		inline Mat4& operator-=(const Mat4& m)
		{
			*this = *this - m;
			return *this;
		}

		inline void translateX(float x)
		{
			data[3][0] += data[0][0] * x;
			data[3][1] += data[0][1] * x;
			data[3][2] += data[0][2] * x;
		}

		inline void translateY(float y)
		{
			data[3][0] += data[1][0] * y;
			data[3][1] += data[1][1] * y;
			data[3][2] += data[1][2] * y;
		}

		inline void translateZ(float z)
		{
			data[3][0] += data[2][0] * z;
			data[3][1] += data[2][1] * z;
			data[3][2] += data[2][2] * z;
		}

		inline void translate(float x, float y, float z)
		{
			data[3][0] += data[0][0] * x + data[1][0] * y + data[2][0] * z;
			data[3][1] += data[0][1] * x + data[1][1] * y + data[2][1] * z;
			data[3][2] += data[0][2] * x + data[1][2] * y + data[2][2] * z;
		}

		inline void scaleX(float x)
		{
			data[0][0] *= x;
			data[0][1] *= x;
			data[0][2] *= x;
			data[0][3] *= x;
		}

		inline void scaleY(float y)
		{
			data[1][0] *= y;
			data[1][1] *= y;
			data[1][2] *= y;
			data[1][3] *= y;
		}

		inline void scaleZ(float z)
		{
			data[2][0] *= z;
			data[2][1] *= z;
			data[2][2] *= z;
			data[2][3] *= z;
		}

		inline void scale(float x, float y, float z)
		{
			scaleX(x);
			scaleY(y);
			scaleZ(z);
		}

		inline void rotate(float rad, float x, float y, float z)
		{
			*this *= makeRotation(rad, x, y, z);
		}

		inline void rotateX(float rad)
		{
			*this *= makeXRotation(rad);
		}

		inline void rotateY(float rad)
		{
			*this *= makeYRotation(rad);
		}

		inline void rotateZ(float rad)
		{
			*this *= makeZRotation(rad);
		}

		inline Mat3 toMat3()
		{
			return Mat3(data[0][0], data[0][1], data[0][2],
				data[1][0], data[1][1], data[1][2],
				data[2][0], data[2][1], data[2][2]);
		}

		float data[4][4];
	};

}

#endif