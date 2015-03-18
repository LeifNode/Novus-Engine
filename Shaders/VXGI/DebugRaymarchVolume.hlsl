//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

Texture3D<float4> gVoxelVolume     : register(t0);

SamplerState gVolumeSampler        : register(s0);

RWTexture2D<float4> gOutputTexture : register(u0);

float3 WorldToVolume(float3 world)
{
	return (world.xyz / float(15.0f).xxx + 1.0f) * 0.5f;
}

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void RaymarchCS(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint2 globalCoords = dispatchThreadID.xy;

	uint2 outputDimensions;
	gOutputTexture.GetDimensions(outputDimensions.x, outputDimensions.y);

	uint3 volumeDimensions;
	gVoxelVolume.GetDimensions(volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);

	if (globalCoords.x >= outputDimensions.x || globalCoords.y >= outputDimensions.y)
		return;

	//Find world space ray from pixel coordinates
	float2 sampleCoords = float2(globalCoords.xy) / float2(outputDimensions.xy);
	float2 clipCoords = sampleCoords * 2.0f - 1.0f;
	float2 uv = clipCoords;
	uv *= outputDimensions.x / outputDimensions.y;

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 forward = normalize(float3(gView[2][0], gView[2][1], gView[2][2]));

	float3 right = cross(forward, up);
	up = cross(forward, right);

	float projheight = gProjection[1][1];
	forward += (clipCoords.x * right - clipCoords.y * up / projheight);
	forward = normalize(forward);

	//Find amount to advance each sample
	const int sampleCount = 256;
	const float maxDistance = 10.0f;
	forward *= maxDistance / sampleCount;

	//Setup ray
	float3 samplePosition = -gEyePosition;
	float4 sampleColor;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float occlusion = 0.0f;

	[unroll]
	for (int i = 0; i < sampleCount; i++)
	{
		//sampleColor = gVoxelVolume.SampleLevel(gVolumeSampler, WorldToVolume(samplePosition), 0);
		sampleColor = gVoxelVolume.Load(int4(WorldToVolume(samplePosition) * volumeDimensions, 0));

		//Average colors if this ray is partially occluded
		[flatten]
		if (occlusion < 1.0f)
		{
			finalColor.rgb += sampleColor.rgb * (sampleColor.a + occlusion > 1.0f ? 1.0f - occlusion : sampleColor.a);
			
		}
		occlusion += sampleColor.a;
		samplePosition += forward;
	}

	gOutputTexture[globalCoords] = finalColor;
}