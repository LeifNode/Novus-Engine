#include "ContactResolver.h"
#include "Contact.h"

namespace novus
{

ContactResolver::ContactResolver(unsigned int iterations, float velocityEpsilon, float positionEpsilon)
:mPositionIterations(iterations),
mVelocityIterations(iterations),
mVelocityEpsilon(velocityEpsilon),
mPositionEpsilon(positionEpsilon)
{
}

ContactResolver::ContactResolver(unsigned int velocityIterations, unsigned int positionIterations, float velocityEpsilon, float positionEpsilon)
:mPositionIterations(positionIterations),
mVelocityIterations(velocityIterations),
mVelocityEpsilon(velocityEpsilon),
mPositionEpsilon(positionEpsilon)
{
}

void ContactResolver::setIterations(unsigned int velocityIterations, unsigned int positionIterations)
{
	mVelocityIterations = velocityIterations;
	mPositionIterations = positionIterations;
}

void ContactResolver::setIterations(unsigned int iterations)
{
	setIterations(iterations, iterations);
}

void ContactResolver::setEpsilon(float velocityEpsilon, float positionEpsolon)
{
	mVelocityEpsilon = velocityEpsilon;
	mPositionEpsilon = positionEpsolon;
}

void ContactResolver::ResolveContacts(Contact* contactArr, unsigned int numContacts, float dt)
{
	if (numContacts == 0)
		return;
	
	//Prepare contacts for processing
	PrepareContacts(contactArr, numContacts, dt);

	//Resolve interpenetration
	AdjustPositions(contactArr, numContacts, dt);

	//Resolve the velocities
	AdjustVelocities(contactArr, numContacts, dt);
}

void ContactResolver::PrepareContacts(Contact* contactArr, unsigned int numContacts, float dt)
{
	Contact* lastContact = contactArr + numContacts;
	for (Contact* contact = contactArr; contact < lastContact; contact++)
	{
		contact->CalculateInternals(dt);
	}
}

void ContactResolver::AdjustVelocities(Contact* c, unsigned int numContacts, float dt)
{
	Vector3 velocityChange[2], rotationChange[2];
	Vector3 deltaVel;

	velocityIterationsUsed = 0;
	while (velocityIterationsUsed < mVelocityIterations)
	{
		float max = mVelocityEpsilon;
		unsigned index = numContacts;
		for (unsigned i = 0; i < numContacts; i++)
		{
			if (c[i].mDesiredDeltaVelocity > max)
			{
				max = c[i].mDesiredDeltaVelocity;
				index = i;
			}
		}
		if (index == numContacts) 
			break;


		c[index].ApplyVelocityChange(velocityChange, rotationChange);

		for (unsigned i = 0; i < numContacts; i++)
		{
			for (unsigned b = 0; b < 2; b++) if (c[i].body[b])
			{
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i].body[b] == c[index].body[d])
					{
						deltaVel = velocityChange[d] +
							rotationChange[d] * c[i].mRelativeContactPosition[b];

						c[i].mContactVelocity += (c[i].mWorldToContact * deltaVel) * (b ? -1.0f : 1.0f);
						c[i].CalculateDesiredDeltaVelocity(dt);
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}

void ContactResolver::AdjustPositions(Contact* c, unsigned int numContacts, float dt)
{
	unsigned i, index;
	Vector3 linearChange[2], angularChange[2];
	float max;
	Vector3 deltaPosition;

	positionIterationsUsed = 0;
	while (positionIterationsUsed < mPositionIterations)
	{
		max = mPositionEpsilon;
		index = numContacts;
		for (i = 0; i<numContacts; i++)
		{
			if (c[i].penetration > max)
			{
				max = c[i].penetration;
				index = i;
			}
		}
		if (index == numContacts) 
			break;

		c[index].ApplyLinearPositionChange(linearChange, angularChange, max);

		for (i = 0; i < numContacts; i++)
		{
			for (unsigned b = 0; b < 2; b++) if (c[i].body[b])
			{
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i].body[b] == c[index].body[d])
					{
						deltaPosition = linearChange[d] +
							angularChange[d] * c[i].mRelativeContactPosition[b];

						c[i].penetration += Dot(deltaPosition, (c[i].contactNormal)) * (b ? 1.0f : -1.0f);
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}

}