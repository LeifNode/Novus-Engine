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
namespace math
{
	template <typename T>
	struct Matrix3x3_t
	{
		typedef Vector3_t<T> col_type;
		typedef Vector3_t<T> row_type;


	};
}
}



#endif