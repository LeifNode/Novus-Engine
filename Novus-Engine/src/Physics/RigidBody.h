//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_RIGIDBODY_H
#define NOVUS_RIGIDBODY_H

#include "Math/Math.h"
#include "Math/Transform.h"

namespace novus
{

class RigidBody
{
public:
	RigidBody();

	void CalculateDerivedData();
	void Integrate(float dt);

	void setMass(float mass);
	float getMass() const;

	void setInverseMass(float inverseMass);
	float getInverseMass() const;

	bool hasFiniteMass() const;

	void setInertiaTensor(const Matrix3& inertiaTensor);
	Matrix3 getInertiaTensor() const;
	Matrix3 getInertiaTensorWorld() const;

	void setInverseInertiaTensor(const Matrix3& inertiaTensor);
	Matrix3 getInverseInertiaTensor() const;
	Matrix3 getInverseInertiaTensorWorld() const;

	void setDamping(float linearDamping, float angularDamping);

	void setLinearDamping(float linearDamping);
	float getLinearDamping() const;

	void setAngularDamping(float angularDamping);
	float getAngularDamping() const;

	Transform* getTransform();

	void setVelocity(const Vector3& velocity);
	Vector3 getVelocity() const;

	void AddVelocity(const Vector3& dVelocity);

	void setAngularVelocity(const Vector3& angularVelocity);
	void setAngularVelocity(float angVelX, float angVelY, float angVelZ);

	Vector3 getAngularVelocity() const;

	void AddAngularVelocity(const Vector3& dAngularVelocity);

	Vector3 getLastFrameAcceleration() const;

	void ClearAccumilators();

	void AddForce(const Vector3& force);
	void AddForceAtPoint(const Vector3& force, const Vector3& point);

	void AddTorque(const Vector3& torque);

private:
	float mInverseMass;

	Matrix3 mInverseInteriaTensor;
	Matrix3 mInverseInteriaTensorWorld;

	float mLinearDamping;
	float mAngularDamping;

	Transform mTransform;

	Vector3 mVelocity;
	Vector3 mAngularVelocity;

	Vector3 mForceAccum;
	Vector3 mTorqueAccum;

	Vector3 mLastFrameAcceleration;
};

}

#endif