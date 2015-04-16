//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "Deferred/GBuffer.hlsl"
#include "Lighting/Shading.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

Texture2D<float> gShadowMap                    : register(t5);
Texture3D<float4> gVoxelVolume                 : register(t6);
Texture3D<float4> gVoxelVolumeAnisotropicMips  : register(t7);

SamplerState gShadowSampler        : register(s0);
SamplerState gVoxelSampler         : register(s1);

RWTexture2D<float4> gOutputTexture : register(u0);

cbuffer cbShadowPass               : register(b3)
{
	float3 gLightDirection;
	float gShadowIntensity;

	float4 gLightColor;

	float4x4 gWorldToShadow;
};

float CalculateShadow(SamplerState texSampler, Texture2D<float> depthTex, float4 shadowPosH)
{
	float depth = shadowPosH.z;

	float shadowDepth = depthTex.SampleLevel(gShadowSampler, shadowPosH.xy, 0);

	return shadowDepth < depth ? 0.0f : 1.0f;
}

//void coneTraceScene(float3 startPosition, float3 direction, )

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void GlobalIllumEvaluation(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint2 globalCoords = dispatchThreadID.xy;

	uint2 outputDimensions;
	gOutputTexture.GetDimensions(outputDimensions.x, outputDimensions.y);

	if (globalCoords.x >= outputDimensions.x || globalCoords.y >= outputDimensions.y)
		return;

	SURFACE_DATA surface = UnpackGBufferViewport(globalCoords);

	surface.Roughness += 0.3f;

	//Direct lighting
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float roughness2 = surface.Roughness * surface.Roughness;

	float3 toLight = float3(-gLightDirection);
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

	finalColor += (gLightColor.rgb / PI) * surface.Diffuse.rgb * diffuseContrib * (1.0f - surface.Metallic);//Diffuse
	//finalColor += (gLightColor.rgb / PI) * diffuseContrib * (1.0f - surface.Metallic);//Diffuse
	finalColor += gLightColor.rgb * saturate(specContrib) * lerp(float3(1.0f, 1.0f, 1.0f), surface.SpecularColor, surface.Metallic);//Specular

	float4 pointShadowSampleSpace = mul(gWorldToShadow, float4(surface.PositionWorld, 1.0f));

	float shadowContrib = CalculateShadow(gShadowSampler, gShadowMap, pointShadowSampleSpace);

	float4 directColor = float4(finalColor * shadowContrib, 1.0f);

		//Cone tracing
	float3 coneSamplePosition = surface.PositionWorld;
	float3 coneSampleDirection = reflect(-toEye, surface.Normal);

	const float radiusRatio = sin(calculateSpecularConeHalfAngle(roughness2));

	//const float radiusRatio = sin(0.05f);
	float startSampleDistance = 0.03f;

	coneSamplePosition += surface.Normal * gVoxelScale * 2.0f; //Offset to avoid self intersections
	float3 originSamplePosition = coneSamplePosition;

	coneSamplePosition += coneSampleDirection * startSampleDistance;
	float currentRadius = radiusRatio * startSampleDistance;
	float currentDistance = startSampleDistance;

	float3 accumilatedColor = float3(0.0f, 0.0f, 0.0f); 
	float accumilatedOcclusion = 0.0f;

	//Specular GI
	for (int i = 0; i < 512; i++)
	{
		float lastDistance = currentDistance;
		//currentDistance = (currentDistance * (1.0f + radiusRatio)) / (1.0f - radiusRatio);
		currentDistance = currentDistance / (1.0f - radiusRatio);
		coneSamplePosition = originSamplePosition + coneSampleDirection * currentDistance;
		currentRadius = radiusRatio * currentDistance;

		//float4 sampleColor = sampleVoxelVolume(gVoxelVolume, gVoxelSampler, coneSamplePosition, currentRadius);
		float4 sampleColor = sampleVoxelVolumeAnisotropic(gVoxelVolume, gVoxelVolumeAnisotropicMips, gVoxelSampler, coneSamplePosition, currentRadius, coneSampleDirection);
	
		//sampleColor.a = 1.0f - pow((1.0f - sampleColor.a), ((currentDistance - lastDistance) / currentRadius));

		accumilateColorOcclusion(sampleColor, accumilatedColor, accumilatedOcclusion);

		if (accumilatedOcclusion >= 1.0f)
			break;
	}

	float3 accumilatedSpecular = accumilatedColor;

	float3 up = surface.Normal.y > 0.98f ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(surface.Normal, up);
	up = cross(surface.Normal, right);

	const float diffuseRadiusRatio = sin(0.53f);//60 degree cones

	//Diffuse GI
	float3 diffuseAccum = float3(0.0f, 0.0f, 0.0f);
	float diffuseOcclusionAccum = 0.0f;
	startSampleDistance = 0.2f;

	for (int conei = 0; conei < 6; conei++)
	{
		coneSamplePosition = surface.PositionWorld;
		coneSamplePosition += surface.Normal * gVoxelScale * 2.0f; //Offset to avoid self intersections
		originSamplePosition = coneSamplePosition;

		coneSampleDirection = surface.Normal;
		coneSampleDirection += cVXGIConeSampleDirections[conei].x * right + cVXGIConeSampleDirections[conei].z * up;
		coneSampleDirection = normalize(coneSampleDirection);

		coneSamplePosition += coneSampleDirection * startSampleDistance;
		currentRadius = diffuseRadiusRatio * startSampleDistance;
		currentDistance = startSampleDistance;

		accumilatedColor = float3(0.0f, 0.0f, 0.0f);
		accumilatedOcclusion = 0.0f;
		float ambientOcclusionAccum = 0.0f;

		for (int i = 0; i < 64; i++)
		{
			float4 sampleColor = sampleVoxelVolumeAnisotropic(gVoxelVolume, gVoxelVolumeAnisotropicMips, gVoxelSampler, coneSamplePosition, currentRadius, coneSampleDirection);

			float lastDistance = currentDistance;
			currentDistance = currentDistance / (1.0f - diffuseRadiusRatio);
			coneSamplePosition = originSamplePosition + coneSampleDirection * currentDistance;
			currentRadius = diffuseRadiusRatio * currentDistance;

			sampleColor.a = 1.0f - pow((1.0f - sampleColor.a), ((currentDistance - lastDistance) / currentRadius));
			accumilateColorOcclusion(sampleColor, accumilatedColor, accumilatedOcclusion);

			ambientOcclusionAccum += sampleColor.a * (0.23f / (currentDistance + 1.0f));

			if (accumilatedOcclusion >= 1.0f)
				break;
		}

		diffuseAccum += accumilatedColor * cVXGIConeSampleWeights[conei];
		diffuseOcclusionAccum += ambientOcclusionAccum * cVXGIConeSampleWeights[conei];
	}

	//float4 sampledColor = sampleVoxelVolume(gVoxelVolume, gVoxelSampler, coneSamplePosition, 0.1f);

	//gOutputTexture[globalCoords] = directColor * 11.0f;
	gOutputTexture[globalCoords] = float4(accumilatedSpecular.rgb + (directColor.rgb * 11.0f) * (1.0f - diffuseOcclusionAccum) + diffuseAccum.rgb * surface.Diffuse.rgb, 1.0f);
	//gOutputTexture[globalCoords] = float4(diffuseAccum.rgb + directColor.rgb * 11.0f, 1.0f);
	//gOutputTexture[globalCoords] = float4(accumilatedSpecular.rgb + directColor.rgb * 11.0f, 1.0f);
	//gOutputTexture[globalCoords] = float4(1.0f - diffuseOcclusionAccum.rrr, 1.0f);
}