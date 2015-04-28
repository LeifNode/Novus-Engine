#include "CollisionDetector.h"

namespace novus
{

unsigned int CollisionDetector::SphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
{
	return 0;
}

unsigned int CollisionDetector::SphereAndSphere(const CollisionSphere& one, const CollisionSphere& two, CollisionData* data)
{
	return 0;
}

unsigned int CollisionDetector::BoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data)
{
	if (data->contactsLeft <= 0)
		return 0;

	//Check intersection here for early out



	static float mults[8][3] = { { 1, 1, 1 }, { -1, 1, 1 }, { 1, -1, 1 }, { -1, -1, 1 },
	{ 1, 1, -1 }, { -1, 1, -1 }, { 1, -1, -1 }, { -1, -1, -1 } };

	Contact* contact = data->contacts;
	unsigned int contactsUsed = 0;

	for (unsigned int i = 0; i < 8; i++)
	{
		Vector3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
		vertexPos *= box.halfSize;
		vertexPos = Vector3(Vector4(vertexPos, 1.0f) * box.getTransform());

		float vertexDistance = Dot(plane.direction, (vertexPos - plane.origin));

		if (vertexDistance <= 0.0f)
		{
			contact->contactPoint = plane.direction;
			contact->contactPoint *= -vertexDistance;
			contact->contactPoint += vertexPos;
			contact->contactNormal = plane.direction;
			contact->penetration = -vertexDistance;

			contact->setBodyData(box.body, NULL, data->friction, data->restitution);

			contact++;
			contactsUsed++;

			if (contactsUsed == (unsigned int)data->contactsLeft)
				return contactsUsed;
		}
	}

	data->AddContacts(contactsUsed);
	return contactsUsed;
}

unsigned int CollisionDetector::BoxAndBox(const CollisionBox& one, const CollisionBox& two, CollisionData* data)
{
	return 0;
}

unsigned int CollisionDetector::BoxAndSphere(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data)
{
	return 0;
}

}