#pragma once

#ifndef NOVUS_MATRIX3X3_T_INL
#define NOVUS_MATRIX3X3_T_INL

#include "Matrix_t.h"
#include "Vector_t.h"
#include "Matrix3x3_t.h"

#include <cassert>

namespace novus
{
	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t()
	{
		this->value[0] = row_type(1, 0, 0);
		this->value[1] = row_type(0, 1, 0);
		this->value[2] = row_type(0, 0, 1);
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix3x3_t<T>& m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const T& s)
	{
		this->value[0] = row_type(s, 0, 0);
		this->value[1] = row_type(0, s, 0);
		this->value[2] = row_type(0, 0, s);
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const row_type& v1, const row_type& v2, const row_type& v3)
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
		this->value[0] = row_type(x1, y1, z1);
		this->value[1] = row_type(x2, y2, z2);
		this->value[2] = row_type(x3, y3, z3);
	}

	//Conversion constructors
	template <typename T>
	template <typename B>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix3x3_t<B>& m)
	{
		this->value[0] = row_type(m[0]);
		this->value[1] = row_type(m[1]);
		this->value[2] = row_type(m[2]);
	}

	template <typename T>
	template <typename V1, typename V2, typename V3>
	Matrix3x3_t<T>::Matrix3x3_t(const Vector3_t<V1>& v1, const Vector3_t<V2>& v2, const Vector3_t<V3>& v3)
	{
		this->value[0] = row_type(v1);
		this->value[1] = row_type(v2);
		this->value[2] = row_type(v3);
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
		this->value[0] = row_type(static_cast<T>(x1), value_type(y1), value_type(z1));
		this->value[1] = row_type(static_cast<T>(x2), value_type(y2), value_type(z2));
		this->value[2] = row_type(static_cast<T>(x3), value_type(y3), value_type(z3));
	}

	template <typename T>
	Matrix3x3_t<T>::Matrix3x3_t(const Matrix4x4_t<T>& m)
	{
		this->value[0] = row_type(m[0]);
		this->value[1] = row_type(m[1]);
		this->value[2] = row_type(m[2]);
	}

	template <typename T>
	size_t Matrix3x3_t<T>::size() const
	{
		return 3;
	}

	template <typename T>
	typename Matrix3x3_t<T>::row_type& Matrix3x3_t<T>::operator[] (size_t i)
	{
		assert(i < this->size());

		return this->value[i];
	}

	template <typename T>
	const typename Matrix3x3_t<T>::row_type& Matrix3x3_t<T>::operator[] (size_t i) const
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
	Matrix3x3_t<T> operator- (const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			-m1[0],
			-m1[1],
			-m1[2]);
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
			m[0] + s,
			m[1] + s,
			m[2] + s);
	}

	template<typename T>
	Matrix3x3_t<T> operator+ (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m[0] + s,
			m[1] + s,
			m[2] + s);
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
			m[0] - s,
			m[1] - s,
			m[2] - s);
	}

	template<typename T>
	Matrix3x3_t<T> operator- (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			s - m[0],
			s - m[1],
			s - m[2]);
	}

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		T const srcA00 = m1[0][0];
		T const srcA01 = m1[0][1];
		T const srcA02 = m1[0][2];
		T const srcA10 = m1[1][0];
		T const srcA11 = m1[1][1];
		T const srcA12 = m1[1][2];
		T const srcA20 = m1[2][0];
		T const srcA21 = m1[2][1];
		T const srcA22 = m1[2][2];

		T const srcB00 = m2[0][0];
		T const srcB01 = m2[0][1];
		T const srcB02 = m2[0][2];
		T const srcB10 = m2[1][0];
		T const srcB11 = m2[1][1];
		T const srcB12 = m2[1][2];
		T const srcB20 = m2[2][0];
		T const srcB21 = m2[2][1];
		T const srcB22 = m2[2][2];

		Matrix3x3_t<T> result;

		result[0][0] = srcA00 * srcB00 + srcA01 * srcB10 + srcA02 * srcB20;
		result[0][1] = srcA00 * srcB01 + srcA01 * srcB11 + srcA02 * srcB21;
		result[0][2] = srcA00 * srcB02 + srcA01 * srcB12 + srcA02 * srcB22;

		result[1][0] = srcA10 * srcB00 + srcA11 * srcB10 + srcA12 * srcB20;
		result[1][1] = srcA10 * srcB01 + srcA11 * srcB11 + srcA12 * srcB21;
		result[1][2] = srcA10 * srcB02 + srcA11 * srcB12 + srcA12 * srcB22;

		result[2][0] = srcA20 * srcB00 + srcA21 * srcB10 + srcA22 * srcB20;
		result[2][1] = srcA20 * srcB01 + srcA21 * srcB11 + srcA22 * srcB21;
		result[2][2] = srcA20 * srcB02 + srcA21 * srcB12 + srcA22 * srcB22;

		return result;
	}

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m, const T& s)
	{
		return Matrix3x3_t<T>(
			m[0] * s,
			m[1] * s,
			m[2] * s);
	}

	template<typename T>
	Matrix3x3_t<T> operator* (const T& s, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m[0] * s,
			m[1] * s,
			m[2] * s);
	}

	template <typename T>
	typename Matrix3x3_t<T>::row_type operator* (const Matrix3x3_t<T>& m, const typename Matrix3x3_t<T>::col_type& v)
	{
		return Matrix3x3_t<T>::row_type(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
	}

	template <typename T>
	typename Matrix3x3_t<T>::col_type operator* (const typename Matrix3x3_t<T>::row_type& v, const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>::col_type(
			v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0],
			v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1],
			v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2]);
	}

	template <typename T>
	bool operator== (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		return (m1[0] == m2[0] && m1[1] == m2[1] && m1[2] == m2[2]);
	}

	template <typename T>
	bool operator!= (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2)
	{
		return (m1[0] != m2[0] || m1[1] != m2[1] || m1[2] != m2[2]);
	}

	template <typename T>
	Matrix3x3_t<T> Matrix3x3_t<T>::Inverse(const Matrix3x3_t<T>& m)
	{
		T OneOverDeterminant = static_cast<T>(1) / (
			+m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
			- m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
			+ m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));

		Matrix3x3_t<T> inverse(0);

		inverse[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDeterminant;
		inverse[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDeterminant;
		inverse[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDeterminant;
		inverse[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDeterminant;
		inverse[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDeterminant;
		inverse[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDeterminant;
		inverse[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDeterminant;
		inverse[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDeterminant;
		inverse[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDeterminant;

		return inverse;
	}

	template <typename T>
	Matrix3x3_t<T> Matrix3x3_t<T>::Transpose(const Matrix3x3_t<T>& m)
	{
		return Matrix3x3_t<T>(
			m[0][0], m[1][0], m[2][0],
			m[0][1], m[1][1], m[2][1],
			m[0][2], m[1][2], m[2][2]);
	}

	template <class T>
	Matrix3x3_t<T> Matrix3x3_t<T>::Scale(const T& scaleX, const T& scaleY, const T& scaleZ)
	{
		return Matrix3x3_t<T>(
			scaleX, 0, 0,
			0, scaleY, 0,
			0, 0, scaleZ);
	}

	template <class T>
	Matrix3x3_t<T> Matrix3x3_t<T>::RotateX(const T& r)
	{
		return Matrix3x3_t<T>(
			1, 0, 0,
			0, cos(r), -sin(r),
			0, sin(r), cos(r));
	}

	template <class T>
	Matrix3x3_t<T> Matrix3x3_t<T>::RotateY(const T& r)
	{
		return Matrix3x3_t<T>(
			cos(r), 0, sin(r),
			0, 1, 0,
			-sin(r), 0, cos(r));
	}

	template <class T>
	Matrix3x3_t<T> Matrix3x3_t<T>::RotateZ(const T& r)
	{
		return Matrix3x3_t<T>(
			cos(r), -sin(r), 0,
			sin(r), cos(r), 0,
			0, 0, 1);
	}

	template <class T>
	Matrix3x3_t<T> Matrix3x3_t<T>::Translate(const T& x, const T& y)
	{
		return Matrix3x3_t<T>(
			1, 0, x,
			0, 1, y,
			0, 0, 1);
	}
}

#endif