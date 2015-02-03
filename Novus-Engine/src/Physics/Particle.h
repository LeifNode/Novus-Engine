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

private:
	Vector3 mPosition;
	Vector3 mVelocity;
	Vector3 mAcceleration;

	Vector3 mForceAccumulator;

	float mDamping;
	float mInverseMass;
};

}

#endif