#include "CollisionPrimitives.h"
#include "RigidBody.h"


namespace novus
{

void CollisionPrimitive::CalculateInternals()
{
	mTransform = body->getTransform()->GetTransform() * offset;
}

}