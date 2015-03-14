#include "ActorComponent.h"
#include "Application/Common.h"
#include "Graphics/D3DRenderer.h"
#include <algorithm>

namespace novus
{

ActorComponent::ActorComponent(const char* displayName, Actor* parentActor)
	: mDisplayName(displayName),
	mpParentActor(parentActor),
	mpParentComponent(NULL),
	mDestroyed(false)
{
}

ActorComponent::~ActorComponent()
{
	for (auto it = mChildComponents.begin(); it != mChildComponents.end(); ++it)
	{	
		(*it)->Destroy();
		NE_DELETE((*it));
	}
}

void ActorComponent::Destroy()
{
	if (!mDestroyed)
	{
		mDestroyed = true;

		for (auto it = mChildComponents.cbegin(); it != mChildComponents.cend(); ++it)
		{
			(*it)->Destroy();
		}
	}
}

void ActorComponent::Update(float dt)
{
	UpdateChildComponents(dt);
}

void ActorComponent::PostUpdate(float dt)
{
	CleanupDestroyedComponents();

	for (auto it = mChildComponents.cbegin(); it != mChildComponents.cend(); ++it)
	{
		(*it)->PostUpdate(dt);
	}
}

void ActorComponent::PreRender(D3DRenderer* renderer)
{
	renderer->PushTransform(transform);
}

void ActorComponent::Render(D3DRenderer* renderer)
{
	RenderChildComponents(renderer);
}

void ActorComponent::PostRender(D3DRenderer* renderer)
{
	renderer->PopTransform();
}

void ActorComponent::AddChildComponent(ActorComponent* component)
{
	assert(component != NULL && component->getParentComponent() == NULL);

	component->mpParentComponent = this;
	mChildComponents.push_back(component);
}

void ActorComponent::UpdateChildComponents(float dt)
{
	for (auto it = mChildComponents.cbegin(); it != mChildComponents.cend(); ++it)
	{
		(*it)->Update(dt);
	}
}

void ActorComponent::RenderChildComponents(D3DRenderer* renderer)
{
	for (auto it = mChildComponents.cbegin(); it != mChildComponents.cend(); ++it)
	{
		(*it)->PreRender(renderer);
		(*it)->Render(renderer);
		(*it)->PostRender(renderer);
	}
}

void ActorComponent::RemoveChildComponent(ActorComponent* component)
{
	for (auto it = mChildComponents.begin(); it != mChildComponents.end(); ++it)
	{
		if ((*it) == component)
		{
			(*it)->Destroy();
			NE_DELETE(*it);

			mChildComponents.erase(it);
			break;
		}
	}
}

void ActorComponent::CleanupDestroyedComponents()
{
	std::vector<ActorComponent*> removedComponents;

	for (auto it = mChildComponents.cbegin(); it != mChildComponents.cend(); ++it)
	{
		if ((*it)->mDestroyed)
			removedComponents.push_back(*it);
	}

	mChildComponents.erase(std::remove_if(mChildComponents.begin(), mChildComponents.end(),
		[](const ActorComponent* component) { return component->IsDestroyed(); }),
		mChildComponents.end());

	for (auto it = removedComponents.begin(); it != removedComponents.end(); ++it)
	{
		NE_DELETE(*it);
	}
}

}