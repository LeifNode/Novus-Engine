#include "ParticleContactGenerators.h"

namespace novus
{

float ParticleLink::currentLength() const
{
	Vector3 relativePos = particle[0]->getPosition() - particle[1]->getPosition();

	return Length(relativePos);
}

unsigned ParticleCable::AddContact(ParticleContact* contact, unsigned limit) const
{
	float length = currentLength();

	if (length < maxLength)
	{
		return 0;
	}

	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	Vector3 normal = particle[1]->getPosition() - particle[0]->getPosition();
	normal = Normalize(normal);

	contact->contactNormal = normal;

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned ParticleRod::AddContact(ParticleContact* contact, unsigned limit) const
{
	float currentLen = currentLength();

	if (currentLen == length)
		return 0;

	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	Vector3 normal = particle[1]->getPosition() - particle[0]->getPosition();
	normal = Normalize(normal);

	if (currentLen > length)
	{
		contact->contactNormal = normal;
		contact->penetration = currentLen - length;
	}
	else
	{
		contact->contactNormal = -normal;
		contact->penetration = length - currentLen;
	}

	contact->restitution = 0.0f;

	return 1;
}

ParticlePlane::ParticlePlane(const Vector3& point, const Vector3& normal, float particleRadius)
	: point(point),
	normal(normal),
	particleRadius(particleRadius),
	restitution(1.0f)
{
}

unsigned ParticlePlane::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned count = 0;
	auto endIt = pParticles->cend();
	for (auto it = pParticles->cbegin(); it != endIt; ++it)
	{
		float d = Dot(normal, ((*it)->getPosition() - point)); //Distance to plane
		if (d < particleRadius)
		{
			contact->contactNormal = normal;
			contact->particle[0] = *it;
			contact->particle[1] = NULL;
			contact->penetration = particleRadius - d;
			contact->restitution = restitution;
			contact++;
			count++;
		}

		if (count >= limit) return count;
	}

	return count;
}

ParticleSelfCollision::ParticleSelfCollision(float particleRadius)
: 
particleRadius(particleRadius),
restitution(0.5f)
{
}

unsigned ParticleSelfCollision::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned count = 0;
	float particleRadiusSq = particleRadius * particleRadius;

	//Should probably have a way to mark particles as having collided with this
	//At the moment two contacts will be generated for each collision though one of them should always get culled by the contact resolution
	auto endIt = pParticles->cend();
	for (auto p1 = pParticles->cbegin(); p1 != endIt; p1++)
	{
		for (auto p2 = pParticles->cbegin(); p2 != endIt; ++p2)
		{
			if (*p1 != *p2)//Don't collide with self
			{
				Vector3 normal = (*p1)->getPosition() - (*p2)->getPosition();
				float dSq = LengthSq(normal);

				if (dSq < particleRadiusSq * 4.0f)
				{
					float d = sqrtf(dSq);

					contact->contactNormal = normal / d;
					contact->particle[0] = *p1;
					contact->particle[1] = *p2;
					contact->penetration = particleRadius * 2.0f - d;
					contact->restitution = restitution;
					contact++;
					count++;
				}

				if (count >= limit) return count;
			}
		}
	}

	return count;
}

}