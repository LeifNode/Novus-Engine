//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONTACT_H
#define NOVUS_CONTACT_H

#include "Math/Math.h"

namespace novus
{

class RigidBody;
class ContactResolver;

class Contact
{
	friend class ContactResolver;

public:
	void setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution);

protected:

	void CalculateInternals(float dt);

	void SwapBodies();

	void CalculateDesiredDeltaVelocity(float dt);

	Vector3 CalculateLocalVelocity(unsigned int bodyIndex, float dt);

	void CalculateContactBasis();

	//void ApplyImpulse(const Vector3& impulse, RigidBody* body, Vector3* velocityChangeOut, Vector3* rotationChangeOut);

	void ApplyVelocityChange(Vector3 velocityChange[2], Vector3 rotationChange[2]);

	void ApplyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);

	void ApplyLinearPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);

	Vector3 CalculateFrictionlessImpulse(Matrix3* inverIntertiaTensor);

public:
	RigidBody* body[2];

	float friction;

	float restitution;

	Vector3 contactPoint;

	Vector3 contactNormal;

	float penetration;

protected:
	Matrix3 mContactToWorld;
	Matrix3 mWorldToContact;

	Vector3 mContactVelocity;

	float mDesiredDeltaVelocity;

	Vector3 mRelativeContactPosition[2];
};

}

#endif