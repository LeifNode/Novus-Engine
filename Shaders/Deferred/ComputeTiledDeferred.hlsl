//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//https://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines

#include "Common.hlsl"
#include "DeferredShading.hlsl"

groupshared uint TileMinZ;
groupshared uint TileMaxZ;

groupshared uint TileLightIndices[MAX_LIGHTS];
groupshared uint TileNumLights;

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void ComputeShaderTileCS(uint3 GroupID          :SV_GroupID,
						 uint3 DispatchThreadID : SV_DispatchThreadID,
						 uint3 GroupThreadID : SV_GroupThreadID)
{
	uint GroupIndex = GroupThreadID.y * COMPUTE_SHADER_TILE_GROUP_DIM + GroupThreadID.x;


}