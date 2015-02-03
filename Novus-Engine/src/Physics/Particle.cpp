#include "Particle.h"

namespace novus
{

Particle::Particle()
	: mDamping(0.999f)
{

}

void Particle::Update(float dt)
{
	if (mInverseMass <= 0.0f)
		return;

	mPosition += mVelocity * dt;
	mVelocity += mAcceleration * mInverseMass * dt;

	//mVelocity *= static_cast<double>(powf(mDamping, dt));

	mAcceleration = Vector3();
}

void Particle::AddForce(const Vector3& force)
{
	mAcceleration += force;
}

void Particle::Reset()
{
	mVelocity = Vector3();
	mAcceleration = Vector3();
}

void Particle::setMass(float mass)
{
	assert(mass > 0.0f);

	mInverseMass = 1.0f / mass;
}

float Particle::getMass() const
{
	return 1.0f / mInverseMass;
}

void Particle::setInverseMass(float inverseMass)
{
	mInverseMass = inverseMass;
}

float Particle::getInverseMass() const
{
	return mInverseMass;
}

void Particle::setPosition(const Vector3& position)
{
	mPosition = position;
}

void Particle::setVelocity(const Vector3& velocity)
{
	mVelocity = velocity;
}

Vector3 Particle::getPosition() const
{
	return mPosition;
}

Vector3 Particle::getVelocity() const
{
	return mVelocity;
}

Vector3 Particle::getAcceleration() const
{
	return mAcceleration;
}

}