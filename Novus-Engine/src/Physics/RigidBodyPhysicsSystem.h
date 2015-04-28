//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_RIGID_BODY_PHYSICS_SYSTEM_H
#define NOVUS_RIGID_BODY_PHYSICS_SYSTEM_H

#include "Math/Math.h"
#include "RigidBody.h"
#include "Contact.h"
#include "ContactResolver.h"
#include "CollisionDetector.h"
#include <list>

namespace novus
{

class RigidBodyPhysicsSystem
{

public:
	RigidBodyPhysicsSystem(unsigned int maxContacts, unsigned int iterations = 0);
	~RigidBodyPhysicsSystem();

	void GenerateContacts();

	void Update(float dt);

	void StartFrame();

private:
	bool mCalculateIterations;

	std::list<RigidBody*> mRigidBodies;

	ContactResolver mResolver;

	Contact* mpContactArr;

	unsigned int mMaxContacts;
};

}

#endif