#include "Transform.h"
#include "Math.h"

namespace novus
{

Transform::Transform()
	: mScale(1.0f),
	mTransformDirty(true),
	mInverseTransformDirty(true)
{}

Transform::~Transform()
{}

void Transform::Rotate(const Quaternion& rotationQuaternion)
{
	mRotation = mRotation * rotationQuaternion;
	mRotation = Quaternion::Normalize(mRotation);
	MarkDirty();
}

void Transform::Rotate(const Vector3& rotationAxis)
{
	mRotation += rotationAxis;
	mRotation = Quaternion::Normalize(mRotation);
	MarkDirty();
}

const Quaternion& Transform::GetRotation() const
{
	return mRotation;
}

void Transform::SetRotation(const Quaternion& rotation)
{
	MarkDirty();
	mRotation = rotation;
}

const Vector3& Transform::GetScale() const
{
	return mScale;
}

void Transform::Scale(float scale)
{
	mScale *= scale;
	MarkDirty();
}

void Transform::Scale(float x, float y, float z)
{
	mScale *= Vector3(x, y, z);
	MarkDirty();
}

void Transform::Scale(const Vector3& scaling)
{
	mScale *= scaling;
	MarkDirty();
}

void Transform::SetScale(float scale)
{
	mScale = Vector3(scale);
	MarkDirty();
}

void Transform::SetScale(float scaleX, float scaleY, float scaleZ)
{
	mScale = Vector3(scaleX, scaleY, scaleZ);
	MarkDirty();
}

void Transform::SetScale(const Vector3& scale)
{
	mScale = scale;
	MarkDirty();
}

void Transform::Translate(const Vector3& offset)
{
	mPosition += offset;
	MarkDirty();
}

void Transform::SetPosition(const Vector3& position)
{
	mPosition = position;
	MarkDirty();
}

const Vector3& Transform::GetPosition() const
{
	return mPosition;
}

const Matrix4& Transform::GetTransform() const
{
	if (mTransformDirty)
	{
		mTransform = Quaternion::ToMatrix(mRotation) * Matrix4::Scale(mScale) * Matrix4::Translate(mPosition);
		mTransformDirty = false;
	}

	return mTransform;
}

const Matrix4& Transform::GetInverseTransform() const
{
	if (mInverseTransformDirty)
	{
		mInverseTransform = Matrix4::Inverse(GetTransform());
		mInverseTransformDirty = false;
	}

	return mTransform;
}

Vector3 Transform::WorldPointToLocal(const Vector3& point) const
{
	return Vector3(GetInverseTransform() * Vector4(point, 1.0f));
}

Vector3 Transform::LocalPointToWorld(const Vector3& point) const
{
	//TODO: Should probably just inline this stuff at some point
	return Vector3(GetTransform() * Vector4(point, 1.0f));
}

Vector3 Transform::WorldDirToLocal(const Vector3& direction) const
{
	return Vector3(GetInverseTransform() * Vector4(direction, 0.0f));
}

Vector3 Transform::LocalDirToWorld(const Vector3& direction) const
{
	return Vector3(GetTransform() * Vector4(direction, 0.0f));
}

void Transform::Reset()
{
	mTransform = Matrix4(1.0f);
	mRotation = Quaternion();
	mPosition = Vector3();
	mScale = Vector3(1.0f);

	mTransformDirty = false;
}

void Transform::MarkDirty()
{
	mTransformDirty = true;
	mInverseTransformDirty = true;
}

}