#include "ParticleContacts.h"
#include "Application/Common.h"


namespace novus
{

ParticleContact::ParticleContact()
{
	particle[0] = NULL;
	particle[1] = NULL;
}

void ParticleContact::Resolve(float dt)
{
	ResolveVelocity(dt);
	ResolveInterpenetration(dt);
}

float ParticleContact::CalculateSeparatingVelocity() const
{
	Vector3 relativeVelocity = particle[0]->getVelocity();
	if (particle[1])
		relativeVelocity -= particle[1]->getVelocity();

	return Dot(relativeVelocity, contactNormal);
}

void ParticleContact::ResolveVelocity(float dt)
{
	float separatingVelocity = CalculateSeparatingVelocity();

	if (separatingVelocity > 0)
	{
		return;
	}

	float newSepVelocity = -separatingVelocity * restitution;

	Vector3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1])
		accCausedVelocity -= particle[1]->getAcceleration();

	float accCausedSepVelocity = Dot(accCausedVelocity, contactNormal) * dt;

	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		if (newSepVelocity < 0)
			newSepVelocity = 0;
	}

	float deltaVelocity = newSepVelocity - separatingVelocity;

	float totalInverseMass = particle[0]->getInverseMass();
	if (particle[1])
		totalInverseMass += particle[1]->getInverseMass();

	if (totalInverseMass <= 0)
		return;

	float impulse = deltaVelocity / totalInverseMass;

	Vector3 inpulsePerIMass = contactNormal * impulse;

	//if (XMVectorGetX(XMVectorIsNaN(particle[0]->getPosition().getXMVector())) != 0)
	//{
	//	std::cout << "Is NaN\n";
	//}

	particle[0]->setVelocity(particle[0]->getVelocity() + inpulsePerIMass * particle[0]->getInverseMass());
	if (particle[1])
		particle[1]->setVelocity(particle[1]->getVelocity() + inpulsePerIMass * -particle[1]->getInverseMass());
}

void ParticleContact::ResolveInterpenetration(float dt)
{
	if (penetration <= 0) return;

	float totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

	if (totalInverseMass <= 0) return;

	Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	particleMovement[0] = movePerIMass * particle[0]->getInverseMass();

	if (particle[1])
	{
		particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
	}
	else
	{
		particleMovement[1] = Vector3();
	}

	particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);

	if (particle[1])
	{
		particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
	}
}

ParticleContactResolver::ParticleContactResolver()
	: iterationsUsed(0)
{}

void ParticleContactResolver::ResolveContacts(ParticleContact* contactArr, unsigned numContacts, unsigned int maxIterations, float dt)
{
	unsigned i;

	iterationsUsed = 0;

	while (iterationsUsed < maxIterations)
	{
		float max = FLT_MAX;
		unsigned maxIndex = numContacts;

		for (i = 0; i < numContacts; i++)
		{
			float sepVel = contactArr[i].CalculateSeparatingVelocity();
			if (sepVel < max && (sepVel < 0 || contactArr[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if (maxIndex == numContacts)
			break;

		contactArr[maxIndex].Resolve(dt);

		iterationsUsed++;
	}
}

}