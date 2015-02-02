//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#include "Vector_t.h"

namespace novus
{

template <typename T> struct Quaternion_t;

typedef Quaternion_t<float>     FQuaternion;
typedef Quaternion_t<double>    DQuaternion;
typedef FQuaternion              Quaternion;

template <typename T>
struct Quaternion_t
{
	T x, y, z, w;

	Quaternion_t();

	Quaternion_t(const T& a, const T& b, const T& c, const T& d);

	template <typename B>
	Quaternion_t(const Quaternion_t<B>& q);

	template <typename B>
	Quaternion_t(const Vector4_t<B>& v);

	static T Magnitude(const Quaternion_t<T>& q);
	static Quaternion_t<T> Normalize(const Quaternion_t<T>& q);

	static T Dot(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

	static Quaternion_t<T> AxisAngle(const Vector3_t<T>& axis, const T& radians);
	static Matrix4x4_t<T> ToMatrix(const Quaternion_t<T>& q);

	static Quaternion_t<T> Slerp(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2, const T& t);
};

template <typename T>
Quaternion_t<T> operator- (const Quaternion_t<T>& q);

template <typename T>
Quaternion_t<T> operator+ (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

template <typename T>
Quaternion_t<T> operator- (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

template <typename T>
Quaternion_t<T> operator* (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

template <typename T>
Quaternion_t<T> operator* (const Quaternion_t<T>& q, const T& s);

template <typename T>
Quaternion_t<T> operator* (const T& s, const Quaternion_t<T>& q);

template <typename T>
Quaternion_t<T> operator/ (const Quaternion_t<T>& q, const T& s);

}