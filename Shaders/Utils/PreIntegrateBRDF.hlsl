//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "Lighting/Shading.hlsl"

RWTexture2D<float2> OutputTexture  : register(u0);

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]
void PreIntegrateBRDFCS(uint3 groupID          : SV_GroupID,
					  uint3 dispatchThreadID : SV_DispatchThreadID,
					  uint3 groupThreadID : SV_GroupThreadID)
{
	uint2 sourceDimensions = 0;
	OutputTexture.GetDimensions(sourceDimensions.x, sourceDimensions.y);

	uint groupIndex = groupThreadID.y * COMPUTE_SHADER_TILE_GROUP_DIM + groupThreadID.x;
	uint2 globalCoords = dispatchThreadID.xy;
	float2 sampleCoords = float2(globalCoords.xy) / float2(sourceDimensions.xy);

	float2 color = IntegrateBRDF(sampleCoords.x, sampleCoords.y);

	if (globalCoords.x < uint(sourceDimensions.x) && globalCoords.y < uint(sourceDimensions.y))
		OutputTexture[globalCoords] = color;

	//OutputTexture[globalCoords] = float2(1.0, 0.0);
}