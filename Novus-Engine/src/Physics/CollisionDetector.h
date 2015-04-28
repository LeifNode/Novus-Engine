//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_COLLISION_DETECTOR_H
#define NOVUS_COLLISION_DETECTOR_H

#include "Math/Math.h"
#include "CollisionPrimitives.h"
#include "Contact.h"

namespace novus
{

struct CollisionData
{
	Contact* contactArray;

	Contact* contacts;

	int contactsLeft;

	unsigned int contactCount;

	float friction;

	float restitution;

	float tolerance;

	bool hasMoreContacts() const
	{
		return contactsLeft > 0;
	}

	void Reset(unsigned int maxContacts)
	{
		contactsLeft = maxContacts;
		contactCount = 0;
		contacts = contactArray;
	}

	void AddContacts(unsigned int count)
	{
		contactsLeft -= count;
		contactCount += count;

		contacts += count;
	}
};

class CollisionDetector
{
public:
	static unsigned int SphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
	static unsigned int SphereAndSphere(const CollisionSphere& one, const CollisionSphere& two, CollisionData* data);

	static unsigned int BoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data);
	static unsigned int BoxAndBox(const CollisionBox& one, const CollisionBox& two, CollisionData* data);

	static unsigned int BoxAndSphere(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data);
};

}

#endif