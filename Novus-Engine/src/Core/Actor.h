//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ACTOR_H
#define NOVUS_ACTOR_H

namespace novus
{
class ActorComponent;
class D3DRenderer;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void Destroy() = 0;

	bool addComponent(ActorComponent* component);
	//void addComponent(ComponentId id, ObjectComponent* component); //Do I want to let the id be precalculated?

	virtual void Init();

	virtual void Update(float dt);

	virtual void PreRender(D3DRenderer* renderer);
	virtual void Render(D3DRenderer* renderer);
	virtual void PostRender(D3DRenderer* renderer);

	template <class T>
	T* getComponent(ComponentId id);

	template <class T>
	T* getComponent(const char *name);

protected:
	void deleteComponents();

private:
	std::map<ComponentId, ActorComponent*> mComponents;
};
}

#endif