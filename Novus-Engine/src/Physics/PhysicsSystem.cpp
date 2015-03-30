#include "PhysicsSystem.h"
#include "Application/Common.h"
#include "Graphics/ConstraintRenderer.h"

namespace novus
{

PhysicsSystem::PhysicsSystem()
: constraintRenderer(0.5f, Vector4(1.0f, 0.0f, 0.0f, 1.0f))
{
	mMaxContacts = 1000;
	mpContactArr = NE_NEW ParticleContact[mMaxContacts]();
}

PhysicsSystem::~PhysicsSystem()
{
	for (auto it = mForceGenerators.begin(); it != mForceGenerators.end(); ++it)
	{
		NE_DELETE(*it);
	}

	mForceGenerators.clear();

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		NE_DELETE(*it);
	}

	mParticles.clear();

	for (auto it = mContactGenerators.begin(); it != mContactGenerators.end(); ++it)
	{
		NE_DELETE(*it);
	}

	NE_DELETEARR(mpContactArr);
}

void PhysicsSystem::Init()
{
	ParticleGravity* gravityForce = NE_NEW ParticleGravity(Vector3(0.0f, -9.8f, 0.0f));

	mForceGenerators.push_back(gravityForce);

	ParticlePlane* planeCollider = NE_NEW ParticlePlane(Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f), 0.2f);
	planeCollider->pParticles = &mParticles; 

	planeCollider->restitution = 0.9f;

	mContactGenerators.push_back(planeCollider);

	for (int i = 0; i < 50; i++)
	{
		Particle* newParticle = NE_NEW Particle();
		newParticle->setMass(10.2f);
		newParticle->setDamping(0.8f);
		newParticle->setPosition(Vector3(Math::RandF(-10.0f, 10.0f), Math::RandF(1.0f, 2.0f), Math::RandF(-10.0f, 10.0f)));

		mForceRegistry.Add(newParticle, gravityForce);

		mParticles.push_back(newParticle);
	}
}

void PhysicsSystem::Update(float dt)
{
	mForceRegistry.Update(dt);

	unsigned int contactCount = GenerateContacts();

	if (contactCount > 0)
	{
		mResolver.ResolveContacts(mpContactArr, contactCount, 2 * contactCount, dt);
	}

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

unsigned int PhysicsSystem::GenerateContacts()
{
	unsigned int limit = mMaxContacts;
	ParticleContact *nextContact = mpContactArr;

	for (auto it = mContactGenerators.cbegin(); it != mContactGenerators.cend(); it++)
	{
		unsigned int used = (*it)->AddContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		if (limit <= 0) break;
	}

	return mMaxContacts - limit;
}

const std::vector<Particle*>& PhysicsSystem::getParticles() const
{
	return mParticles;
}

void PhysicsSystem::AddParticle(Particle* particle)
{
	mParticles.push_back(particle);
}

void PhysicsSystem::AddForceGenerator(ParticleForceGenerator* generator)
{
	mForceGenerators.push_back(generator);
}

void PhysicsSystem::AddRegistryEntry(Particle* particle, ParticleForceGenerator* generator)
{
	mForceRegistry.Add(particle, generator);
}

}