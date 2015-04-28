#include "RigidBody.h"

namespace novus
{

RigidBody::RigidBody()
: mInverseMass(1.0f),
mLinearDamping(0.99f),
mAngularDamping(0.99f)
{
	CalculateDerivedData();
}

void RigidBody::CalculateDerivedData()
{
	mInverseInteriaTensorWorld = Matrix3(mTransform.GetTransform()) * mInverseInteriaTensor;
}

void RigidBody::Integrate(float dt)
{
	mLastFrameAcceleration = Vector3();
	mLastFrameAcceleration += mForceAccum * mInverseMass;

	Vector3 angularAcceleration = mInverseInteriaTensorWorld * mTorqueAccum;

	mVelocity += mLastFrameAcceleration * dt;
	mAngularVelocity += angularAcceleration * dt;

	//Drag
	mVelocity *= powf(mLinearDamping, dt);
	mAngularVelocity *= powf(mAngularDamping, dt);

	mTransform.Translate(mVelocity * dt);

	mTransform.Rotate(mAngularVelocity * dt);

	CalculateDerivedData();

	ClearAccumilators();
}

void RigidBody::setMass(float mass)
{
	assert(mass != 0.0f);
	mInverseMass = 1.0f / mass;
}

float RigidBody::getMass() const
{
	if (mInverseMass == 0)
		return FLT_MAX;
	else
		return 1.0f / mInverseMass;
}

void RigidBody::setInverseMass(float inverseMass)
{
	mInverseMass = inverseMass;
}

float RigidBody::getInverseMass() const
{
	return mInverseMass;
}

bool RigidBody::hasFiniteMass() const
{
	return mInverseMass >= 0.0f;
}

void RigidBody::setInertiaTensor(const Matrix3& inertiaTensor)
{
	mInverseInteriaTensor = Matrix3::Inverse(inertiaTensor);
}

Matrix3 RigidBody::getInertiaTensor() const
{
	return Matrix3::Inverse(mInverseInteriaTensor);
}

Matrix3 RigidBody::getInertiaTensorWorld() const
{
	return Matrix3::Inverse(mInverseInteriaTensorWorld);
}

void RigidBody::setInverseInertiaTensor(const Matrix3& inertiaTensor)
{
	mInverseInteriaTensor = inertiaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensor() const
{
	return mInverseInteriaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensorWorld() const
{
	return mInverseInteriaTensorWorld;
}

void RigidBody::setDamping(float linearDamping, float angularDamping)
{
	mLinearDamping = linearDamping;
	mAngularDamping = angularDamping;
}

void RigidBody::setLinearDamping(float linearDamping)
{
	mLinearDamping = linearDamping;
}

float RigidBody::getLinearDamping() const
{
	return mLinearDamping;
}

void RigidBody::setAngularDamping(float angularDamping)
{
	mAngularDamping = angularDamping;
}

float RigidBody::getAngularDamping() const
{
	return mAngularDamping;
}

Transform* RigidBody::getTransform()
{
	return &mTransform;
}

void RigidBody::setVelocity(const Vector3& velocity)
{
	mVelocity = velocity;
}

Vector3 RigidBody::getVelocity() const
{
	return mVelocity;
}

void RigidBody::AddVelocity(const Vector3& dVelocity)
{
	mVelocity += dVelocity;
}

void RigidBody::setAngularVelocity(const Vector3& angularVelocity)
{
	mAngularVelocity = angularVelocity;
}

void RigidBody::setAngularVelocity(float angVelX, float angVelY, float angVelZ)
{
	mAngularVelocity = Vector3(angVelX, angVelY, angVelZ);
}

Vector3 RigidBody::getAngularVelocity() const
{
	return mAngularVelocity;
}

void RigidBody::AddAngularVelocity(const Vector3& dAngularVelocity)
{
	mAngularVelocity += dAngularVelocity;
}

Vector3 RigidBody::getLastFrameAcceleration() const
{
	return mLastFrameAcceleration;
}

void RigidBody::ClearAccumilators()
{
	mForceAccum = Vector3(0.0f);
	mTorqueAccum = Vector3(0.0f);
}

void RigidBody::AddForce(const Vector3& force)
{
	mForceAccum += force;
}

void RigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
{
	Vector3 toPt = point - mTransform.GetPosition();

	mForceAccum += force;
	mTorqueAccum += Cross(toPt, force);
}

void RigidBody::AddTorque(const Vector3& torque)
{
	mTorqueAccum += torque;
}

}