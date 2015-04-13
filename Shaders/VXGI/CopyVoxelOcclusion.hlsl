//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

RWTexture3D<uint> gInput              : register(u0);
RWTexture3D<uint> gOutput             : register(u1);

[numthreads(8, 8, 8)] //512 threads per group
void CopyVoxelOcclusionCS(uint3 dispatchThreadID : SV_DispatchThreadID)
{	
	float4 color = float4(0.0f, 0.0f, 0.0f, toFloat4(gInput[dispatchThreadID]).a);//Copy only the occlusion

	gOutput[dispatchThreadID] = toUint(color);
}