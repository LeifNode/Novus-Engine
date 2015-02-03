//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef LIGHTS_HLSL
#define LIGHTS_HLSL

struct DirectionalLight
{
	float4 Color;
	float3 Direction;

	float Intensity;
};

struct PointLight
{
	float4 Color;

	float3 PositionView;
	float Range;

	float Intensity;
	float Radius;
	float FalloffPow;
	float pad;
};

struct SpotLight
{
	float4 Color;

	float3 Position;
	float Range;

	float3 Direction;

	float InnerAngle;
	float Penumbra;
	float Intensity;
	float2 pad;
};

#endif