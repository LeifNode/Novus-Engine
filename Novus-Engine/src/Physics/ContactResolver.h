//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONTACT_RESOLVER_H
#define NOVUS_CONTACT_RESOLVER_H

#include "Math/Math.h"

namespace novus
{

class Contact;

class ContactResolver
{
public:
	ContactResolver(unsigned int iterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);
	ContactResolver(unsigned int velocityIterations, unsigned int positionIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);

	void setIterations(unsigned int velocityIterations, unsigned int positionIterations);

	void setIterations(unsigned int iterations);

	void setEpsilon(float velocityEpsilon, float positionEpsolon);

	void ResolveContacts(Contact* contactArr, unsigned int numContacts, float dt);

protected:
	void PrepareContacts(Contact* contactArr, unsigned int numContacts, float dt);
	void AdjustVelocities(Contact* contactArr, unsigned int numContacts, float dt);
	void AdjustPositions(Contact* contactArr, unsigned int numContacts, float dt);

public:
	unsigned int velocityIterationsUsed;
	unsigned int positionIterationsUsed;

protected:
	unsigned int mVelocityIterations;
	unsigned int mPositionIterations;

	float mVelocityEpsilon;
	float mPositionEpsilon;

};

}

#endif