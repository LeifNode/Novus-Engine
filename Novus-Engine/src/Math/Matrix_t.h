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
namespace math
{
	template <typename T> struct Matrix3x3_t;
	template <typename T> struct Matrix4x4_t;

	typedef Matrix3x3_t<float>     fMatrix3x3;
	typedef Matrix3x3_t<double>    dMatrix3x3;
	typedef Matrix3x3_t<int>       iMatrix3x3;
	typedef fMatrix3x3             Matrix3x3;
	typedef fMatrix3x3             Matrix3;

	typedef Matrix4x4_t<float>     fMatrix4x4;
	typedef Matrix4x4_t<double>    dMatrix4x4;
	typedef Matrix4x4_t<int>       iMatrix4x4;
	typedef fMatrix4x4             Matrix4x4;
	typedef fMatrix4x4             Matrix4;
}
}

#endif