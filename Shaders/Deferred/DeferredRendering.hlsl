//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================


//Handles rendering to the GBuffer

#ifndef DEFERRED_RENDERING_HLSL
#define DEFERRED_RENDERING_HLSL

#include "Utils/Defines.hlsl"
#include "Utils/ConstantBuffers.hlsl"
#include "Lighting/Shading.hlsl"

struct PS_GBUFFER_OUT
{
	float4 Diffuse   : SV_TARGET0;
	float3 Normal    : SV_TARGET1;
	float4 Specular  : SV_TARGET2;
	float4 Emissive  : SV_TARGET3;
};

PS_GBUFFER_OUT PackGBuffer(float4 DiffuseColor, float3 Normal, float3 SpecularColor, float Roughness, float Metallic, float3 Emissive)
{
	PS_GBUFFER_OUT Out;

	// Pack all the data into the GBuffer structure
	Out.Diffuse = DiffuseColor;
	Out.Normal = Normal * 0.5 + 0.5;
	Out.Specular = float4(SpecularColor, Roughness);
	Out.Emissive = float4(Emissive, Metallic);

	return Out;
}

#endif