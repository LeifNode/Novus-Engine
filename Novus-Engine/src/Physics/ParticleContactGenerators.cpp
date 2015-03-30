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

	contact->restitution = 0;

	return 1;
}

ParticlePlane::ParticlePlane(const Vector3& point, const Vector3& normal, float particleRadius)
	: point(point),
	normal(normal),
	particleRadius(particleRadius),
	restitution(0.5f)
{
}

unsigned ParticlePlane::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned count = 0;
	auto endIt = pParticles->cend();
	for (auto p = pParticles->cbegin(); p != endIt; p++)
	{
		float d = Dot(normal, ((*p)->getPosition() - point)); //Distance to plane
		if (d < particleRadius)
		{
			contact->contactNormal = normal;
			contact->particle[0] = *p;
			contact->particle[1] = NULL;
			contact->penetration = d - particleRadius;
			contact->restitution = restitution;
			contact++;
			count++;
		}

		if (count >= limit) return count;
	}

	return count;
}

}