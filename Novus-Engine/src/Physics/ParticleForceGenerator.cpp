#include "ParticleForceGenerator.h"
#include "MassAggregatePhysicsSystem.h"
#include "Math/Math.h"

namespace novus
{

ParticlePlanetaryGravitation::ParticlePlanetaryGravitation(MassAggregatePhysicsSystem* physicsSystem)
	: mpPhysicsSystem(physicsSystem)
{
}

void ParticlePlanetaryGravitation::UpdateForce(Particle* particle, float dt)
{
	const static double gravitationalConstant = 6.673e-11f;

	for (auto it = mpPhysicsSystem->getParticles().cbegin(); it != mpPhysicsSystem->getParticles().cend(); ++it)
	{
		//Don't apply force to self
		if (particle != (*it))
		{
			double distanceSq = static_cast<double>(LengthSq(particle->getPosition() - (*it)->getPosition()));

			double mass = static_cast<double>(particle->getMass()) * static_cast<double>((*it)->getMass());

			double force = gravitationalConstant * (mass / distanceSq);

			Vector3 forceVec = Normalize((*it)->getPosition() - particle->getPosition()) * static_cast<float>(force);

			particle->AddForce(forceVec);
		}
	}
}


ParticleGravity::ParticleGravity(const Vector3& gravity)
	:mGravity(gravity)
{
}

void ParticleGravity::UpdateForce(Particle* particle, float dt)
{
	if (particle->getInverseMass() <= 0.0f)
		return;

	particle->AddForce(mGravity * particle->getMass());
}

ParticleDrag::ParticleDrag(float k1, float k2)
	:mK1(k1),
	mK2(k2)
{}

void ParticleDrag::UpdateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getVelocity();

	float dragCoeff = Length(force);
	dragCoeff = mK1 * dragCoeff + mK2 * dragCoeff * dragCoeff;

	force = Normalize(force);
	force *= -dragCoeff;

	particle->AddForce(force);
}

ParticleSpring::ParticleSpring(Particle* other, float springConstant, float restLength)
	:other(other),
	springConstant(springConstant),
	restLength(restLength)
{

}

void ParticleSpring::UpdateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition();

	force -= other->getPosition();

	float magnitude = Length(force);
	magnitude = fabsf(magnitude - restLength);
	magnitude *= springConstant;

	force = Normalize(force);
	force *= -magnitude;

	particle->AddForce(force);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vector3* anchor, float springConstant, float restLength)
	:anchor(anchor),
	springConstant(springConstant),
	restLength(restLength)
{
}

void ParticleAnchoredSpring::UpdateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition() - *anchor;

	float magnitude = Length(force);
	magnitude = (restLength - magnitude) * springConstant;

	force = Normalize(force);
	force *= magnitude;

	particle->AddForce(force);
}

ParticleBungee::ParticleBungee(Particle* otherP, float springConstant, float restLength)
	:other(otherP),
	springConstant(springConstant),
	restLength(restLength)
{}

void ParticleBungee::UpdateForce(Particle* particle, float dt)
{
	Vector3 force = particle->getPosition();
	force -= other->getPosition();

	float magnitude = Length(force);
	if (magnitude <= restLength)
		return;

	magnitude = springConstant * (restLength - magnitude);

	force = Normalize(force);
	force *= magnitude;

	particle->AddForce(force);
}

void ParticleForceRegistry::Update(float dt)
{
	for (auto it = registrations.begin(); it != registrations.end(); ++it)
	{
		it->fg->UpdateForce(it->particle, dt);
	}
}

void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* fg)
{
	ParticleForceRegistry::ParticleForceRegistration registration;
	registration.particle = particle;
	registration.fg = fg;
	registrations.push_back(registration);
}

void ParticleForceRegistry::Clear()
{
	registrations.clear();
}

}//namespace novus