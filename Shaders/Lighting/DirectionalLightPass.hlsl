//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "Deferred/GBuffer.hlsl"
#include "Lighting/Shading.hlsl"

Texture2D<float> gShadowMap        : register(t5);

SamplerState gShadowSampler        : register(s0);

RWTexture2D<float4> gOutputTexture : register(u0);

cbuffer cbShadowPass               : register(b2)
{
	float4x4 gToTextureTransform;
	float4 gDirLightColor;
	float3 gDirLightDirection;
	float pad;
};

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void DirLightAccum(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint2 globalCoords = dispatchThreadID.xy;

	uint2 outputDimensions;
	gOutputTexture.GetDimensions(outputDimensions.x, outputDimensions.y);

	if (globalCoords.x >= outputDimensions.x || globalCoords.y >= outputDimensions.y)
		return;

	SURFACE_DATA surface = UnpackGBufferViewport(globalCoords);

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float roughness2 = surface.Roughness * surface.Roughness;

	float3 toLight = gDirLightDirection;
	float3 toEye = normalize(gEyePosition - surface.PositionWorld);

	float3 H = normalize(toLight + toEye);

	float NoV = abs(dot(surface.Normal, toEye));
	float NoL = saturate(dot(surface.Normal, toLight));
	float NoH = saturate(dot(surface.Normal, H));
	float VoH = saturate(dot(toEye, H));
	float LoH = saturate(dot(toLight, H));

	//float diffuseContrib = saturate(NoL);
	float diffuseContrib = saturate(Diffuse_OrenNayar(roughness2, surface.Normal, toLight, toEye, NoL, NoV));

	float D = D_GGX(roughness2, NoH);
	float G = G_Smith(surface.Roughness, NoV, NoL);
	float3 F = F_Schlick(lerp(float3(0.0f, 0.0f, 0.0f), surface.SpecularColor, surface.Metallic), VoH);

	float3 specContrib = ((D*G*F) / (4.0 * NoL * NoV)) * NoL;//TODO: This is not correct, but fixes the specular falloff to some degree for the time being

	finalColor += gDirLightColor.rgb / PI * surface.Diffuse.rgb * diffuseContrib * (1.0f - surface.Metallic);//Diffuse
	finalColor += gDirLightColor.rgb * saturate(specContrib) * lerp(float3(1.0f, 1.0f, 1.0f), surface.SpecularColor, surface.Metallic);//Specular

	float4 color = float4(finalColor, 0.0f);

	//float4 oldColor = gOutputTexture[globalCoords];


}