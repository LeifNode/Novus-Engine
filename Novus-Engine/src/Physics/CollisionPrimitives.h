//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_COLLISION_PRIMITIVES_H
#define NOVUS_COLLISION_PRIMITIVES_H

#include "Math/Math.h"

namespace novus
{

class IntersectionTests;
class CollisionDetector;
class RigidBody;

class CollisionPrimitive
{
	friend class IntersectionTests;
	friend class CollisionDetector;

public:
	RigidBody* body;

	//Offset relative to the rigidbody
	Matrix4 offset;

	void CalculateInternals();

	Vector3 getAxis(unsigned int index) const
	{
		return Vector3(mTransform[index]);
	}

	const Matrix4& getTransform() const
	{
		return mTransform;
	}

protected:
	//The combined transform of the rigidbody and offset
	Matrix4 mTransform;
};

class CollisionSphere : public CollisionPrimitive
{
public:
	float radius;
};

class CollisionPlane
{
public:
	Vector3 direction;

	//Vector3 origin;
	float offset;
};

class CollisionBox : public CollisionPrimitive
{
public:
	Vector3 halfSize;
};



}

#endif