//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef PLANETARY_GRAVITATION_GENERATOR_H
#define PLANETARY_GRAVITATION_GENERATOR_H

#include <Application/Common.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/ParticleForceGenerator.h>

class PlanetaryGravitationGenerator : public novus::ParticleForceGenerator
{
public:
	PlanetaryGravitationGenerator(class novus::PhysicsSystem* physicsSystem);

	virtual void UpdateForce(novus::Particle* particle, float dt) override;

private:
	class novus::PhysicsSystem* mpPhysicsSystem;
};


#endif