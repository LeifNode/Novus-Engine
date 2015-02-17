//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef PLANET_H
#define PLANET_H

#include <Application/Common.h>
#include <Physics/Particle.h>

namespace novus
{
	class LineRenderer;
	class D3DRenderer;
	class TextRenderer;
}

class PlanetParticle : public novus::Particle
{
public:
	PlanetParticle();
	~PlanetParticle();

	const std::string& getName() const { return mPlanetName; }
	void setName(const std::string& name) { mPlanetName = name; }

	double getRadius() const { return mPlanetRadius; }
	void setRadius(double radius) { mPlanetRadius = radius; }

	void Init();

	virtual void Update(float dt) override;
	void Render(novus::D3DRenderer* renderer);

	void ForwardRender(novus::D3DRenderer* renderer);

private:
	PlanetParticle(const PlanetParticle&);
	PlanetParticle& operator= (const PlanetParticle&);

private:
	std::string mPlanetName;
	double mPlanetRadius;

	novus::LineRenderer* mpPathRenderer;
	float mTimeToUpdate;

	novus::LineRenderer* mpForceVector;
	novus::LineRenderer* mpVelocityVector;

	novus::TextRenderer* mpNameRenderer;
};


#endif