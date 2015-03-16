//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "Lighting/Shading.hlsl"

TextureCube         EnvironmentMap : register(t0);
SamplerState        EnvSampler     : register(s0);

RWTexture2DArray<float4> OutputTexture  : register(u0);

cbuffer cbPreFilter : register(b2)
{
	int2 gSourceDimensions;
	int gCurrentMip;
	int gMipCount;

	float3 gUp;
	float pad2;
	float3 gForward;
	float pad3;
};

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void FilterEnvMapCS(uint3 groupID          : SV_GroupID,
					uint3 dispatchThreadID : SV_DispatchThreadID,
					uint3 groupThreadID    : SV_GroupThreadID)
{
	uint groupIndex = groupThreadID.y * COMPUTE_SHADER_TILE_GROUP_DIM + groupThreadID.x;
	uint2 globalCoords = dispatchThreadID.xy;
	float2 sampleCoords = float2(globalCoords.xy) / float2(gSourceDimensions.xy);
	float2 clipCoords = sampleCoords * 2.0f - 1.0f;

	float3 right = cross(gUp, gForward);

	float3 normal = gForward;
	normal += clipCoords.x * right - clipCoords.y * gUp;
	normal = normalize(normal);

	float3 color = PrefilterEnvMap(EnvironmentMap, EnvSampler, MipLevelToRoughness(gCurrentMip, gMipCount), normal);

	if (globalCoords.x < uint(gSourceDimensions.x) && globalCoords.y < uint(gSourceDimensions.y))
		OutputTexture[uint3(globalCoords, 0)] = float4(color, 1.0f);
}