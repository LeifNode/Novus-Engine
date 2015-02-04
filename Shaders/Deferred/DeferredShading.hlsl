//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Handles sampling from GBuffer for lighting

#ifndef DEFERRED_SHADING_HLSL
#define DEFERRED_SHADING_HLSL

#include "Utils/Defines.hlsl"
#include "Utils/ConstantBuffers.hlsl"
#include "Lighting/Lights.hlsl"
#include "Lighting/Shading.hlsl"

Texture2D<float4> DiffuseTexture   : register(t0);
Texture2D<float3> NormalTexture    : register(t1);
Texture2D<float4> SpecularTexture  : register(t2);//rgb specular color, a roughness
Texture2D<float4> EmissiveTexture  : register(t3);//a is unused at the moment
Texture2D<float> DepthTexture      : register(t4);

RWTexture2D<float4> OutputTexture  : register(u0);

SamplerState PointSampler          : register(s0);

StructuredBuffer<PointLight> gPointLights : register(t5);

float ConvertZToLinearDepth(float depth)
{
	float linearDepth = gProjection._43 / (depth + gProjection._33);
	return linearDepth;
}

float3 GetViewPos(float2 posClip, float viewSpaceZ)
{
	float3 position;
	//float linearDepth = ConvertZToLinearDepth(depth);

	position.xy = posClip.xy * float2(1.0 / gProjection._11, 1.0 / gProjection._22) * viewSpaceZ;
	position.z = viewSpaceZ;

	return position;
}

float3 GetWorldPos(float2 posClip, float depth)
{
	float4 position;

	position.xy = posClip.xy;

	position.z = depth;
	position.w = 1.0;


	position = mul(position, gViewProjInv);

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

SURFACE_DATA UnpackGBufferViewport(uint2 viewportPosition)
{
	SURFACE_DATA Out;

	
	Out.Diffuse = DiffuseTexture.Load(uint3(viewportPosition, 0));
	Out.Normal = NormalTexture.Load(uint3(viewportPosition, 0)).rgb * 2.0 - 1.0;
	float4 spec = SpecularTexture.Load(uint3(viewportPosition, 0));
	Out.SpecularColor = spec.rgb;
	Out.Roughness = spec.a;
	Out.Emissive = EmissiveTexture.Load(uint3(viewportPosition, 0)).rgb;

	float2 GBufferDim;
	DiffuseTexture.GetDimensions(GBufferDim.x, GBufferDim.y);

	//Calcluate screen space position
	float2 ScreenPixelOffset = float2(2.0f, -2.0f) / GBufferDim;
	float2 ScreenPosition = (float2(viewportPosition.xy) + 0.5f) * ScreenPixelOffset.xy + float2(-1.0f, 1.0f);

	float zBuffer = DepthTexture.Load(uint3(viewportPosition, 0)).r;
	float ViewSpaceZ = ConvertZToLinearDepth(zBuffer);

	Out.PositionView = GetViewPos(ScreenPosition, ViewSpaceZ);
	Out.PositionWorld = GetWorldPos(ScreenPosition, zBuffer);

	return Out;
}

#endif