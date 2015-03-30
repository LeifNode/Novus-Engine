#include "Actor.h"
#include "ActorComponent.h"
#include "Application/Common.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

Actor::Actor()
	: mDestroyed(false),
	mpParentActor(NULL),
	mId(0),
	mCastShadow(true)
{
}


Actor::~Actor()
{
	this->Destroy();
	//this->CleanupDestroyedActors();
	//this->CleanupDestroyedComponents();

	for (auto it = mChildActors.begin(); it != mChildActors.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mComponents.begin(); it != mComponents.end(); ++it)
	{
		NE_DELETE(it->second);
	}
}

void Actor::Destroy()
{
	if (!mDestroyed)
	{
		for (auto mapIt = mComponents.cbegin(); mapIt != mComponents.cend(); ++mapIt)
		{
			mapIt->second->Destroy();
		}

		for (auto it = mChildActors.cbegin(); it != mChildActors.cend(); ++it)
		{
			(*it)->Destroy();
		}

		mDestroyed = true;
	}
}

void Actor::Update(float dt)
{
	UpdateComponents(dt);
	UpdateChildren(dt);
}

void Actor::PostUpdate(float dt)
{
	CleanupDestroyedActors();
	CleanupDestroyedComponents();

	for (auto it = mComponents.cbegin(); it != mComponents.cend(); ++it)
	{
		it->second->PostUpdate(dt);
	}

	for (auto it = mChildActors.cbegin(); it != mChildActors.cend(); ++it)
	{
		(*it)->PostUpdate(dt);
	}
}

void Actor::PreRender(D3DRenderer* renderer)
{
	renderer->PushTransform(transform);
}

void Actor::Render(D3DRenderer* renderer)
{
	RenderComponents(renderer);
	RenderChildren(renderer);
}

void Actor::PostRender(D3DRenderer* renderer)
{
	renderer->PopTransform();
}

void Actor::AddChildActor(Actor* actor)
{
	assert(actor != NULL && actor->mpParentActor == NULL);

	actor->mpParentActor = this;
	mChildActors.push_back(actor);
}

void Actor::AddComponent(ActorComponent* component)
{
	if (component->getParentActor())
	{
		NE_ERROR("Cannot add component to actor when it is already attached.", "Actor");
		return;
	}

	mComponents.insert(std::make_pair(component->GetType(), component));
	component->mpParentActor = this;
}

void Actor::RemoveComponent(ActorComponent* component)
{

}

void Actor::setDisplayName(const std::string& name)
{
	mDisplayName = name;
}

std::string Actor::getDisplayName() const
{
	return mDisplayName;
}

void Actor::CleanupDestroyedActors()
{
	std::vector<Actor*> removedActors;

	for (auto it = mChildActors.cbegin(); it != mChildActors.cend(); ++it)
	{
		if ((*it)->IsDestroyed())
			removedActors.push_back(*it);
	}

	mChildActors.erase(std::remove_if(mChildActors.begin(), mChildActors.end(),
		[](const Actor* actor) { return actor->IsDestroyed(); }),
		mChildActors.end());

	for (auto it = removedActors.begin(); it != removedActors.end(); ++it)
	{
		NE_DELETE(*it);
	}
}

void Actor::CleanupDestroyedComponents()
{
	for (auto it = mComponents.begin(); it != mComponents.end();)
	{
		if (it->second->IsDestroyed())
		{
			NE_DELETE(it->second);
			mComponents.erase(it++);
		}
		else
			++it;
	}
}

void Actor::UpdateComponents(float dt)
{
	for (auto it = mComponents.cbegin(); it != mComponents.cend(); ++it)
	{
		it->second->Update(dt);
	}
}

void Actor::UpdateChildren(float dt)
{
	for (auto it = mChildActors.cbegin(); it != mChildActors.cend(); ++it)
	{
		(*it)->Update(dt);
	}
}

void Actor::RenderComponents(D3DRenderer* renderer)
{
	for (auto it = mComponents.cbegin(); it != mComponents.cend(); ++it)
	{
		it->second->PreRender(renderer);
		it->second->Render(renderer);
		it->second->PostRender(renderer);
	}
}

void Actor::RenderChildren(D3DRenderer* renderer)
{
	for (auto it = mChildActors.cbegin(); it != mChildActors.cend(); ++it)
	{
		(*it)->PreRender(renderer);
		(*it)->Render(renderer);
		(*it)->PostRender(renderer);
	}
}

}