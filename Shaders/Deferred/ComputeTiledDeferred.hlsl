//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//https://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines

#include "Common.hlsl"
#include "Deferred/DeferredShading.hlsl"

groupshared uint TileMinZ;
groupshared uint TileMaxZ;

groupshared uint TileLightIndices[MAX_LIGHTS];
groupshared uint TileNumLights;

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void ComputeShaderTileCS(uint3 groupID          :SV_GroupID,
						 uint3 dispatchThreadID : SV_DispatchThreadID,
						 uint3 groupThreadID : SV_GroupThreadID)
{
	uint groupIndex = groupThreadID.y * COMPUTE_SHADER_TILE_GROUP_DIM + groupThreadID.x;

	uint totalLights, unused;
	gPointLights.GetDimensions(totalLights, unused);

	uint2 globalCoords = dispatchThreadID.xy;

	SURFACE_DATA surface = UnpackGBufferViewport(globalCoords);

	float minZSample = gClipNearFar.y;
	float maxZSample = gClipNearFar.x;

	[flatten]
	if (surface.ViewPosition.z >= gClipNearFar.x &&
		surface.ViewPosition.z < gClipNearFar.y)
	{
		minZSample = min(minZSample, surface.ViewPosition.z);
		maxZSample = max(maxZSample, surface.ViewPosition.z);
	}

	//Init shared memory
	if (groupIndex == 0)
	{
		TileNumLights = 0;
		TileMinZ = 0x7F7FFFFF; //Max float
		TileMaxZ = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	//Get the min and max Z for this group
	if (maxZSample >= minZSample)
	{
		InterlockedMin(TileMinZ, asuint(minZSample));
		InterlockedMax(TileMaxZ, asuint(maxZSample));
	}

	GroupMemoryBarrierWithGroupSync();

	float minTileZ = asfloat(TileMinZ);
	float maxTileZ = asfloat(TileMaxZ);

	float2 tileScale = float2(gScreenDimensions.xy) * rcp(float(2 * COMPUTE_SHADER_TILE_GROUP_DIM));
	float2 tileBias = tileScale - float2(groupID.xy);

	float4 c1 = float4(gProjection[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
	float4 c2 = float4(0.0f, -gProjection[3][3] * tileScale.y, tileBias.y, 0.0f);
	float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

	float4 frustumPlanes[6];

	//Side planes
	frustumPlanes[0] = c4 - c1;
	frustumPlanes[1] = c4 + c1;
	frustumPlanes[2] = c4 - c2;
	frustumPlanes[3] = c4 + c2;


	frustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -minTileZ);
	frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, maxTileZ);

	//Normalize planes
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
	}

	//Find which lights are in tile
	for (uint lightIndex = groupIndex; lightIndex < totalLights; lightIndex += COMPUTE_SHADER_TILE_GROUP_SIZE)
	{
		PointLight light = gPointLights[lightIndex];

		bool inFrustum = true;
		[unroll]
		for (uint i = 0; i < 6; i++)
		{
			float d = dot(frustumPlanes[i], float4(light.PositionView, 1.0f));
			inFrustum = inFrustum && d >= light.Range;
		}

		[branch]
		if (inFrustum)
		{
			uint listIndex;
			InterlockedAdd(TileNumLights, 1, listIndex);
			TileLightIndices[listIndex] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	OutputTexture[globalCoords].rgba = (float(TileNumLights) / 255.0f).xxxx;
}