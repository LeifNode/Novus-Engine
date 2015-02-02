//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TRANSFORM_H
#define NOVUS_TRANSFORM_H

#include "Matrix4x4_t.h"
#include "Vector3_t.h"
#include "Vector4_t.h"
#include "Quaternion_t.h"

namespace novus
{
class Transform
{
	Transform();
	~Transform();

	void Rotate(const Quaternion& rotationQuaternion);

	Quaternion GetRotation() const;
	void SetRotation(const Quaternion& rotation);// { mTransformDirty = true; mRotationQuat = rotation; }

	Vector3 GetScale() const;// { return mScaling; }

	void Scale(float scale);
	void Scale(float x, float y, float z);
	void Scale(const Vector3& scaling);

	void SetScale(float scale);
	void SetScale(float scaleX, float scaleY, float scaleZ);
	void SetScale(const Vector3& scale);

	void Translate(const Vector3& offset);

	void SetPosition(const Vector3& translation);// { mTranslation = translation; }
	Vector3 GetPosition() const;// { return mTranslation; }

	Matrix4 GetTransform() const;

	void reset();

private:
	void MarkDirty();

private:
	mutable bool mTransformDirty;
	mutable bool mInverseDirty;

	Matrix4    mTransform;

	Quaternion mRotation;
	Vector3    mScale;
	Vector3    mTranslation;
};
}

#endif