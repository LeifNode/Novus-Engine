//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Handles sampling from GBuffer for lighting

#ifndef DEFERRED_SHADING_HLSL
#define DEFERRED_SHADING_HLSL

#include "ConstantBuffers.hlsl"
#include "Defines.hlsl"

Texture2D<float4> DiffuseTexture   : register(t0);
Texture2D<float3> NormalTexture    : register(t1);
Texture2D<float> RoughnessTexture  : register(t2);
Texture2D<float4> EmissiveTexture  : register(t3);
Texture2D<float> DepthTexture      : register(t4);

SamplerState PointSampler          : register(s0);

struct SURFACE_DATA
{
	float Depth;
	float LinearDepth;
	float4 Diffuse;
	float3 Normal;
	float3 Specular;
	float SpecPow;
	float3 Emissive;
	float EmissiveMultiplier;
};

float ConvertZToLinearDepth(float depth)
{
	float linearDepth = gProjection[3][2] / (depth + gProjection[2][2]);
	return linearDepth;
}

//float3 GetWorldPos(float2 posClip, float depth)
//{
//	float4 position;
//	float linearDepth = ConvertZToLinearDepth(depth);
//
//	position.xy = posClip.xy * float2(1.0 / gProjection[0][0], 1.0 / gProjection[1][1]) * linearDepth;
//	position.z = linearDepth;
//	position.w = 1.0;
//
//	return mul(gViewInv, position).xyz;
//}

float3 GetWorldPos(float2 posClip, float depth)
{
	float4 position;

	position.xy = posClip.xy;

	position.z = depth;
	position.w = 1.0;


	position = mul(gViewProjInv, position);

	return position.xyz / position.w;
}

SURFACE_DATA UnpackGBuffer(float2 UV)
{
	SURFACE_DATA Out;

	Out.Depth = DepthTexture.Sample(PointSampler, UV).x;
	Out.LinearDepth = ConvertZToLinearDepth(Out.Depth);
	Out.Diffuse = DiffuseTexture.Sample(PointSampler, UV);
	Out.Normal = normalize(NormalTexture.Sample(PointSampler, UV).xyz * 2.0 - 1.0);
	float4 spec = SpecularTexture.Sample(PointSampler, UV);
		Out.Specular = spec.xyz;
	Out.SpecPow = spec.w * SPECULAR_POW_RANGE_MAX + SPECULAR_POW_RANGE_MIN;
	float4 emissive = EmissiveTexture.Sample(PointSampler, UV);
		Out.Emissive = emissive.xyz;
	Out.EmissiveMultiplier = emissive.z * EMISSIVE_RANGE_MAX + EMISSIVE_RANGE_MIN;

	return Out;
}

#endif