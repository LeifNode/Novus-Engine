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

	typedef Matrix3x3_t<float>     FMatrix3x3;
	typedef Matrix3x3_t<double>    DMatrix3x3;
	typedef FMatrix3x3             Matrix3x3;
	typedef FMatrix3x3             Matrix3;

	typedef Matrix4x4_t<float>     FMatrix4x4;
	typedef Matrix4x4_t<double>    DMatrix4x4;
	typedef FMatrix4x4             Matrix4x4;
	typedef FMatrix4x4             Matrix4;
}

#endif