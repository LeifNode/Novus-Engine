//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_PHYSICS_PARTICLE_COMPONENT_H
#define NOVUS_PHYSICS_PARTICLE_COMPONENT_H

#include "Core/ActorComponent.h"
#include "Physics/Particle.h"

namespace novus
{
	
class PhysicsParticleComponent : public ActorComponent
{
	NOVUS_OBJECT_DECL(PhysicsParticleComponent);

public:
	PhysicsParticleComponent();

};

NOVUS_OBJECT_DEF(PhysicsParticleComponent);

}

#endif