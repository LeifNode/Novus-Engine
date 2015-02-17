//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef PLANET_UI_RENDERER_H
#define PLANET_UI_RENDERER_H

namespace novus
{
	class D3DRenderer;
	class TextRenderer;
}

class PlanetParticle;

class PlanetUIRenderer
{
public:
	PlanetUIRenderer();
	~PlanetUIRenderer();

	void Init();

	void SelectPlanet(const PlanetParticle* planet);

	void Render(novus::D3DRenderer* renderer);

private:
	novus::TextRenderer* mpTextRenderer;
};

#endif