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
namespace math
{
	template <typename T> struct Vector2_t;
	template <typename T> struct Vector3_t;
	template <typename T> struct Vector4_t;

	typedef Vector2_t<int>          iVector2;
	typedef Vector2_t<float>        fVector2;
	typedef Vector2_t<double>       dVector2;
	typedef Vector2_t<unsigned int> uVector2;
	typedef Vector2_t<bool>         bVector2;
	typedef fVector2                 Vector2;

	typedef Vector3_t<int>          iVector3;
	typedef Vector3_t<float>        fVector3;
	typedef Vector3_t<double>       dVector3;
	typedef Vector3_t<unsigned int> uVector3;
	typedef Vector3_t<bool>         bVector3;
	typedef fVector3                 Vector3;

	typedef Vector4_t<int>          iVector4;
	typedef Vector4_t<float>        fVector4;
	typedef Vector4_t<double>       dVector4;
	typedef Vector4_t<unsigned int> uVector4;
	typedef Vector4_t<bool>         bVector4;
	typedef fVector4                 Vector4;
}
}

#endif