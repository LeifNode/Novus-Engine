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
	auto cendIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != cendIt; ++it)
	{
		it->second->Update(dt);
	}

	//Allow actors and components to clean up destroyed things 
	cendIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != cendIt; ++it)
	{
		it->second->PostUpdate(dt);
	}
	
	auto endIt = mActors.end();
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
	auto endIt = mRenderTargets.cend();

	//Depth pass
	for (auto it = mRenderTargets.cbegin(); it != endIt; ++it)
	{
		IRenderTarget* renderTarget = (*it);

		if (renderTarget->GetRenderPass() == RenderPass::Depth)
		{
			RenderToRenderTarget(renderer, renderTarget);
		}
	}


}

void World::AddActor(Actor* actor)
{
	auto findIt = mActors.find(actor->getId());
	if (findIt == mActors.end())
	{
		actor->mId = GetNextActorId();
		mActors[actor->getId()] = actor;
	}
}

ActorId World::GetNextActorId()
{
	return ++mCurrentActorId;
}


void World::RenderScenePass(D3DRenderer* renderer, RenderPass::Type pass)
{
	auto endIt = mRenderTargets.cend();
	for (auto it = mRenderTargets.cbegin(); it != endIt; ++it)
	{
		if ((*it)->GetRenderPass() == pass)
		{
			RenderToRenderTarget(renderer, *it);
		}
	}
}

void World::RegisterRenderTarget(IRenderTarget* renderTarget)
{
	mRenderTargets.insert(renderTarget);
}

void World::UnregisterRenderTarget(IRenderTarget* renderTarget)
{
	mRenderTargets.erase(renderTarget);
}

void World::RenderToRenderTarget(D3DRenderer* renderer, IRenderTarget* renderTarget)
{
	renderTarget->BindTarget(renderer);

	auto endIt = mActors.cend();
	for (auto it = mActors.cbegin(); it != endIt; ++it)
	{
		if (renderTarget->ShouldRenderActor(it->second))
		{
			it->second->PreRender(renderer);
			it->second->Render(renderer);
			it->second->PostRender(renderer);
		}
	}

	renderTarget->UnbindTarget(renderer);
}

}