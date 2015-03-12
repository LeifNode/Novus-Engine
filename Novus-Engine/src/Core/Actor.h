//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ACTOR_H
#define NOVUS_ACTOR_H

#include <map>
#include <vector>

typedef unsigned ComponentId;

namespace novus
{
class ActorComponent;
class D3DRenderer;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	virtual void PreDestroy() {};
	virtual void Destroy() {};
	virtual void PostDestroy() {};

	bool addComponent(ActorComponent* component);
	//void addComponent(ComponentId id, ObjectComponent* component); //Do I want to let the id be precalculated?

	virtual void Init() {};

	virtual void Update(float dt) {};

	virtual void PreRender(D3DRenderer* renderer) {};
	virtual void Render(D3DRenderer* renderer) {};
	virtual void PostRender(D3DRenderer* renderer) {};

	void AddComponent(ActorComponent* component);

	template <class ComponentT>
	ComponentT* getComponent(unsigned index = 0);

	template <class ComponentT>
	std::vector<ComponentT*>& getComponents();

	/*template <class T>
	T* getComponent(const char *name);*/

protected:
	void deleteComponents();
	
	void renderComponents();
	void renderChildren();

	void deleteChildren();

private:
	std::multimap<ComponentId, ActorComponent*> mComponents;
};
}

#endif