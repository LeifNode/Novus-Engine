#include "World.h"
#include "Application/Common.h"
#include "Actor.h"

namespace novus
{

World::World()
	: mCurrentActorId(0)
{
}

World::~World()
{
	auto endIt = mActors.end();
	for (auto it = mActors.begin(); it != endIt; ++it)
	{
		NE_DELETE(it->second);
	}
}

void World::Update(float dt)
{
	auto endIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != endIt; ++it)
	{
		it->second->Update(dt);
	}

	//Allow actors and components to clean up destroyed things 
	endIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != endIt; ++it)
	{
		it->second->PostUpdate(dt);
	}
	
	endIt = mActors.end();
	for (auto it = mActors.begin(); it != endIt; )
	{
		if (it->second->IsDestroyed())
		{
			NE_DELETE(it->second);
			mActors.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void World::Render(D3DRenderer* renderer)
{
	auto endIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != endIt; ++it)
	{
		it->second->PreRender(renderer);
		it->second->Render(renderer);
		it->second->PostRender(renderer);
	}
}

void World::AddActor(Actor* actor)
{
	auto findIt = mActors.find(actor->getId());
	if (findIt == mActors.end())
	{
		actor->mId = getNextActorId();
		mActors[actor->getId()] = actor;
	}
}

ActorId World::getNextActorId()
{
	return ++mCurrentActorId;
}

}