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

}

void MassAggregatePhysicsSystem::Update(float dt)
{
	mForceRegistry.Update(dt);

	for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

}