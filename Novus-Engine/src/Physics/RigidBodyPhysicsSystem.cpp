#include "RigidBodyPhysicsSystem.h"
#include "Application/Common.h"

namespace novus
{

RigidBodyPhysicsSystem::RigidBodyPhysicsSystem(unsigned int maxContacts, unsigned int iterations)
:mResolver(iterations),
mMaxContacts(maxContacts)
{
	mpContactArr = NE_NEW Contact[maxContacts];
	mCalculateIterations = (iterations == 0);
}

RigidBodyPhysicsSystem::~RigidBodyPhysicsSystem()
{
	NE_DELETEARR(mpContactArr);
}

void RigidBodyPhysicsSystem::StartFrame()
{
	for (auto it = mRigidBodies.cbegin(); it != mRigidBodies.cend(); ++it)
	{
		(*it)->ClearAccumilators();
		(*it)->CalculateDerivedData();
	}
}

void RigidBodyPhysicsSystem::Update(float dt)
{
	for (auto it = mRigidBodies.cbegin(); it != mRigidBodies.cend(); ++it)
	{
		(*it)->Integrate(dt);
	}


}

void RigidBodyPhysicsSystem::GenerateContacts()
{
	unsigned int limit = mMaxContacts;
	Contact* nextContact = mpContactArr;
}

}