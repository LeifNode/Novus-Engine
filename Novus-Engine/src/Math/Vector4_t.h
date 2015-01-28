//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Based off GLM's architecture

#pragma once

#ifndef NOVUS_VECTOR4_T_H
#define NOVUS_VECTOR4_T_H

#include "Vector_t.h"

namespace novus
{
	template <typename T>
	struct Vector4_t
	{
		union { T x, r; };
		union { T y, g; };
		union { T z, b; };
		union { T w, a; };

		Vector4_t();


		Vector4_t(const Vector4_t<T>& v);

		explicit Vector4_t(const T& s);

		Vector4_t(const T& a, const T& b, const T& c, const T& d);

		//Conversion constructors
		template <typename B>
		explicit Vector4_t(const Vector2_t<B>& v);

		template <typename B>
		explicit Vector4_t(const Vector3_t<B>& v);

		template <typename B>
		explicit Vector4_t(const Vector4_t<B>& v);



		template <typename A, typename B, typename C, typename D>
		Vector4_t(const A& a, const B& b, const C& c, const D& d);

		template <typename A, typename B, typename C>
		Vector4_t(const Vector2_t<A>& a, const B& b, const C& c);

		template <typename A, typename B, typename C>
		Vector4_t(const A& a, const B& b, const Vector2_t<C>& c);

		template <typename A, typename B, typename C>
		Vector4_t(const A& a, const Vector2_t<B>& b, const C& c);

		template <typename A, typename B>
		Vector4_t(const Vector2_t<A>& a, const Vector2_t<B>& b);

		template <typename A, typename B>
		Vector4_t(const Vector3_t<A>& a, const B& b);

		template <typename A, typename B>
		Vector4_t(const A& a, const Vector3_t<B>& b);

		size_t size() const;

		T& operator[] (size_t i);
		const T& operator[] (size_t i) const;

		//Arithmetic operators
		template <typename U>
		Vector4_t<T>& operator= (const Vector4_t<U>& v);

		template <typename U>
		Vector4_t<T>& operator+= (const Vector4_t<U>& v);

		template <typename U>
		Vector4_t<T>& operator-= (const Vector4_t<U>& v);

		template <typename U>
		Vector4_t<T>& operator*= (const Vector4_t<U>& v);

		template <typename U>
		Vector4_t<T>& operator/= (const Vector4_t<U>& v);


		template <typename U>
		Vector4_t<T>& operator+= (const U& s);

		template <typename U>
		Vector4_t<T>& operator-= (const U& s);

		template <typename U>
		Vector4_t<T>& operator*= (const U& s);

		template <typename U>
		Vector4_t<T>& operator/= (const U& s);
	};

	template <typename T>
	Vector4_t<T> operator+ (const Vector4_t<T>& a, const Vector4_t<T>& b);
	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& a, const Vector4_t<T>& b);
	template <typename T>
	Vector4_t<T> operator* (const Vector4_t<T>& a, const Vector4_t<T>& b);
	template <typename T>
	Vector4_t<T> operator/ (const Vector4_t<T>& a, const Vector4_t<T>& b);

	template <typename T>
	Vector4_t<T> operator+ (const Vector4_t<T>& v, const T& s);
	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& v, const T& s);
	template <typename T>
	Vector4_t<T> operator* (const Vector4_t<T>& v, const T& s);
	template <typename T>
	Vector4_t<T> operator/ (const Vector4_t<T>& v, const T& s);

	template <typename T>
	Vector4_t<T> operator+ (const T& s, const Vector4_t<T>& v);
	template <typename T>
	Vector4_t<T> operator- (const T& s, const Vector4_t<T>& v);
	template <typename T>
	Vector4_t<T> operator* (const T& s, const Vector4_t<T>& v);
	template <typename T>
	Vector4_t<T> operator/ (const T& s, const Vector4_t<T>& v);

	template <typename T>
	Vector4_t<T> operator- (const Vector4_t<T>& v);

	template <typename T>
	bool operator== (const Vector4_t<T>& v1, const Vector4_t<T>& v2);
	template <typename T>
	bool operator!= (const Vector4_t<T>& v1, const Vector4_t<T>& v2);

	template <typename T>
	T Length(const Vector4_t<T>& v);

	template <typename T>
	T LengthSq(const Vector4_t<T>& v);

	template <typename T>
	Vector4_t<T> Normalize(const Vector4_t<T>& v);

	template <typename T>
	T Dot(const Vector4_t<T>& v1, const Vector4_t<T>& v2);
}

#include "Vector4_t.inl"

#endif