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
#include "Actor.h"

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

private:
	World(const World&) {}
	World& operator= (const World&) {}

	ActorId getNextActorId();

private:
	std::map<ActorId, Actor*> mActors;

	ActorId mCurrentActorId;
};

}

#endif