//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Based off GLM's architecture

#pragma once

#ifndef NOVUS_VECTOR3_T_H
#define NOVUS_VECTOR3_T_H

#include "Vector_t.h"

namespace novus
{
	template <typename T>
	struct Vector3_t
	{
		union { T x, r; };
		union { T y, g; };
		union { T z, b; };

		Vector3_t();


		Vector3_t(const Vector3_t<T>& v);

		explicit Vector3_t(const T& s);

		Vector3_t(const T& a, const T& b, const T& c);
			
		//Conversion constructors
		template <typename B>
		explicit Vector3_t(const Vector2_t<B>& v);

		template <typename B>
		explicit Vector3_t(const Vector3_t<B>& v);
		
		template <typename B>
		explicit Vector3_t(const Vector4_t<B>& v);

		template <typename A, typename B, typename C>
		Vector3_t(const A& a, const B& b, const C& c);

		template <typename A, typename B>
		Vector3_t(const Vector2_t<A>& a, const B& b);
			
		template <typename A, typename B>
		Vector3_t(const A& a, const Vector2_t<B>& b);

		size_t size() const;

		T& operator[] (size_t i);
		const T& operator[] (size_t i) const;

		//Arithmetic operators
		template <typename U>
		Vector3_t<T>& operator= (const Vector3_t<U>& v);

		template <typename U>
		Vector3_t<T>& operator+= (const Vector3_t<U>& v);

		template <typename U>
		Vector3_t<T>& operator-= (const Vector3_t<U>& v);

		template <typename U>
		Vector3_t<T>& operator*= (const Vector3_t<U>& v);

		template <typename U>
		Vector3_t<T>& operator/= (const Vector3_t<U>& v);

		template <typename U>
		Vector3_t<T>& operator+= (const U& s);

		template <typename U>
		Vector3_t<T>& operator-= (const U& s);

		template <typename U>
		Vector3_t<T>& operator*= (const U& s);

		template <typename U>
		Vector3_t<T>& operator/= (const U& s);
	};

	template <typename T>
	Vector3_t<T> operator+ (const Vector3_t<T>& a, const Vector3_t<T>& b);
	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& a, const Vector3_t<T>& b);
	template <typename T>
	Vector3_t<T> operator* (const Vector3_t<T>& a, const Vector3_t<T>& b);
	template <typename T>
	Vector3_t<T> operator/ (const Vector3_t<T>& a, const Vector3_t<T>& b);

	template <typename T>
	Vector3_t<T> operator+ (const Vector3_t<T>& v, const T& s);
	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& v, const T& s);
	template <typename T>
	Vector3_t<T> operator* (const Vector3_t<T>& v, const T& s);
	template <typename T>
	Vector3_t<T> operator/ (const Vector3_t<T>& v, const T& s);

	template <typename T>
	Vector3_t<T> operator+ (const T& s, const Vector3_t<T>& v);
	template <typename T>
	Vector3_t<T> operator- (const T& s, const Vector3_t<T>& v);
	template <typename T>
	Vector3_t<T> operator* (const T& s, const Vector3_t<T>& v);
	template <typename T>
	Vector3_t<T> operator/ (const T& s, const Vector3_t<T>& v);

	template <typename T>
	Vector3_t<T> operator- (const Vector3_t<T>& v);

	template <typename T>
	bool operator== (const Vector3_t<T>& v1, const Vector3_t<T>& v2);
	template <typename T>
	bool operator!= (const Vector3_t<T>& v1, const Vector3_t<T>& v2);

	template <typename T>
	T Length(const Vector3_t<T>& v);

	template <typename T>
	T LengthSq(const Vector3_t<T>& v);

	template <typename T>
	Vector3_t<T> Normalize(const Vector3_t<T>& v);

	template <typename T>
	T Dot(const Vector3_t<T>& v1, const Vector3_t<T>& v2);

	template <typename T>
	Vector3_t<T> Cross(const Vector3_t<T>& v1, const Vector3_t<T>& v2);
}

//#include "Vector3_t.inl"

#endif