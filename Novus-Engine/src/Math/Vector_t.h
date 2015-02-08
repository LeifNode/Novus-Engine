//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_VECTOR_T_H
#define NOVUS_VECTOR_T_H

namespace novus
{
	template <typename T> struct Vector2_t;
	template <typename T> struct Vector3_t;
	template <typename T> struct Vector4_t;

	typedef Vector2_t<float>        Vector2f;
	typedef Vector2_t<double>       Vector2d;
	typedef Vector2_t<int>          Vector2i;
	typedef Vector2_t<unsigned int> Vector2u;
	typedef Vector2f                 Vector2;

	typedef Vector3_t<float>        Vector3f;
	typedef Vector3_t<double>       Vector3d;
	typedef Vector3_t<int>          Vector3i;
	typedef Vector3_t<unsigned int> Vector3u;
	typedef Vector3f                 Vector3;

	typedef Vector4_t<float>        Vector4f;
	typedef Vector4_t<double>       Vector4d;
	typedef Vector4_t<int>          Vector4i;
	typedef Vector4_t<unsigned int> Vector4u;
	typedef Vector4f                 Vector4;
}

#endif