#include "MatrixStack.h"
#include "Utils/Logging/Logger.h"

using novus::MatrixStack;
using novus::Matrix4;

MatrixStack::MatrixStack()
:mMatrixStack(),
mTopInverseDirty(true)
{
	mMatrixStack.push_back(Matrix4(1.0f));
}

void MatrixStack::Push(const Matrix4& m)
{
	Matrix4 newTop = m * mMatrixStack.back();
	mMatrixStack.push_back(newTop);

	mTopInverseDirty = true;
}

void MatrixStack::Pop()
{
	mMatrixStack.pop_back();

	if (mMatrixStack.empty())
	{
		mMatrixStack.push_back(Matrix4(1.0f));
		NE_WARN("Pop called on matrix stack when it was already empty.\n", "MatrixStack");
	}

	mTopInverseDirty = true;
}

const Matrix4& MatrixStack::getTop() const
{
	return mMatrixStack.back();
}

const Matrix4& MatrixStack::getTopInverse() const
{
	if (mTopInverseDirty)
	{
		mTopInverse = Matrix4::Inverse(mMatrixStack.back());
		mTopInverseDirty = false;
	}

	return mTopInverse;
}