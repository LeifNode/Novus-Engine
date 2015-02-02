//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Handles sampling from GBuffer for lighting

#ifndef DEFERRED_SHADING_HLSL
#define DEFERRED_SHADING_HLSL

#include "Defines.hlsl"
#include "ConstantBuffers.hlsl"
#include "Lights.hlsl"

Texture2D<float4> DiffuseTexture   : register(t0);
Texture2D<float3> NormalTexture    : register(t1);
Texture2D<float4> SpecularTexture  : register(t2);//rgb specular color, a roughness
Texture2D<float4> EmissiveTexture  : register(t3);//a is unused at the moment
Texture2D<float> DepthTexture      : register(t4);

SamplerState PointSampler          : register(s0);

StructuredBuffer<PointLight> gPointLights : register(t5);

struct SURFACE_DATA
{
	float3 ViewPosition;
	float4 Diffuse;
	float3 Normal;
	float3 SpecularColor;
	float Roughness;
	float3 Emissive;
};

float ConvertZToLinearDepth(float depth)
{
	float linearDepth = gProjection[3][2] / (depth + gProjection[2][2]);
	return linearDepth;
}

float3 GetViewPos(float2 posClip, float viewSpaceZ)
{
	float3 position;
	//float linearDepth = ConvertZToLinearDepth(depth);

	position.xy = posClip.xy * float2(1.0 / gProjection[0][0], 1.0 / gProjection[1][1]) * viewSpaceZ;
	position.z = viewSpaceZ;

	return position;
}

float3 GetWorldPos(float2 posClip, float depth)
{
	float4 position;

	position.xy = posClip.xy;

	position.z = depth;
	position.w = 1.0;


	position = mul(gViewProjInv, position);

	return position.xyz / position.w;
}

//SURFACE_DATA UnpackGBuffer(float2 UV)
//{
//	SURFACE_DATA Out;
//
//	Out.Depth = DepthTexture.Sample(PointSampler, UV).r;
//	Out.Diffuse = DiffuseTexture.Sample(PointSampler, UV);
//	Out.Normal = normalize(NormalTexture.Sample(PointSampler, UV).rgb * 2.0 - 1.0);
//	float4 spec = SpecularTexture.Sample(PointSampler, UV);
//	Out.SpecularColor = spec.rgb;
//	Out.Roughness = spec.a;
//	Out.Emissive = EmissiveTexture.Sample(PointSampler, UV).rgb;
//
//	return Out;
//}

SURFACE_DATA UnpackGBufferViewport(uint viewportPosition)
{
	SURFACE_DATA Out;

	
	Out.Diffuse = DiffuseTexture.Load(viewportPosition);
	Out.Normal = NormalTexture.Load(viewportPosition).rgb * 2.0 - 1.0);
	float4 spec = SpecularTexture.Load(viewportPosition);
	Out.SpecularColor = spec.rgb;
	Out.Roughness = spec.a;
	Out.Emissive = EmissiveTexture.Load(viewportPosition).rgb;



	float zBuffer = DepthTexture.Load(PointSampler, UV).r;

	return Out;
}

#endif