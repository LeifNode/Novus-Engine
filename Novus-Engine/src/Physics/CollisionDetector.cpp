#include "CollisionDetector.h"
#include "RigidBody.h"

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
	contact->contactPoint = positionOne - midLine * 0.5f;
	contact->penetration = (one.radius + two.radius - length);
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
	if (data->contactsLeft <= 0)
		return 0;

	Vector3 center = sphere.getAxis(3);

	float dist = LengthSq(center - box.getAxis(3));

	float boundingSphere = Dot(box.halfSize * box.body->getTransform()->GetScale(), box.halfSize * box.body->getTransform()->GetScale());
	boundingSphere += sphere.radius * sphere.radius;

	if (dist > boundingSphere)
		return 0;
	
	Matrix4 transform = Quaternion::ToMatrix(box.body->getTransform()->GetRotation()) * Matrix4::Translate(box.body->getTransform()->GetPosition());
	Matrix4 invTransform = Matrix4::Inverse(transform);

	//float dist;
	Vector3 relCenter = Vector3(Vector4(center, 1.0f) * invTransform);

	Vector3 scaledHalfSize = box.halfSize * box.body->getTransform()->GetScale();
	//Vector3 scaledHalfSize = Vector3(1.0f);

	if (fabsf(relCenter.x) - sphere.radius > scaledHalfSize.x ||
		fabsf(relCenter.y) - sphere.radius > scaledHalfSize.y ||
		fabsf(relCenter.z) - sphere.radius > scaledHalfSize.z)
	{
		return 0;
	}

	Vector3 closestPt;

	dist = relCenter.x;
	if (dist > scaledHalfSize.x) dist = scaledHalfSize.x;
	if (dist < -scaledHalfSize.x) dist = -scaledHalfSize.x;
	closestPt.x = dist;

	dist = relCenter.y;
	if (dist > scaledHalfSize.y) dist = scaledHalfSize.y;
	if (dist < -scaledHalfSize.y) dist = -scaledHalfSize.y;
	closestPt.y = dist;

	dist = relCenter.z;
	if (dist > scaledHalfSize.z) dist = scaledHalfSize.z;
	if (dist < -scaledHalfSize.z) dist = -scaledHalfSize.z;
	closestPt.z = dist;

	dist = LengthSq(closestPt - relCenter);
	if (dist > sphere.radius * sphere.radius) 
		return 0;

	Vector3 closestPtWorld = Vector3(Vector4(closestPt, 1.0f) * transform);

	Contact* contact = data->contacts;
	contact->contactNormal = Normalize(closestPtWorld - center);
	contact->contactPoint = closestPtWorld;
	contact->penetration = (sphere.radius - sqrtf(dist));
	//contact->setBodyData(box.body, sphere.body, data->friction, data->restitution);
	contact->setBodyData(NULL, sphere.body, data->friction, data->restitution);

	data->AddContacts(1);
	return 1;
}

}