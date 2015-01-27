#include "Matrix_t.h"
#include "Vector_t.h"
#include <cassert>

namespace novus
{
	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t()
	{
		this->value[0] = row_type(1, 0, 0, 0);
		this->value[1] = row_type(0, 1, 0, 0);
		this->value[2] = row_type(0, 0, 1, 0);
		this->value[3] = row_type(0, 0, 0, 1);
	}

	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t(const Matrix4x4_t<T>& m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
		this->value[3] = m.value[3];
	}

	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t(const T& s)
	{
		this->value[0] = row_type(s, 0, 0, 0);
		this->value[1] = row_type(0, s, 0, 0);
		this->value[2] = row_type(0, 0, s, 0);
		this->value[3] = row_type(0, 0, 0, s);
	}

	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t(const row_type& v1, const row_type& v2, const row_type& v3, const row_type& v4)
	{
		this->value[0] = v1;
		this->value[1] = v2;
		this->value[2] = v3;
		this->value[3] = v4;
	}

	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t(
		const T& x1, const T& y1, const T& z1, const T& w1,
		const T& x2, const T& y2, const T& z2, const T& w2,
		const T& x3, const T& y3, const T& z3, const T& w3,
		const T& x4, const T& y4, const T& z4, const T& w4)
	{
		this->value[0] = row_type(x1, y1, z1, w1);
		this->value[1] = row_type(x2, y2, z2, w2);
		this->value[2] = row_type(x3, y3, z3, w3);
		this->value[3] = row_type(x4, y4, z4, w4);
	}

	//Conversion constructors
	template <typename T>
	template <typename B>
	Matrix4x4_t<T>::Matrix4x4_t(const Matrix4x4_t<B>& m)
	{
		this->value[0] = row_type(m[0]);
		this->value[1] = row_type(m[1]);
		this->value[2] = row_type(m[2]);
		this->value[3] = row_type(m[3]);
	}

	template <typename T>
	template <typename V1, typename V2, typename V3, typename V4>
	Matrix4x4_t<T>::Matrix4x4_t(const Vector4_t<V1>& v1, const Vector4_t<V2>& v2, const Vector4_t<V3>& v3, const Vector4_t<V4>& v4)
	{
		this->value[0] = row_type(v1);
		this->value[1] = row_type(v2);
		this->value[2] = row_type(v3);
		this->value[2] = row_type(v4);
	}

	template <typename T>
	template
		<typename X1, typename Y1, typename Z1, typename W1,
		typename X2, typename Y2, typename Z2, typename W2,
		typename X3, typename Y3, typename Z3, typename W3,
		typename X4, typename Y4, typename Z4, typename W4>
	Matrix4x4_t<T>::Matrix4x4_t(
		const X1& x1, const Y1& y1, const Z1& z1, const W1& w1,
		const X2& x2, const Y2& y2, const Z2& z2, const W2& w2,
		const X3& x3, const Y3& y3, const Z3& z3, const W3& w3,
		const X4& x4, const Y4& y4, const Z4& z4, const W4& w4)
	{
		this->value[0] = row_type(static_cast<T>(x1), value_type(y1), value_type(z1), value_type(w1));
		this->value[1] = row_type(static_cast<T>(x2), value_type(y2), value_type(z2), value_type(w2));
		this->value[2] = row_type(static_cast<T>(x3), value_type(y3), value_type(z3), value_type(w3));
		this->value[3] = row_type(static_cast<T>(x4), value_type(y4), value_type(z4), value_type(w4));
	}

	template <typename T>
	Matrix4x4_t<T>::Matrix4x4_t(const Matrix3x3_t<T>& m)
	{
		this->value[0] = row_type(m[0], 0);
		this->value[1] = row_type(m[1], 0);
		this->value[2] = row_type(m[2], 0);
		this->value[3] = row_type(0, 0, 0, 1);
	}

	template <typename T>
	size_t Matrix4x4_t<T>::size() const
	{
		return 4;
	}

	template <typename T>
	typename Matrix4x4_t<T>::row_type& Matrix4x4_t<T>::operator[] (size_t i)
	{
		assert(i < this->size());

		return this->value[i];
	}

	template <typename T>
	const typename Matrix4x4_t<T>::row_type& Matrix4x4_t<T>::operator[] (size_t i) const
	{
		assert(i < this->size());

		return this->value[i];
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator= (const Matrix4x4_t<U>& m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
		this->value[3] = m.value[3];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator+= (const Matrix4x4_t<U>& m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		this->value[2] += m[2];
		this->value[3] += m[3];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator-= (const Matrix4x4_t<U>& m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		this->value[2] -= m[2];
		this->value[3] -= m[3];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator*= (const Matrix4x4_t<U>& m)
	{
		return (*this = *this * m);
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator/= (const Matrix4x4_t<U>& m)
	{

	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator+= (const U& s)
	{
		this->value[0] += s;
		this->value[1] += s;
		this->value[2] += s;
		this->value[3] += s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator-= (const U& s)
	{
		this->value[0] -= s;
		this->value[1] -= s;
		this->value[2] -= s;
		this->value[3] -= s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator*= (const U& s)
	{
		this->value[0] *= s;
		this->value[1] *= s;
		this->value[2] *= s;
		this->value[3] *= s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix4x4_t<T>& Matrix4x4_t<T>::operator/= (const U& s)
	{
		this->value[0] /= s;
		this->value[1] /= s;
		this->value[2] /= s;
		this->value[3] /= s;
		return *this;
	}

	template <typename T>
	Matrix4x4_t<T> operator+ (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2)
	{
		return Matrix4x4_t<T>(
			m1[0] + m2[0],
			m1[1] + m2[1],
			m1[2] + m2[2],
			m1[3] + m2[3]);
	}

	template <typename T>
	Matrix4x4_t<T> operator+ (const Matrix4x4_t<T>& m, const T& s)
	{
		return Matrix4x4_t<T>(
			m[0] + s,
			m[1] + s,
			m[2] + s,
			m[3] + s);
	}

	template<typename T>
	Matrix4x4_t<T> operator+ (const T& s, const Matrix4x4_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m[0] + s,
			m[1] + s,
			m[2] + s,
			m[3] + s);
	}

	template <typename T>
	Matrix4x4_t<T> operator- (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2)
	{
		return Matrix4x4_t<T>(
			m1[0] - m2[0],
			m1[1] - m2[1],
			m1[2] - m2[2],
			m1[3] - m2[3]);
	}

	template <typename T>
	Matrix4x4_t<T> operator- (const Matrix4x4_t<T>& m, const T& s)
	{
		return Matrix4x4_t<T>(
			m[0] - s,
			m[1] - s,
			m[2] - s,
			m[3] - s);
	}

	template<typename T>
	Matrix4x4_t<T> operator- (const T& s, const Matrix4x4_t<T>& m)
	{
		return Matrix4x4_t<T>(
			s - m[0],
			s - m[1],
			s - m[2],
			s - m[3]);
	}

	template <typename T>
	Matrix4x4_t<T> operator* (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2)
	{
		T const srcA00 = m1[0][0];
		T const srcA01 = m1[0][1];
		T const srcA02 = m1[0][2];
		T const srcA03 = m1[0][3];
		T const srcA10 = m1[1][0];
		T const srcA11 = m1[1][1];
		T const srcA12 = m1[1][2];
		T const srcA13 = m1[1][3];
		T const srcA20 = m1[2][0];
		T const srcA21 = m1[2][1];
		T const srcA22 = m1[2][2];
		T const srcA23 = m1[2][3];
		T const srcA30 = m1[3][0];
		T const srcA31 = m1[3][1];
		T const srcA32 = m1[3][2];
		T const srcA33 = m1[3][3];

		T const srcB00 = m2[0][0];
		T const srcB01 = m2[0][1];
		T const srcB02 = m2[0][2];
		T const srcB03 = m2[0][3];
		T const srcB10 = m2[1][0];
		T const srcB11 = m2[1][1];
		T const srcB12 = m2[1][2];
		T const srcB13 = m2[1][3];
		T const srcB20 = m2[2][0];
		T const srcB21 = m2[2][1];
		T const srcB22 = m2[2][2];
		T const srcB23 = m2[2][3];
		T const srcB30 = m2[3][0];
		T const srcB31 = m2[3][1];
		T const srcB32 = m2[3][2];
		T const srcB33 = m2[3][3];

		Matrix4x4_t<T> result;

		result[0][0] = srcA00 * srcB00 + srcA01 * srcB10 + srcA02 * srcB20 + srcA03 * srcB30;
		result[0][1] = srcA00 * srcB01 + srcA01 * srcB11 + srcA02 * srcB21 + srcA03 * srcB31;
		result[0][2] = srcA00 * srcB02 + srcA01 * srcB12 + srcA02 * srcB22 + srcA03 * srcB32;
		result[0][3] = srcA00 * srcB03 + srcA01 * srcB13 + srcA02 * srcB23 + srcA03 * srcB33;

		result[1][0] = srcA10 * srcB00 + srcA11 * srcB10 + srcA12 * srcB20 + srcA13 * srcB30;
		result[1][1] = srcA10 * srcB01 + srcA11 * srcB11 + srcA12 * srcB21 + srcA13 * srcB31;
		result[1][2] = srcA10 * srcB02 + srcA11 * srcB12 + srcA12 * srcB22 + srcA13 * srcB32;
		result[1][3] = srcA10 * srcB03 + srcA11 * srcB13 + srcA12 * srcB23 + srcA13 * srcB33;

		result[2][0] = srcA20 * srcB00 + srcA21 * srcB10 + srcA22 * srcB20 + srcA23 * srcB30;
		result[2][1] = srcA20 * srcB01 + srcA21 * srcB11 + srcA22 * srcB21 + srcA23 * srcB31;
		result[2][2] = srcA20 * srcB02 + srcA21 * srcB12 + srcA22 * srcB22 + srcA23 * srcB32;
		result[2][3] = srcA20 * srcB03 + srcA21 * srcB13 + srcA22 * srcB23 + srcA23 * srcB33;

		result[3][0] = srcA30 * srcB00 + srcA31 * srcB10 + srcA32 * srcB20 + srcA33 * srcB30;
		result[3][1] = srcA30 * srcB01 + srcA31 * srcB11 + srcA32 * srcB21 + srcA33 * srcB31;
		result[3][2] = srcA30 * srcB02 + srcA31 * srcB12 + srcA32 * srcB22 + srcA33 * srcB32;
		result[3][3] = srcA30 * srcB03 + srcA31 * srcB13 + srcA32 * srcB23 + srcA33 * srcB33;

		return result;
	}

	template <typename T>
	Matrix4x4_t<T> operator* (const Matrix4x4_t<T>& m, const T& s)
	{
		return Matrix4x4_t<T>(
			m[0] * s,
			m[1] * s,
			m[2] * s,
			m[3] * s);
	}

	template<typename T>
	Matrix4x4_t<T> operator* (const T& s, const Matrix4x4_t<T>& m)
	{
		return Matrix4x4_t<T>(
			m[0] * s,
			m[1] * s,
			m[2] * s,
			m[3] * s);
	}

	template <typename T>
	typename Matrix4x4_t<T>::row_type operator* (const Matrix4x4_t<T>& m, const typename Matrix4x4_t<T>::col_type& v)
	{
		return Matrix4x4_t<T>::row_type(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
	}

	template <typename T>
	typename Matrix4x4_t<T>::col_type operator* (const typename Matrix4x4_t<T>::row_type& v, const Matrix4x4_t<T>& m)
	{
		return Matrix4x4_t<T>::col_type(
			v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0], v.w * m[3][0],
			v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1], v.w * m[3][1],
			v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2], v.w * m[3][2],
			v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3], v.w * m[3][3]);
	}

	template <typename T>
	bool operator== (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2)
	{
		return (m1[0] == m2[0] && m1[1] == m2[1] && m1[2] == m2[2] && m1[3] == m2[3]);
	}

	template <typename T>
	bool operator!= (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2)
	{
		return (m1[0] != m2[0] || m1[1] != m2[1] || m1[2] != m2[2] || m1[3] != m2[3]);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::Invert(const Matrix4x4_t<T>& m)
	{
		//Using GLM's implementation of Cramer's rule adapted for row-major matrices

		T Coef00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
		T Coef02 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
		T Coef03 = m[2][1] * m[3][2] - m[2][2] * m[3][1];

		T Coef04 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
		T Coef06 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
		T Coef07 = m[1][1] * m[3][2] - m[1][2] * m[3][1];

		T Coef08 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
		T Coef10 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
		T Coef11 = m[1][1] * m[2][2] - m[1][2] * m[2][1];

		T Coef12 = m[0][2] * m[3][3] - m[0][3] * m[3][2];
		T Coef14 = m[0][1] * m[3][3] - m[0][3] * m[3][1];
		T Coef15 = m[0][1] * m[3][2] - m[0][2] * m[3][1];

		T Coef16 = m[0][2] * m[2][3] - m[0][3] * m[2][2];
		T Coef18 = m[0][1] * m[2][3] - m[0][3] * m[2][1];
		T Coef19 = m[0][1] * m[2][2] - m[0][2] * m[2][1];

		T Coef20 = m[0][2] * m[1][3] - m[0][3] * m[1][2];
		T Coef22 = m[0][1] * m[1][3] - m[0][3] * m[1][1];
		T Coef23 = m[0][1] * m[1][2] - m[0][2] * m[1][1];

		Vector4_t<T> Fac0(Coef00, Coef00, Coef02, Coef03);
		Vector4_t<T> Fac1(Coef04, Coef04, Coef06, Coef07);
		Vector4_t<T> Fac2(Coef08, Coef08, Coef10, Coef11);
		Vector4_t<T> Fac3(Coef12, Coef12, Coef14, Coef15);
		Vector4_t<T> Fac4(Coef16, Coef16, Coef18, Coef19);
		Vector4_t<T> Fac5(Coef20, Coef20, Coef22, Coef23);

		Vector4_t<T> Vec0(m[0][1], m[0][0], m[0][0], m[0][0]);
		Vector4_t<T> Vec1(m[1][1], m[1][0], m[1][0], m[1][0]);
		Vector4_t<T> Vec2(m[2][1], m[2][0], m[2][0], m[2][0]);
		Vector4_t<T> Vec3(m[3][1], m[3][0], m[3][0], m[3][0]);

		Vector4_t<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		Vector4_t<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		Vector4_t<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		Vector4_t<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		Vector4_t<T> SignA(+1, -1, +1, -1);
		Vector4_t<T> SignB(-1, +1, -1, +1);
		Matrix4x4_t<T> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);
		Inverse = Transpose(Inverse); //Might be beneficial to remove this

		Vector4_t<T> Dot0(Vector4_t<T>(m[0][0], m[1][0], m[2][0], m[3][0]) * Inverse[0]);
		T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

		T OneOverDeterminant = static_cast<T>(1) / Dot1;

		return Inverse * OneOverDeterminant;
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::Transpose(const Matrix4x4_t<T>& m)
	{
		return Matrix4x4_t<T>(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::Scale(const T& scaleX, const T& scaleY, const T& scaleZ)
	{
		return Matrix4x4_t<T>(
			scaleX, 0, 0, 0,
			0, scaleY, 0, 0,
			0, 0, scaleZ, 0,
			0, 0, 0, 1);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::RotateX(const T& r)
	{
		return Matrix4x4_t<T>(
			1, 0, 0, 0,
			0, cos(r), -sin(r), 0,
			0, sin(r), cos(r), 0,
			0, 0, 0, 1);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::RotateY(const T& r)
	{
		return Matrix4x4_t<T>(
			cos(r), 0, sin(r), 0,
			0, 1, 0, 0,
			-sin(r), 0, cos(r), 0,
			0, 0, 0, 1);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::RotateZ(const T& r)
	{
		return Matrix4x4_t<T>(
			cos(r), -sin(r), 0, 0,
			sin(r), cos(r), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	template <typename T>
	Matrix4x4_t<T> Matrix4x4_t<T>::Translate(const T& x, const T& y, const T& z)
	{
		return Matrix4x4_t<T>(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1);
	}
}
