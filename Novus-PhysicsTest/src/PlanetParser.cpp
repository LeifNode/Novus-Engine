#include "PlanetParser.h"

using namespace tinyxml2;

PlanetParser::PlanetParser()
{}

bool PlanetParser::Parse(const std::string& path)
{
	tinyxml2::XMLDocument document;
	document.LoadFile(path.c_str());

	XMLElement* root = document.FirstChildElement("solarsystemlist");

	for (XMLElement* solarSystemIt = root->FirstChildElement("solarsystem"); solarSystemIt != NULL; solarSystemIt = solarSystemIt->NextSiblingElement())
	{
		for (XMLElement* planetIt = solarSystemIt->FirstChildElement("planet"); planetIt != NULL; planetIt = planetIt->NextSiblingElement())
		{
			ParsePlanet(planetIt, NULL);
		}
	}

	return true;
}

void PlanetParser::ParsePlanet(XMLElement* planetElement, const Planet* parent)
{
	Planet newPlanet;
	XMLElement* elementIt = planetElement->FirstChildElement("name");

	newPlanet.Name = std::string(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("mass");
	newPlanet.Mass = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("radius");
	newPlanet.Radius = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("aphelion");
	newPlanet.Aphelion = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("perihelion");
	newPlanet.Perihelion = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("semimajoraxis");
	newPlanet.SemiMajorAxis = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("semiminoraxis");
	newPlanet.SemiMinorAxis = atof(elementIt->GetText());

	elementIt = elementIt->NextSiblingElement("perihelionvelocity");
	newPlanet.PerihelionVelocity = atof(elementIt->GetText());

	if (parent != NULL)
	{
		newPlanet.Perihelion += parent->Perihelion;
		newPlanet.PerihelionVelocity += parent->PerihelionVelocity;
	}

	mPlanets.push_back(newPlanet);

	elementIt = elementIt->NextSiblingElement("moons"); 
	if (elementIt != NULL)
	{
		for (elementIt = elementIt->FirstChildElement("planet"); elementIt != NULL; elementIt = elementIt->NextSiblingElement())
		{
			ParsePlanet(elementIt, &newPlanet);
		}
	}
}