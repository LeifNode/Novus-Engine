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

	typedef Vector2_t<float>        FVector2;
	typedef Vector2_t<double>       DVector2;
	typedef FVector2                 Vector2;

	typedef Vector3_t<float>        FVector3;
	typedef Vector3_t<double>       DVector3;
	typedef FVector3                 Vector3;

	typedef Vector4_t<float>        FVector4;
	typedef Vector4_t<double>       DVector4;
	typedef FVector4                 Vector4;
}

#endif