//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef LIGHTS_HLSL
#define LIGHTS_HLSL

struct Material
{
	float4 Diffuse;
	float3 SpecularColor;
	float Roughness;
	float3 Emissive;
	float Metallic;

	bool HasDiffuseTexture;
	bool HasNormalTexture;
	bool HasSpecularTexture;
	bool pad3;
};

struct DirectionalLight
{
	float4 Color;
	float3 Direction;

	float Intensity;
};

struct PointLight
{
	float3 Color;
	float Intensity;

	float3 PositionView;
	float Range;

	float3 PositionWorld;
	float pad;

	float Radius;
	float FalloffPow;
	float2 pad2;
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