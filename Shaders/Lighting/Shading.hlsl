//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef SHADING_HLSL
#define SHADING_HLSL

#include "Lighting/Lights.hlsl"

struct SURFACE_DATA
{
	float3 PositionView;
	float4 Diffuse;
	float3 Normal;
	float3 SpecularColor;
	float Roughness;
	float3 Emissive;
};

void AccumulateBRDF(SURFACE_DATA surface, PointLight light, inout float3 finalColor)//Need to optimize
{
	float3 toEye = normalize(-surface.PositionView);
	float3 toLight = surface.PositionView - light.PositionView;
	float distance = length(toLight);
	toLight /= distance;

	float NoV = dot(surface.Normal, toEye);
	float NoL = dot(surface.Normal, toLight);

	float distanceSq = distance * distance;
	float oneOverDistSq = rcp(distanceSq);

	finalColor += light.Color * light.Intensity * saturate(NoL) * oneOverDistSq;
}

#endif