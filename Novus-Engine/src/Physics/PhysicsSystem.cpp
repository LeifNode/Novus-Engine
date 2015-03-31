#include "PhysicsSystem.h"
#include "Application/Common.h"
#include "Graphics/ConstraintRenderer.h"

namespace novus
{

PhysicsSystem::PhysicsSystem()
: constraintRenderer(0.5f, Vector4(1.0f, 0.0f, 0.0f, 1.0f))
{
	mMaxContacts = 8000;
	mpContactArr = NE_NEW ParticleContact[mMaxContacts]();
}

PhysicsSystem::~PhysicsSystem()
{
	Clear();

	NE_DELETEARR(mpContactArr);
}

void PhysicsSystem::Init()
{
	
}

void PhysicsSystem::Clear()
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

	mContactGenerators.clear();

	mForceRegistry.Clear();
}

void PhysicsSystem::Update(float dt)
{
	mForceRegistry.Update(dt);

	unsigned int contactCount = GenerateContacts();

	if (contactCount > 0)
	{
		mResolver.ResolveContacts(mpContactArr, contactCount, contactCount, dt);
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

		if (limit <= 0)
		{
			NE_WARN("Contact limit exceeded.", "PhysicsSystem");
			break;
		}
	}

	return mMaxContacts - limit;
}

const std::vector<Particle*>* PhysicsSystem::getParticles() const
{
	return &mParticles;
}

const std::vector<ParticleContactGenerator*>* PhysicsSystem::getContactGenerators() const
{
	return &mContactGenerators;
}

const std::list<ParticleForceGenerator*>* PhysicsSystem::getForceGenerators() const
{
	return &mForceGenerators;
}

const ParticleForceRegistry* PhysicsSystem::getForceRegistry() const
{
	return &mForceRegistry;
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

void PhysicsSystem::AddContactGenerator(ParticleContactGenerator* generator)
{
	mContactGenerators.push_back(generator);
}

}