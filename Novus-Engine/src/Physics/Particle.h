//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_PHYSICS_PARTICLE_H
#define NOVUS_PHYSICS_PARTICLE_H

#include "Math/Math.h"

namespace novus
{

class Particle
{
public:
	Particle();
	virtual ~Particle() {};

	virtual void Update(float dt);

	void AddForce(const Vector3& force);

	void Reset();

	void setMass(float mass);
	float getMass() const;

	void setInverseMass(float inverseMass);
	float getInverseMass() const;

	void setDamping(float damping);

	void setPosition(const Vector3& position);
	void setVelocity(const Vector3& velocity);

	Vector3 getPosition() const;
	Vector3 getVelocity() const;
	Vector3 getAcceleration() const;

protected:
	Vector3 mPosition;
	Vector3 mVelocity;
	Vector3 mAcceleration;

	float mDamping;
	float mInverseMass;
};

}

#endif