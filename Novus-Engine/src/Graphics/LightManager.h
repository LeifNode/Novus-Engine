//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_LIGHT_MANAGER_H
#define NOVUS_LIGHT_MANAGER_H

#include <vector>
#include "Lights.h"

namespace novus
{

class LightManager
{
public:
	LightManager();
	~LightManager();

	void AddLightForFrame(const PointLight& light);

	void ClearLights();

	const std::vector<PointLight>& getLights();

private:
	std::vector<PointLight> mLights;
};

}

#endif