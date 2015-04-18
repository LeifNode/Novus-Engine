//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

Texture3D<float4> gInputDiffuse       : register(t0);
Texture3D<float4> gInputEmissive      : register(t1);
//RWTexture3D<uint> gInputDiffuse
RWTexture3D<uint> gOutput             : register(u0);

[numthreads(8, 8, 8)] //512 threads per group
void CopyVoxelOcclusionCS(uint3 dispatchThreadID : SV_DispatchThreadID)
{	
	float4 color = float4(gInputEmissive.Load(int4(dispatchThreadID, 0)).rgb, gInputDiffuse.Load(int4(dispatchThreadID, 0)).a);//Copy only the occlusion

	gOutput[dispatchThreadID] = toUint(color);
}