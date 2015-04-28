//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

Texture2D<float> gShadowMap           : register(t0);
Texture3D<float4> gVoxelVolume        : register(t1);
Texture3D<float4> gVoxelNormals       : register(t2);

//SamplerState gShadowSampler        : register(s0);

RWTexture3D<uint> gOutputTexture      : register(u0);
//RWTexture2D<float4> gOutputPosTexture : register(u1);

cbuffer cbLightInjectionPass          : register(b3)
{
	float3 gLightDirection;
	float pad;

	float3 gLightColor;
	float gLightIntensity;

	float4x4 gShadowToWorld;
	float4x4 gShadowToVoxelVolume;

	int2 gShadowMapDimensions;
	int2 pad2;

	int3 gVoxelVolumeDimensions;
	int pad3;
};

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void LightInjectionCS(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	if (dispatchThreadID.x >= (uint)gShadowMapDimensions.x || dispatchThreadID.y >= (uint)gShadowMapDimensions.y)
		return;

	float2 textureCoordinates = dispatchThreadID.xy / float2(gShadowMapDimensions);

	float4 screenSpaceCoords = float4(textureCoordinates * 2.0f - 1.0f, gShadowMap.Load(int3(dispatchThreadID.xy, 0)), 1.0f);
	screenSpaceCoords.y = -screenSpaceCoords.y;

	//float4 worldSpaceCoords = mul(gShadowToWorld, screenSpaceCoords);
	float4 volumeCoordinates = mul(gShadowToVoxelVolume, screenSpaceCoords);

	//gOutputPosTexture[dispatchThreadID.xy] = float4(worldSpaceCoords.xyz, 1.0f);

	volumeCoordinates.xyz = volumeCoordinates.xyz * 0.5f + 0.5f;

	volumeCoordinates.xyz *= float3(gVoxelVolumeDimensions);

	//float4 outputColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 outputColor = float4(gVoxelVolume.Load(int4(volumeCoordinates.xyz, 0)).rgb, 1.0f);
	float3 normal = gVoxelNormals.Load(int4(volumeCoordinates.xyz, 0)).rgb;

	outputColor.rgb *= dot(normal, -gLightDirection) * gLightIntensity * gLightColor;

	outputColor *= 255.0f;

	gOutputTexture[int3(volumeCoordinates.xyz)] = convVec4ToRGBA8(outputColor);

}