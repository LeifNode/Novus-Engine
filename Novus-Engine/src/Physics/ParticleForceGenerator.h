#pragma once

#ifndef NOVUS_PARTICLE_FORCE_GENERATOR_H
#define NOVUS_PARTICLE_FORCE_GENERATOR_H

#include "Particle.h"

namespace novus
{

class ParticleForceGenerator
{
public:
	virtual void UpdateForce(Particle* particle, float dt) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
public:
	ParticleGravity(const Vector3& gravity);

	virtual void UpdateForce(Particle* particle, float dt) override;

private:
	Vector3 mGravity;
};

class ParticleDrag : public ParticleForceGenerator
{
public:
	ParticleDrag(float k1, float k2);

	virtual void UpdateForce(Particle* particle, float dt) override;

private:
	float mK1, mK2;
};

class ParticleSpring : public ParticleForceGenerator
{
public:
	ParticleSpring(Particle* other, float springConstant, float restLength);

	virtual void UpdateForce(Particle* particle, float dt) override;

private:
	Particle* other;

	float springConstant;

	float restLength;
};

class ParticleAnchoredSpring : public ParticleForceGenerator
{
public:
	ParticleAnchoredSpring(Vector3* anchor, float springConstant, float restLength);

	virtual void UpdateForce(Particle* particle, float dt) override;

protected:
	Vector3* anchor;

	float springConstant;
	float restLength;
};

class ParticleBungee : public ParticleForceGenerator
{
public:
	ParticleBungee(Particle* other, float springConstant, float restLength);

	virtual void UpdateForce(Particle* particle, float dt) override;

private:
	Particle* other;

	float springConstant;
	float restLength;
};

class ParticleForceRegistry
{
protected:
	struct ParticleForceRegistration
	{
		Particle* particle;
		ParticleForceGenerator* fg;
	};

	std::vector<ParticleForceRegistration> registrations;

public:
	void Add(Particle* particle, ParticleForceGenerator* fg);

	void Clear();

	void Update(float dt);
};

}//namespace novus

#endif