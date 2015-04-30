#include "CollisionDetector.h"

namespace novus
{

unsigned int CollisionDetector::SphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
{
	if (data->contactsLeft <= 0)
		return 0;

	Vector3 position = sphere.getAxis(3);

	float ballDistance = Dot(plane.direction, position) - sphere.radius - plane.offset;

	if (ballDistance >= 0.0f)
		return 0;

	Contact* contact = data->contacts;
	contact->contactNormal = plane.direction;
	contact->penetration = -ballDistance;
	contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
	contact->setBodyData(sphere.body, NULL, data->friction, data->restitution);

	data->AddContacts(1);
	return 1;
}

unsigned int CollisionDetector::SphereAndSphere(const CollisionSphere& one, const CollisionSphere& two, CollisionData* data)
{
	if (data->contactsLeft <= 0)
		return 0;

	Vector3 positionOne = one.getAxis(3);
	Vector3 positionTwo = two.getAxis(3);

	Vector3 midLine = positionOne - positionTwo;
	float length = Length(midLine);

	if (length <= 0.0f || length >= one.radius + two.radius)
	{
		return 0;
	}

	Vector3 normal = midLine / length;

	Contact* contact = data->contacts;

	contact->contactNormal = normal;
	contact->contactPoint = positionOne + midLine * 0.5f;
	contact->penetration = one.radius + two.radius - length;
	contact->setBodyData(one.body, two.body, data->friction, data->restitution);

	data->AddContacts(1);
	return 1;
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

		//float vertexDistance = Dot(plane.direction, (vertexPos - plane.origin));
		float vertexDistance = Dot(vertexPos, plane.direction);

		if (vertexDistance <= 0.0f)
		{
			/*contact->contactPoint = plane.direction;
			contact->contactPoint *= -vertexDistance;
			contact->contactPoint += vertexPos;
			contact->contactNormal = plane.direction;
			contact->penetration = -vertexDistance;*/

			contact->contactPoint = plane.direction;
			contact->contactPoint *= (vertexDistance - plane.offset);
			contact->contactPoint += vertexPos;
			contact->contactNormal = plane.direction;
			contact->penetration = plane.offset - vertexDistance;

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