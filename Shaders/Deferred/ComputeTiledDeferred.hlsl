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
	if (surface.PositionView.z >= gClipNearFar.x &&
		surface.PositionView.z < gClipNearFar.y)
	{
		minZSample = min(minZSample, surface.PositionView.z);
		maxZSample = max(maxZSample, surface.PositionView.z);
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

	float4 c1 = float4(gProjection._11 * tileScale.x, 0.0f, gProjection._13 * tileScale.x + tileBias.x, 0.0f);
	float4 c2 = float4(0.0f, -gProjection._22 * tileScale.y, gProjection._23 * tileScale.y + tileBias.y, 0.0f);
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
			inFrustum = inFrustum && (d >= -light.Range);
		}

		[branch]
		if (inFrustum && light.Intensity > 0.0f)
		{
			uint listIndex;
			InterlockedAdd(TileNumLights, 1, listIndex);
			TileLightIndices[listIndex] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	uint numLights = TileNumLights;

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float3 toEye = normalize(gEyePosition - surface.PositionWorld);

	if (all(globalCoords < gScreenDimensions.xy))
	{
		if (numLights > 0)
		{
			for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex)
			{
				AccumulateBRDF(surface, gPointLights[TileLightIndices[tileLightIndex]], toEye, finalColor);
			}
		}
	}

	finalColor += surface.Emissive;

	float3 specularLighting = ApproximateSpecularIBL(EnvironmentProbe, EnvironmentSampler, BRDF_LUT, 11, lerp(float3(0.0f, 0.0f, 0.0f), surface.SpecularColor, surface.Metallic), surface.Roughness, surface.Normal, toEye);
	//float3 specularLighting = SpecularIBL(EnvironmentProbe, EnvironmentSampler, lerp(float3(0.0f, 0.0f, 0.0f), surface.SpecularColor, surface.Metallic), surface.Roughness, surface.Normal, toEye);

	[flatten]
	if (surface.PositionView.z < gClipNearFar.y - 1000.0)
		finalColor += specularLighting;
	
	OutputTexture[globalCoords] = float4(finalColor, 1.0f);

	//OutputTexture[globalCoords] = float4(sampleColor + surface.Emissive, 1.0f);

	//OutputTexture[globalCoords] = float4((float(TileNumLights) / 1028.0f).xxx, 1.0f);

	//OutputTexture[globalCoords] = float(maxTileZ).xxxx / 20.0f;

	//OutputTexture[globalCoords] = float4(surface.Normal.xyz, 1.0f);

	//OutputTexture[globalCoords] = float4(surface.PositionWorld.xyz / 10.0f, 1.0f);

	//OutputTexture[globalCoords] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}