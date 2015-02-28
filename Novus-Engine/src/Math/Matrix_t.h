//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MATRIX_T_H
#define NOVUS_MATRIX_T_H

namespace novus
{
	template <typename T> struct Matrix3x3_t;
	template <typename T> struct Matrix4x4_t;

	typedef Matrix3x3_t<float>     Matrix3x3f;
	typedef Matrix3x3_t<double>    Matrix3x3d;
	typedef Matrix3x3f             Matrix3x3;
	typedef Matrix3x3f             Matrix3;

	typedef Matrix4x4_t<float>     Matrix4x4f;
	typedef Matrix4x4_t<double>    Matrix4x4d;
	typedef Matrix4x4f             Matrix4x4;
	typedef Matrix4x4f             Matrix4;
}

#endif