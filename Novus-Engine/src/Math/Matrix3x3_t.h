//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Based off GLM's architecture

#pragma once

#ifndef NOVUS_MATRIX3X3_T_H
#define NOVUS_MATRIX3X3_T_H

#include "Matrix_t.h"
#include "Vector_t.h"

namespace novus
{
	template <typename T>
	struct Matrix3x3_t
	{
		typedef Vector3_t<T> col_type;
		typedef Vector3_t<T> row_type;

		typedef T value_type;

	private:
		col_type value[3];

	public:

		Matrix3x3_t();

		Matrix3x3_t(const Matrix3x3_t<T>& m);
		explicit Matrix3x3_t(const T& s);

		Matrix3x3_t(const row_type& v1, const row_type& v2, const row_type& v3);
		Matrix3x3_t(
			const T& x1, const T& y1, const T& z1,
			const T& x2, const T& y2, const T& z2,
			const T& x3, const T& y3, const T& z3);

		//Conversion constructors
		template <typename B>
		explicit Matrix3x3_t(const Matrix3x3_t<B>& m);

		template <typename V1, typename V2, typename V3>
		Matrix3x3_t(const Vector3_t<V1>& v1, const Vector3_t<V2>& v2, const Vector3_t<V3>& v3);

		template 
		   <typename X1, typename Y1, typename Z1,
			typename X2, typename Y2, typename Z2,
			typename X3, typename Y3, typename Z3>
		Matrix3x3_t(
			const X1& x1, const Y1& y1, const Z1& z1,
			const X2& x2, const Y2& y2, const Z2& z2,
			const X3& x3, const Y3& y3, const Z3& z3);

		explicit Matrix3x3_t(const Matrix4x4_t<T>& m);

		size_t size() const;

		row_type& operator[] (size_t i);
		const row_type& operator[] (size_t i) const;

		template <typename U>
		Matrix3x3_t<T>& operator= (const Matrix3x3_t<U>& m);

		template <typename U>
		Matrix3x3_t<T>& operator+= (const Matrix3x3_t<U>& m);

		template <typename U>
		Matrix3x3_t<T>& operator-= (const Matrix3x3_t<U>& m);

		template <typename U>
		Matrix3x3_t<T>& operator*= (const Matrix3x3_t<U>& m);

		template <typename U>
		Matrix3x3_t<T>& operator/= (const Matrix3x3_t<U>& m);

		template <typename U>
		Matrix3x3_t<T>& operator+= (const U& s);

		template <typename U>
		Matrix3x3_t<T>& operator-= (const U& s);

		template <typename U>
		Matrix3x3_t<T>& operator*= (const U& s);

		template <typename U>
		Matrix3x3_t<T>& operator/= (const U& s);

		static Matrix3x3_t<T> Invert(const Matrix3x3_t<T>& m);

		static Matrix3x3_t<T> Transpose(const Matrix3x3_t<T>& m);

		static Matrix3x3_t<T> Scale(const T& scaleX, const T& scaleY, const T& scaleZ);

		static Matrix3x3_t<T> RotateX(const T& r);
		static Matrix3x3_t<T> RotateY(const T& r);
		static Matrix3x3_t<T> RotateZ(const T& r);

		static Matrix3x3_t<T> Translate(const T& x, const T& y);
	};

	template <typename T>
	Matrix3x3_t<T> operator+ (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2);

	template <typename T>
	Matrix3x3_t<T> operator+ (const Matrix3x3_t<T>& m, const T& s);

	template<typename T>
	Matrix3x3_t<T> operator+ (const T& s, const Matrix3x3_t<T>& m);

	template <typename T>
	Matrix3x3_t<T> operator- (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2);

	template <typename T>
	Matrix3x3_t<T> operator- (const Matrix3x3_t<T>& m, const T& s);

	template<typename T>
	Matrix3x3_t<T> operator- (const T& s, const Matrix3x3_t<T>& m);

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2);

	template <typename T>
	Matrix3x3_t<T> operator* (const Matrix3x3_t<T>& m, const T& s);

	template<typename T>
	Matrix3x3_t<T> operator* (const T& s, const Matrix3x3_t<T>& m);

	template <typename T>
	typename Matrix3x3_t<T>::row_type operator* (const Matrix3x3_t<T>& m, const typename Matrix3x3_t<T>::col_type& v);

	template <typename T>
	typename Matrix3x3_t<T>::col_type operator* (const typename Matrix3x3_t<T>::row_type& v, const Matrix3x3_t<T>& m);

	template <typename T>
	bool operator== (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2);

	template <typename T>
	bool operator!= (const Matrix3x3_t<T>& m1, const Matrix3x3_t<T>& m2);
}

//#include "Matrix3x3_t.inl"

#endif