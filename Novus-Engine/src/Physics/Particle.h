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

	void AddForce(const DVector3& force);

	void Reset();

	void setMass(float mass);
	float getMass() const;

	void setInverseMass(float inverseMass);
	float getInverseMass() const;

	void setPosition(const DVector3& position);
	void setVelocity(const DVector3& velocity);

	DVector3 getPosition() const;
	DVector3 getVelocity() const;
	DVector3 getAcceleration() const;

private:
	DVector3 mPosition;
	DVector3 mVelocity;
	DVector3 mAcceleration;

	float mDamping;
	float mInverseMass;
};

}

#endif