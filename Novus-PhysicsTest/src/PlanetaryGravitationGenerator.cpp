#include "PlanetaryGravitationGenerator.h"

using namespace novus;

PlanetaryGravitationGenerator::PlanetaryGravitationGenerator(PhysicsSystem* physicsSystem)
: mpPhysicsSystem(physicsSystem)
{
}

void PlanetaryGravitationGenerator::UpdateForce(Particle* particle, float dt)
{
	const static double gravitationalConstant = 6.673e-20f;

	for (auto it = mpPhysicsSystem->getParticles().cbegin(); it != mpPhysicsSystem->getParticles().cend(); ++it)
	{
		//Don't apply force to self
		if (particle != (*it))
		{
			double distanceSq = static_cast<double>(LengthSq(particle->getPosition() - (*it)->getPosition()));

			double mass = static_cast<double>(particle->getMass()) * static_cast<double>((*it)->getMass());

			double force = gravitationalConstant * (mass / distanceSq);

			Vector3d forceVec = Normalize((*it)->getPosition() - particle->getPosition()) * force;

			particle->AddForce(forceVec);
		}
	}
}