//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ACTOR_COMPONENT_H
#define NOVUS_ACTOR_COMPONENT_H

#include <vector>
#include "Math/Transform.h"
#include "Object.h"

namespace novus
{
class Actor;
class D3DRenderer;

class ActorComponent : public Object
{
	friend class Actor;

public:
	ActorComponent();
	virtual ~ActorComponent();

	virtual void Init() {};

	virtual void Destroy();

	virtual void Update(float dt);
	virtual void PostUpdate(float dt); //Mostly for handling Destroy cleanup

	virtual void PreRender(D3DRenderer* renderer);
	virtual void Render(D3DRenderer* renderer);
	virtual void PostRender(D3DRenderer* renderer);

	void AddChildComponent(ActorComponent* component);

	Actor* getParentActor() const { return mpParentActor; }
	ActorComponent* getParentComponent() const { return mpParentComponent; }

	bool IsDestroyed() const { return mDestroyed; }

	Transform transform;//TODO: Cache this for to world and from world transforms in the hierarchy 

	void setDisplayName(const char* name);
	std::string getDisplayName() const { return mDisplayName; }

private:
	void UpdateChildComponents(float dt);
	void RenderChildComponents(D3DRenderer* renderer);

	void RemoveChildComponent(ActorComponent* component);

	void CleanupDestroyedComponents();

private:
	std::string mDisplayName;
	bool mDestroyed;

	Actor* mpParentActor;
	ActorComponent* mpParentComponent;

	std::vector<ActorComponent*> mChildComponents;
};
}

#endif