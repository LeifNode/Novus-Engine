#include "Matrix_t.h"
#include "Vector_t.h"
#include <cassert>

namespace novus
{
namespace math
{
	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t()
	{
		this->value[0] = col_type(1, 0, 0);
		this->value[1] = col_type(0, 1, 0);
		this->value[2] = col_type(0, 0, 1);
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix3x3_t<T> m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const col_type& v1, const col_type& v2, const col_type& v3)
	{
		this->value[0] = v1;
		this->value[1] = v2;
		this->value[2] = v3;
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2,
		const T& x3, const T& y3, const T& z3)
	{
		this->value = col_type(x1, y1, z1);
		this->value = col_type(x2, y2, z2);
		this->value = col_type(x3, y3, z3);
	}

	//Conversion constructors
	template <typename T>
	template <typename B>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix3x3_t<B>& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
	}

	template <typename T>
	template <typename V1, typename V2, typename V3>
	Matrix3x3_t<T>::Matrix3x3_t(const Vector3_t<V1>& v1, const Vector3_t<V2>& v2, const Vector3_t<V3>& v3)
	{
		this->value[0] = col_type(v1);
		this->value[1] = col_type(v2);
		this->value[2] = col_type(v3);
	}

	template <typename T>
	template
		<typename X1, typename Y1, typename Z1,
		 typename X2, typename Y2, typename Z2,
		 typename X3, typename Y3, typename Z3>
	Matrix3x3_t<T>::Matrix3x3_t(
		 const X1& x1, const Y1& y1, const Z1& z1,
		 const X2& x2, const Y2& y2, const Z2& z2,
		 const X3& x3, const Y3& y3, const Z3& z3)
	{
		this->value[0] = col_type(static_cast<T>(x1), value_type(y1), value_type(z1));
		this->value[1] = col_type(static_cast<T>(x2), value_type(y2), value_type(z2));
		this->value[2] = col_type(static_cast<T>(x3), value_type(y3), value_type(z3));
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix4x4_t<T>& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
	}

	template <typename T>
	size_t Matrix3x3_t<T>::size() const
	{
		return 3;
	}

	template <typename T>
	Matrix3x3_t<T>::col_type& operator[] (size_t i)
	{
		assert(i < this->size());

		return this->value[i];
	}

	template <typename T>
	const Matrix3x3_t<T>::col_type& operator[] (size_t i) const
	{
		assert(i < this->size());

		return this->value[i];
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator= (const Matrix3x3_t<U>& m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator+= (const Matrix3x3_t<U>& m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		this->value[2] += m[2];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator-= (const Matrix3x3_t<U>& m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		this->value[2] -= m[2];
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator*= (const Matrix3x3_t<U>& m)
	{
		return (*this = *this * m);
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator/= (const Matrix3x3_t<U>& m)
	{

	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator+= (const U& s)
	{
		this->value[0] += s;
		this->value[1] += s;
		this->value[2] += s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator-= (const U& s)
	{
		this->value[0] -= s;
		this->value[1] -= s;
		this->value[2] -= s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator*= (const U& s)
	{
		this->value[0] *= s;
		this->value[1] *= s;
		this->value[2] *= s;
		return *this;
	}

	template <typename T>
	template <typename U>
	Matrix3x3_t<T>& Matrix3x3_t<T>::operator/= (const U& s)
	{
		this->value[0] /= s;
		this->value[1] /= s;
		this->value[2] /= s;
		return *this;
	}

	template <typename T>
	Matrix3x3_t<T> operator+ (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		return Matrix3x3_t<T>(
			m1[0] + m2[0],
			m1[1] + m2[1],
			m1[2] + m2[2]);
	}

	template <typename T>
	Matrix3x3_t<T> operator+ (const Matrix3x3_t<T>& m, const T& s)
	{
		return Matrix3x3_t<T>(
			m1[0] + s,
			m1[1] + s,
			m1[2] + s);
	}

	template<typename T>
	Matrix3x3_t<T> operator+ (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m1[0] + s,
			m1[1] + s,
			m1[2] + s);
	}

	template <typename T>
	Matrix3x3_t<T> operator- (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		return Matrix3x3_t<T>(
			m1[0] - m2[0],
			m1[1] - m2[1],
			m1[2] - m2[2]);
	}

	template <typename T>
	Matrix3x3_t<T> operator- (const Matrix3x3_t<T>& m, const T& s)
	{
		return Matrix3x3_t<T>(
			m1[0] - s,
			m1[1] - s,
			m1[2] - s);
	}

	template<typename T>
	Matrix3x3_t<T> operator- (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			s - m1[0],
			s - m1[1],
			s - m1[2]);
	}

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		T const SrcA00 = m1[0][0];
		T const SrcA01 = m1[0][1];
		T const SrcA02 = m1[0][2];
		T const SrcA10 = m1[1][0];
		T const SrcA11 = m1[1][1];
		T const SrcA12 = m1[1][2];
		T const SrcA20 = m1[2][0];
		T const SrcA21 = m1[2][1];
		T const SrcA22 = m1[2][2];

		T const SrcB00 = m2[0][0];
		T const SrcB01 = m2[0][1];
		T const SrcB02 = m2[0][2];
		T const SrcB10 = m2[1][0];
		T const SrcB11 = m2[1][1];
		T const SrcB12 = m2[1][2];
		T const SrcB20 = m2[2][0];
		T const SrcB21 = m2[2][1];
		T const SrcB22 = m2[2][2];

		//TODO: convert to row-major
		Matrix3x3_t<T> Result();
		Result[0][0] = SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02;
		Result[0][1] = SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02;
		Result[0][2] = SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02;
		Result[1][0] = SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12;
		Result[1][1] = SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12;
		Result[1][2] = SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12;
		Result[2][0] = SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22;
		Result[2][1] = SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22;
		Result[2][2] = SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22;

		return Result;
	}

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m, const T& s)
	{
		return Matrix3x3_t<T>(
			m1[0] * s,
			m1[1] * s,
			m1[2] * s);
	}

	template<typename T>
	Matrix3x3_t<T> operator* (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m1[0] * s,
			m1[1] * s,
			m1[2] * s);
	}

	template <typename T>
	Matrix3x3_t<T>::col_type operator* (const Matrix3x3_t<T>& m, const Matrix3x3_t<T>::row_type& v)
	{
		return Matrix3x3_t<T>::col_type(
			m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
			m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
			m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
	}
}
}