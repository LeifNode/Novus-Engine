#include "Contact.h"
#include "RigidBody.h"

namespace novus
{

void Contact::setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution)
{
	body[0] = one;
	body[1] = two;
	this->restitution = restitution;
	this->friction = friction;
}

void Contact::CalculateDesiredDeltaVelocity(float dt)
{
	const static float velocityLimit = 0.25f;

	float velocityFromAcc = 0.0f;

	velocityFromAcc += Dot(body[0]->getLastFrameAcceleration() * dt, contactNormal);

	if (body[1] != NULL)
	{
		velocityFromAcc -= Dot(body[1]->getLastFrameAcceleration() * dt, contactNormal);
	}

	float thisRestitution = restitution;
	if (fabsf(mContactVelocity.x) < velocityLimit)
		thisRestitution = 0.0f;

	mDesiredDeltaVelocity = -mContactVelocity.x - thisRestitution * (mContactVelocity.x - velocityFromAcc);
}

Vector3 Contact::CalculateLocalVelocity(unsigned int bodyIndex, float dt)
{
	RigidBody* thisBody = body[bodyIndex];

	//Get velocity of the contact point
	Vector3 velocity = Cross(thisBody->getAngularVelocity(), mRelativeContactPosition[bodyIndex]);
	velocity += thisBody->getVelocity();

	//Transform the velocity into the contact coordinate space
	Vector3 contactVelocity = mWorldToContact * velocity;

	//Calculate velocity due to passive forces
	Vector3 accVelocity = thisBody->getLastFrameAcceleration() * dt;

	accVelocity = mWorldToContact * accVelocity;

	//Ignore acceleration in the direction of the contact normal
	accVelocity.x = 0.0f;

	contactVelocity += accVelocity;

	return contactVelocity;
}

void Contact::CalculateContactBasis()
{
	Vector3 contactTangent[2];

	//Check if the Z axis is closer to the X or Y axis
	if (fabsf(contactNormal.x) > fabsf(contactNormal.y))
	{
		const float scale = 1.0f / sqrtf(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

		contactTangent[0].x = contactNormal.z * scale;
		contactTangent[0].y = 0.0f;
		contactTangent[0].z = -contactNormal.x * scale;

		contactTangent[1].x = contactNormal.y * contactTangent[0].x;
		contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
	}
	else
	{
		const float scale = 1.0f / sqrtf(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

		contactTangent[0].x = 0.0f;
		contactTangent[0].y = -contactNormal.z * scale;
		contactTangent[0].z = contactNormal.y * scale;

		contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = contactNormal.x * contactTangent[0].y;
	}

	/*contactTangent[0] = Normalize(Cross(contactNormal, (contactNormal.y < 0.98f) ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f)));
	contactTangent[1] = Cross(contactNormal, contactTangent[0]);*/

	mContactToWorld = Matrix3(contactNormal.x, contactTangent[0].x, contactTangent[1].x,
							  contactNormal.y, contactTangent[0].y, contactTangent[1].y,
							  contactNormal.z, contactTangent[0].z, contactTangent[1].z);

	//mContactToWorld = Matrix3(contactNormal, contactTangent[0], contactTangent[1]);

	mWorldToContact = Matrix3::Transpose(mContactToWorld);
}

void Contact::CalculateInternals(float dt)
{
	if (!body[0])
		SwapBodies();

	CalculateContactBasis();

	mRelativeContactPosition[0] = contactPoint - body[0]->getTransform()->GetPosition();
	if (body[1] != NULL)
	{
		mRelativeContactPosition[1] = contactPoint - body[1]->getTransform()->GetPosition();
	}

	mContactVelocity = CalculateLocalVelocity(0, dt);

	if (body[1] != NULL)
	{
		mContactVelocity -= CalculateLocalVelocity(1, dt);
	}

	CalculateDesiredDeltaVelocity(dt);
}

void Contact::SwapBodies()
{
	contactNormal *= -1.0f;

	RigidBody* temp = body[0];
	body[0] = body[1];
	body[1] = temp;
}

void Contact::ApplyVelocityChange(Vector3 velocityChange[2], Vector3 rotationChange[2])
{
	Matrix3 inverseInertiaTensor[2];
	inverseInertiaTensor[0] = body[0]->getInverseInertiaTensorWorld();

	if (body[1] != NULL)
	{
		inverseInertiaTensor[1] = body[1]->getInverseInertiaTensorWorld();
	}

	Vector3 impulseContact;

	impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensor);

	//Transform impulse to world coordinates
	Vector3 impulse = mContactToWorld * impulseContact;

	//Split the impulse into linear and rotational components
	Vector3 impulsiveTorque = Cross(mRelativeContactPosition[0], impulse);
	rotationChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
	velocityChange[0] = Vector3();
	velocityChange[0] += impulse * body[0]->getInverseMass();

	body[0]->AddVelocity(velocityChange[0]);
	body[0]->AddAngularVelocity(rotationChange[0]);

	if (body[1] != NULL)
	{
		Vector3 impulsiveTorque = Cross(impulse, mRelativeContactPosition[1]);
		rotationChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
		velocityChange[1] = Vector3();
		velocityChange[1] += impulse * -body[1]->getInverseMass();

		body[1]->AddVelocity(velocityChange[1]);
		body[1]->AddAngularVelocity(rotationChange[1]);
	}
}

void Contact::ApplyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], float penetration)
{
	const float angularLimit = 0.2f;

	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0.0f;
	float linearInertia[2];
	float angularInertia[2];

	for (unsigned int i = 0; i < 2; i++) if (body[i] != NULL)
	{
		Matrix3 inverseInertiaTensor = body[i]->getInverseInertiaTensorWorld();

		Vector3 angularInertiaWorld = Cross(mRelativeContactPosition[i], contactNormal);
		angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
		angularInertiaWorld = Cross(angularInertiaWorld, mRelativeContactPosition[i]);
		angularInertia[i] = Dot(angularInertiaWorld, contactNormal);

		linearInertia[i] = body[i]->getInverseMass();

		totalInertia += linearInertia[i] + angularInertia[i];
	}

	for (unsigned int i = 0; i < 2; i++) if (body[i] != NULL)
	{
		float sign = (i == 0) ? 1.0f : -1.0f;
		
		angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
		linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

		//Avoid overrotation on massive objects by limiting the angular move
		Vector3 projection = mRelativeContactPosition[i];
		projection += contactNormal * -Dot(mRelativeContactPosition[i], contactNormal);

		float maxMagnitude = angularLimit * Length(projection);

		if (angularMove[i] < maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if (angularMove[i] > maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}

		//Calculating the required rotation to apply to the rigidbody to match with the adjusted linear movement
		if (angularMove[i] == 0.0f)
		{
			angularChange[i] = Vector3();//No angular movement so no rotation
		}
		else
		{
			//Get the direction the body needs to rotate in
			Vector3 targetAngularDirection = mRelativeContactPosition[i] * contactNormal;

			Matrix3 inverseIntertiaTensor = body[i]->getInverseInertiaTensorWorld();

			//Get the direction it needs to rotate
			angularChange[i] = (inverseIntertiaTensor * targetAngularDirection) * (angularMove[i] / angularInertia[i]);
		}

		linearChange[i] = contactNormal * linearMove[i];

		//Apply the linear movement
		Vector3 position = body[i]->getTransform()->GetPosition();
		position += contactNormal * linearMove[i];
		body[i]->getTransform()->SetPosition(position);

		Quaternion q = body[i]->getTransform()->GetRotation();
		q += angularChange[i];
		body[i]->getTransform()->SetRotation(q);

		//Resolve derived data on sleeping bodies here
	}
}

Vector3 Contact::CalculateFrictionlessImpulse(Matrix3* inverIntertiaTensor)
{
	Vector3 impulseContact;

	//Build a vector that shows the change in velocity for a unit impulse in the direcion of the contact normal
	Vector3 deltaVelWorld = Cross(mRelativeContactPosition[0], contactNormal);
	deltaVelWorld = inverIntertiaTensor[0] * deltaVelWorld;
	deltaVelWorld = Cross(deltaVelWorld, mRelativeContactPosition[0]);

	//Calculate the change in velocity in contact coordinates
	float deltaVelocity = Dot(deltaVelWorld, contactNormal);

	deltaVelocity += body[0]->getInverseMass();

	if (body[1] != NULL)
	{
		Vector3 deltaVelWorld = Cross(mRelativeContactPosition[1], contactNormal);
		deltaVelWorld = inverIntertiaTensor[1] * deltaVelWorld;
		deltaVelWorld = Cross(deltaVelWorld, mRelativeContactPosition[1]);

		deltaVelocity += Dot(deltaVelWorld, contactNormal);

		deltaVelocity += body[1]->getInverseMass();
	}

	impulseContact.x = mDesiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0.0f;
	impulseContact.z = 0.0f;

	return impulseContact;
}

}