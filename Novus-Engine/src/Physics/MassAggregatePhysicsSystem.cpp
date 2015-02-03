#include "MassAggregatePhysicsSystem.h"
#include "Application/Common.h"

namespace novus
{

MassAggregatePhysicsSystem::MassAggregatePhysicsSystem()
{
}

MassAggregatePhysicsSystem::~MassAggregatePhysicsSystem()
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

void MassAggregatePhysicsSystem::Init()
{
	//mForceGenerators.push_back(new ParticlePlanetaryGravitation(this));
}

void MassAggregatePhysicsSystem::Update(float dt)
{
	mForceRegistry.Update(dt);

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

const std::vector<Particle*>& MassAggregatePhysicsSystem::getParticles() const
{
	return mParticles;
}

void MassAggregatePhysicsSystem::AddParticle(Particle* particle)
{
	mParticles.push_back(particle);
}

void MassAggregatePhysicsSystem::AddForceGenerator(ParticleForceGenerator* generator)
{
	mForceGenerators.push_back(generator);
}

void MassAggregatePhysicsSystem::AddRegistryEntry(Particle* particle, ParticleForceGenerator* generator)
{
	mForceRegistry.Add(particle, generator);
}

}