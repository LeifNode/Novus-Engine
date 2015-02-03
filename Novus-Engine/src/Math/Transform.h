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
public:
	Transform();
	~Transform();

	void Rotate(const Quaternion& rotationQuaternion);

	const Quaternion& GetRotation() const;
	void SetRotation(const Quaternion& rotation);

	const Vector3& GetScale() const;

	void Scale(float scale);
	void Scale(float x, float y, float z);
	void Scale(const Vector3& scaling);

	void SetScale(float scale);
	void SetScale(float scaleX, float scaleY, float scaleZ);
	void SetScale(const Vector3& scale);

	void Translate(const Vector3& offset);

	void SetPosition(const Vector3& position);
	const Vector3& GetPosition() const;

	const Matrix4& GetTransform() const;

	void Reset();

private:
	void MarkDirty();

private:
	mutable bool mTransformDirty;

	mutable Matrix4 mTransform;

	Quaternion mRotation;
	Vector3    mScale;
	Vector3    mPosition;
};
}

#endif