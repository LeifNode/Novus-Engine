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

	void Update(float dt);

	void AddForce(const Vector3d& force);

	void Reset();

	void setMass(float mass);
	float getMass() const;

	void setInverseMass(float inverseMass);
	float getInverseMass() const;

	void setPosition(const Vector3d& position);
	void setVelocity(const Vector3d& velocity);

	Vector3d getPosition() const;
	Vector3d getVelocity() const;
	Vector3d getAcceleration() const;

private:
	Vector3d mPosition;
	Vector3d mVelocity;
	Vector3d mAcceleration;

	float mDamping;
	float mInverseMass;
};

}

#endif