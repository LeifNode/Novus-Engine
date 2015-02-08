//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MASS_AGGREGATE_PHYSICS_SYSTEM_H
#define NOVUS_MASS_AGGREGATE_PHYSICS_SYSTEM_H

#include "Math/Math.h"
#include "Particle.h"
#include "ParticleForceGenerator.h"
#include <list>

namespace novus
{

class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Init();

	void Update(float dt);

	const std::vector<Particle*>& getParticles() const;

	void AddParticle(Particle* particle);
	void AddForceGenerator(ParticleForceGenerator* generator);
	void AddRegistryEntry(Particle* particle, ParticleForceGenerator* generator);

private:
	std::vector<Particle*> mParticles;

	std::list<ParticleForceGenerator*> mForceGenerators;
	ParticleForceRegistry mForceRegistry;
};

}

#endif