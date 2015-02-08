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

	mPosition += mVelocity * static_cast<double>(dt);
	mVelocity += mAcceleration * static_cast<double>(mInverseMass)* static_cast<double>(dt);

	//mVelocity *= static_cast<double>(powf(mDamping, dt));

	mAcceleration = Vector3d();
}

void Particle::AddForce(const Vector3d& force)
{
	mAcceleration += force;
}

void Particle::Reset()
{
	mVelocity = Vector3d();
	mAcceleration = Vector3d();
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

void Particle::setPosition(const Vector3d& position)
{
	mPosition = position;
}

void Particle::setVelocity(const Vector3d& velocity)
{
	mVelocity = velocity;
}

Vector3d Particle::getPosition() const
{
	return mPosition;
}

Vector3d Particle::getVelocity() const
{
	return mVelocity;
}

Vector3d Particle::getAcceleration() const
{
	return mAcceleration;
}

}