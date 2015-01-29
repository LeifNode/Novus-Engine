//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Based off GLM's architecture

#pragma once

#ifndef NOVUS_MATRIX4X4_T_H
#define NOVUS_MATRIX4X4_T_H

#include "Matrix_t.h"

namespace novus
{
	template <typename T>
	struct Matrix4x4_t
	{
		typedef Vector4_t<T> col_type;
		typedef Vector4_t<T> row_type;

		typedef T value_type;

	private:
		col_type value[4];

	public:

		Matrix4x4_t();

		Matrix4x4_t(const Matrix4x4_t<T>& m);
		explicit Matrix4x4_t(const T& s);

		Matrix4x4_t(const row_type& v1, const row_type& v2, const row_type& v3, const row_type& v4);
		Matrix4x4_t(
			const T& x1, const T& y1, const T& z1, const T& w1,
			const T& x2, const T& y2, const T& z2, const T& w2,
			const T& x3, const T& y3, const T& z3, const T& w3,
			const T& x4, const T& y4, const T& z4, const T& w4);

		//Conversion constructors
		template <typename B>
		explicit Matrix4x4_t(const Matrix4x4_t<B>& m);

		template <typename V1, typename V2, typename V3, typename V4>
		Matrix4x4_t(const Vector4_t<V1>& v1, const Vector4_t<V2>& v2, const Vector4_t<V3>& v3, const Vector4_t<V4>& v4);

		template
			<typename X1, typename Y1, typename Z1, typename W1,
			typename X2, typename Y2, typename Z2, typename W2,
			typename X3, typename Y3, typename Z3, typename W3,
			typename X4, typename Y4, typename Z4, typename W4>
		Matrix4x4_t(
			const X1& x1, const Y1& y1, const Z1& z1, const W1& w1,
			const X2& x2, const Y2& y2, const Z2& z2, const W2& w2,
			const X3& x3, const Y3& y3, const Z3& z3, const W3& w3,
			const X4& x4, const Y4& y4, const Z4& z4, const W4& w4);

		explicit Matrix4x4_t(const Matrix3x3_t<T>& m);

		size_t size() const;

		row_type& operator[] (size_t i);
		const row_type& operator[] (size_t i) const;

		template <typename U>
		Matrix4x4_t<T>& operator= (const Matrix4x4_t<U>& m);

		template <typename U>
		Matrix4x4_t<T>& operator+= (const Matrix4x4_t<U>& m);

		template <typename U>
		Matrix4x4_t<T>& operator-= (const Matrix4x4_t<U>& m);

		template <typename U>
		Matrix4x4_t<T>& operator*= (const Matrix4x4_t<U>& m);

		template <typename U>
		Matrix4x4_t<T>& operator/= (const Matrix4x4_t<U>& m);

		template <typename U>
		Matrix4x4_t<T>& operator+= (const U& s);

		template <typename U>
		Matrix4x4_t<T>& operator-= (const U& s);

		template <typename U>
		Matrix4x4_t<T>& operator*= (const U& s);

		template <typename U>
		Matrix4x4_t<T>& operator/= (const U& s);


		static Matrix4x4_t<T> Inverse(const Matrix4x4_t<T>& m);

		static Matrix4x4_t<T> Transpose(const Matrix4x4_t<T>& m);

		static Matrix4x4_t<T> Scale(const T& scaleX, const T& scaleY, const T& scaleZ);

		static Matrix4x4_t<T> RotateX(const T& r);
		static Matrix4x4_t<T> RotateY(const T& r);
		static Matrix4x4_t<T> RotateZ(const T& r);

		static Matrix4x4_t<T> Translate(const T& x, const T& y, const T& z);

		static Matrix4x4_t<T> LookToward(const Vector3_t<T>& eyePosition, const Vector3_t<T>& lookDirection, const Vector3_t<T>& up);

		static Matrix4x4_t<T> LookAt(const Vector3_t<T>& eyePosition, const Vector3_t<T>& lookAtPosition, const Vector3_t<T>& up);

		static Matrix4x4_t<T> Perspective(T fovAngleY, T aspectHByW, T nearZ, T farZ);
		static Matrix4x4_t<T> Orthographic();
	};

	template <typename T>
	Matrix4x4_t<T> operator+ (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2);

	template <typename T>
	Matrix4x4_t<T> operator+ (const Matrix4x4_t<T>& m, const T& s);

	template<typename T>
	Matrix4x4_t<T> operator+ (const T& s, const Matrix4x4_t<T>& m);

	template <typename T>
	Matrix4x4_t<T> operator- (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2);

	template <typename T>
	Matrix4x4_t<T> operator- (const Matrix4x4_t<T>& m, const T& s);

	template<typename T>
	Matrix4x4_t<T> operator- (const T& s, const Matrix4x4_t<T>& m);

	template <typename T>
	Matrix4x4_t<T> operator* (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2);

	template <typename T>
	Matrix4x4_t<T> operator* (const Matrix4x4_t<T>& m, const T& s);

	template<typename T>
	Matrix4x4_t<T> operator* (const T& s, const Matrix4x4_t<T>& m);

	template <typename T>
	typename Matrix4x4_t<T>::row_type operator* (const Matrix4x4_t<T>& m, const typename Matrix4x4_t<T>::col_type& v);

	template <typename T>
	typename Matrix4x4_t<T>::col_type operator* (const typename Matrix4x4_t<T>::row_type& v, const Matrix4x4_t<T>& m);

	template <typename T>
	bool operator== (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2);

	template <typename T>
	bool operator!= (const Matrix4x4_t<T>& m1, const Matrix4x4_t<T>& m2);
}

//#include "Matrix4x4_t.inl"

#endif