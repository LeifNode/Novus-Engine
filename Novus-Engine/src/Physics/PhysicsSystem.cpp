#include "PhysicsSystem.h"
#include "Application/Common.h"

namespace novus
{

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
	for (auto it = mForceGenerators.begin(); it != mForceGenerators.end(); ++it)
	{
		NE_DELETE((*it));
	}

	mForceGenerators.clear();

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		NE_DELETE((*it));
	}

	mParticles.clear();
}

void PhysicsSystem::Init()
{
	//mForceGenerators.push_back(new ParticlePlanetaryGravitation(this));
}

void PhysicsSystem::Update(float dt)
{
	mForceRegistry.Update(dt);

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->Update(dt);
	}
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