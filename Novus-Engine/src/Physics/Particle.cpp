#include "Particle.h"

namespace novus
{

Particle::Particle()
{

}

void Particle::Update(float dt)
{
	if (mInverseMass <= 0.0f)
		return;

	mPosition += mVelocity * static_cast<double>(dt);
	mVelocity += mAcceleration * static_cast<double>(mInverseMass) * static_cast<double>(dt);

	mVelocity *= static_cast<double>(powf(mDamping, dt));

	mAcceleration = DVector3();
}

void Particle::AddForce(const DVector3& force)
{
	mAcceleration += force;
}

void Particle::Reset()
{
	mVelocity = DVector3();
	mAcceleration = DVector3();
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

void Particle::setPosition(const DVector3& position)
{
	mPosition = position;
}

void Particle::setVelocity(const DVector3& velocity)
{
	mVelocity = velocity;
}

DVector3 Particle::getPosition() const
{
	return mPosition;
}

DVector3 Particle::getVelocity() const
{
	return mVelocity;
}

DVector3 Particle::getAcceleration() const
{
	return mAcceleration;
}

}