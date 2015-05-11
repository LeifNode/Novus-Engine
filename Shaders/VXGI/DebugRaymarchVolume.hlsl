//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

Texture3D<float4> gVoxelVolume       : register(t0);

SamplerState gVolumeSampler        : register(s0);

RWTexture2D<float4> gOutputTexture : register(u0);

float3 WorldToVolume(float3 world)
{
	return mul(gWorldViewProj, float4(-world, 1.0f)).xyz * 0.5f + 0.5f;
	//return (world.xyz / float(15.0f).xxx + 1.0f) * 0.5f;
}

float4 convRGBA8ToVec4(uint val)
{
	return float4(float((val & 0x000000FF)), float((val & 0x0000FF00) >> 8U), float((val & 0x00FF0000) >> 16U), float((val & 0xFF000000) >> 24U));
}

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void RaymarchCS(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	int mipLevel = 0;

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
	forward += (clipCoords.x * right / gProjection[0][0] - clipCoords.y * up / projheight);
	forward = normalize(forward);

	//Find amount to advance each sample
	const int sampleCount = 1024;
	const float maxDistance = 20.0f;
	forward *= maxDistance / sampleCount;

	//Setup ray
	float3 samplePosition = -gEyePosition;
	float4 sampleColor;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float occlusion = 0.0f;

	[loop]
	for (int i = 0; i < sampleCount; i++)
	{
		//sampleColor = convRGBA8ToVec4(gVoxelVolume.SampleLevel(gVolumeSampler, WorldToVolume(samplePosition), 0));
		int3 volumeCoord = WorldToVolume(samplePosition) * volumeDimensions / pow(2, mipLevel);
		volumeCoord.x /= 6;
		volumeCoord.x += volumeDimensions.y * 5;
		sampleColor = gVoxelVolume.Load(int4(volumeCoord, mipLevel));

		//if (sampleColor.a > 0.0f)
			//sampleColor.a = 1.0f;

		//Average colors if this ray is partially occluded
		if (occlusion < 1.0f)
		{
			finalColor.rgb += sampleColor.rgb * (sampleColor.a + occlusion > 1.0f ? 1.0f - occlusion : sampleColor.a);
		}
		else
		{
			//gOutputTexture[globalCoords] = finalColor;
			break;
		}


		occlusion += sampleColor.a;
		samplePosition += forward;
	}

	gOutputTexture[globalCoords] = finalColor;
}