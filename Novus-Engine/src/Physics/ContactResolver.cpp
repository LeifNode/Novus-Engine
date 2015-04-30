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

void ContactResolver::AdjustVelocities(Contact* contactArr, unsigned int numContacts, float dt)
{
	Vector3 velocityChange[2], rotationChange[2];
	Vector3 deltaVal;

	velocityIterationsUsed = 0;
	while (velocityIterationsUsed < mVelocityIterations)
	{
		float max = mVelocityEpsilon;
		unsigned int index = numContacts;

		for (unsigned int i = 0; i < numContacts; i++)
		{
			if (contactArr[i].mDesiredDeltaVelocity > max)
			{
				max = contactArr[i].mDesiredDeltaVelocity;
				index = i;
			}
		}

		if (index == numContacts) 
			break;

		//Match awake here

		contactArr[index].ApplyVelocityChange(velocityChange, rotationChange);

		//Changing the velocity of the two bodies makes some of the other reletive velocities inaccurate
		for (unsigned int i = 0; i < numContacts; i++)
		{
			for (unsigned int b = 0; b < 2; b++) if (contactArr[i].body[b] != NULL)
			{
				for (unsigned int d = 0; d < 2; d++)
				{
					if (contactArr[i].body[b] == contactArr[index].body[d])
					{
						deltaVal = velocityChange[d] + (rotationChange[d] * contactArr[i].mRelativeContactPosition[b]);

						contactArr[i].mContactVelocity += (contactArr[i].mWorldToContact * deltaVal) * (b ? -1.0f : 1.0f);
						contactArr[i].CalculateDesiredDeltaVelocity(dt);
					}
				}
			}
		}

		velocityIterationsUsed++;
	}
}

void ContactResolver::AdjustPositions(Contact* contactArr, unsigned int numContacts, float dt)
{
	unsigned int i, index;
	Vector3 linearChange[2], angularChange[2];
	float max;
	Vector3 deltaPosition;

	positionIterationsUsed = 0;

	while (positionIterationsUsed < mPositionIterations)
	{
		max = mPositionEpsilon;
		index = numContacts;
		for (i = 0; i < numContacts; i++)
		{
			if (contactArr[i].penetration > max)
			{
				max = contactArr[i].penetration;
				index = i;
			}
		}

		if (index == numContacts)
			break;

		//contactArr[index].ApplyPositionChange(linearChange, angularChange, max);
		contactArr[index].ApplyLinearPositionChange(linearChange, angularChange, max);

		for (i = 0; i < numContacts; i++)
		{
			for (unsigned int b = 0; b < 2; b++) if (contactArr[i].body[b] != NULL)
			{
				for (unsigned int d = 0; d < 2; d++)
				{
					if (contactArr[i].body[b] == contactArr[index].body[d])
					{
						deltaPosition = linearChange[d] + (angularChange[d] * contactArr[i].mRelativeContactPosition[b]);

						contactArr[i].penetration += Dot(deltaPosition, contactArr[i].contactNormal) * (b ? 1.0f : -1.0f);
					}
				}
			}
		}

		positionIterationsUsed++;
	}
}

}