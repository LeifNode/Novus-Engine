//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_PARTICLE_CONTACT_GENERATOR_H
#define NOVUS_PARTICLE_CONTACT_GENERATOR_H

#include "Particle.h"
#include "ParticleContacts.h"

namespace novus
{

class ParticleContactGenerator
{
public:
	virtual unsigned AddContact(ParticleContact* contact, unsigned limit) const = 0;
};

class ParticleLink : public ParticleContactGenerator
{
public:
	virtual unsigned AddContact(ParticleContact* contact, unsigned limit) const = 0;

public:
	Particle* particle[2];

protected:
	float currentLength() const;
};

class ParticleCable : public ParticleLink
{
public:
	unsigned AddContact(ParticleContact* contact, unsigned limit) const override;

public:
	float maxLength;

	float restitution;
};

class ParticleRod : public ParticleLink
{
public:
	unsigned AddContact(ParticleContact* contact, unsigned limit) const override;

public:
	float length;
};

class ParticlePlane : public ParticleContactGenerator
{
public:
	ParticlePlane(const Vector3& point, const Vector3& normal, float particleRadius);

	unsigned AddContact(ParticleContact* contact, unsigned limit) const override;

public:
	std::vector<Particle*>* pParticles;

	Vector3 point;
	Vector3 normal;
	float particleRadius;

	float restitution;
};

}

#endif