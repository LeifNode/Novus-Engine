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
	float3 PositionWorld;
	float4 Diffuse;
	float3 Normal;
	float3 SpecularColor;
	float Roughness;
	float3 Emissive;
};

void AccumulateBRDF(SURFACE_DATA surface, PointLight light, inout float3 finalColor)//Need to optimize
{
	float specPow = (1.0f - surface.Roughness) * 128.0f;

	float3 toEye = normalize(gEyePosition - surface.PositionWorld);
	float3 toLight = light.PositionWorld - surface.PositionWorld;
	float distance = length(toLight);
	toLight /= distance;

	float3 H = (toEye + toLight) / 2.0f;

	float NoV = dot(surface.Normal, toEye);
	float NoL = dot(surface.Normal, toLight);
	float NoH = dot(surface.Normal, H);

	float distanceSq = distance * distance;
	float oneOverDistSq = rcp(distanceSq);

	finalColor += light.Color * light.Intensity * saturate(NoL) * oneOverDistSq;
	finalColor += max(light.Color * pow(saturate(NoH), specPow) * light.Intensity * oneOverDistSq, 0.0);
}

#endif