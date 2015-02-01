//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MATRIX_STACK_H
#define NOVUS_MATRIX_STACK_H

#include "Matrix_t.h"
#include "Matrix4x4_t.h"
#include <vector>

namespace novus
{
class MatrixStack
{
public:
	MatrixStack();

	void Push(const Matrix4& m);
	void Pop();

	void Clear();

	const Matrix4& getTop() const;
	const Matrix4& getTopInverse() const;

private:
	mutable Matrix4 mTopInverse;
	mutable bool mTopInverseDirty;

	std::vector<Matrix4> mMatrixStack;
};
}


#endif