//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_LIGHTS_H
#define NOVUS_LIGHTS_H

#include "Math/Math.h"

namespace novus
{
struct PointLight
{
	Vector3 Color;
	float Intensity;

	Vector3 PositionView;
	float Range;

	Vector3 PositionWorld;
	float pad;

	float Radius;
	float FalloffPow;
	Vector2 pad2;
};
}

#endif