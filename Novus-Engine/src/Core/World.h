//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_WORLD_H
#define NOVUS_WORLD_H

#include <vector>
#include <unordered_set>
#include "Actor.h"
#include "Graphics/RenderTargets/IRenderTarget.h"

namespace novus
{

class D3DRenderer;
class Actor;

class World
{
	friend class Actor;

public:
	World();
	~World();

	void Update(float dt);

	void Render(D3DRenderer* renderer);

	void AddActor(Actor* actor);

	void RenderScenePass(D3DRenderer* renderer, RenderPass::Type pass);

	void RegisterRenderTarget(IRenderTarget* renderTarget);
	void UnregisterRenderTarget(IRenderTarget* renderTarget);

private:
	World(const World&) {}
	World& operator= (const World&) {}

	ActorId GetNextActorId();

	void RenderToRenderTarget(D3DRenderer* renderer, IRenderTarget* renderTarget);

private:
	std::map<ActorId, Actor*> mActors;

	ActorId mCurrentActorId;

	std::unordered_set<IRenderTarget*> mRenderTargets;//This should probably be a multimap based on the pass type
};

}

#endif