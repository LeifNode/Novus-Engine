//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_PARTICLE_CONTACTS_H
#define NOVUS_PARTICLE_CONTACTS_H

#include "Particle.h"

namespace novus
{

class ParticleContact
{
	friend class ParticleContactResolver;
public:
	ParticleContact();

	Particle* particle[2];

	float restitution;

	Vector3 contactNormal;

	float penetration;

	Vector3 particleMovement[2];

protected:
	void Resolve(float dt);

	float CalculateSeparatingVelocity() const;

private:
	void ResolveVelocity(float dt);
	void ResolveInterpenetration(float dt);
};

class ParticleContactResolver
{
public:
	ParticleContactResolver();

	void ResolveContacts(ParticleContact* contactArr, unsigned int numContacts, unsigned int maxIterations, float dt);

protected:

	//For perfromance tracking
	unsigned iterationsUsed;
};

}

#endif