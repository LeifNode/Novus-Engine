//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef PLANET_PARSER_H
#define PLANET_PARSER_H

#include <Application/Common.h>
#include <TinyXML/tinyxml2.h>

class PlanetParser
{
public:
	struct Planet
	{
		std::string Name;
		std::string Description;
		double Mass;
		double Radius;
		double Aphelion;
		double Perihelion;
		double SemiMajorAxis;
		double SemiMinorAxis;
		double PerihelionVelocity;
		int KeyBind;
	};
	
public:
	PlanetParser();

	bool Parse(const std::string& path);

	const std::vector<Planet>& getPlanets() const { return mPlanets; }

private: 
	void ParsePlanet(tinyxml2::XMLElement* planetElement, const Planet* parent);

private:
	//tinyxml2::XMLDocument mDocument;

	std::vector<Planet> mPlanets;
};

#endif