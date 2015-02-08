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

class Planet : public novus::Particle
{
public:
	Planet() : mPlanetRadius(0.0) {}

	const std::string& getName() const { return mPlanetName; }
	void setName(const std::string& name) { mPlanetName = name; }

	double getRadius() const { return mPlanetRadius; }
	void setRadius(double radius) { mPlanetRadius = radius; }

private:
	Planet(const Planet&);
	Planet& operator= (const Planet&);


private:
	std::string mPlanetName;
	double mPlanetRadius;
};


#endif