//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

RWTexture3D<uint> gInput              : register(u0);
RWTexture3D<uint> gOutputMip          : register(u1);

//Filters 6 primary directions of light from base volume and stores in first mip
[numthreads(8, 8, 8)] //512 threads per group
void VoxelVolumeMipMap(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	const int3 lowerMipIndex = dispatchThreadID * 2;
	int3 sampleIndex;
	uint arrIndex;

	uint3 destinationIndex = dispatchThreadID;

	float4 sampledColors[8] = {
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	float4 colorArr[8] = {
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	int x, y, z;

	//Get texture samples
	[unroll]
	for (x = 0; x < 2; x++)
	{
		[unroll]
		for (y = 0; y < 2; y++)
		{
			[unroll]
			for (z = 0; z < 2; z++)
			{
				sampleIndex = lowerMipIndex + int3(x, y, z);
				sampledColors[x + y * 2 + z * 4] = toFloat4(gInput[sampleIndex]);
			}
		}
	}

	//+X
	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[x * 4 + y * 2 + z] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//-X
	destinationIndex.x += gVoxelVolumeDim.x;

	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[4 - x * 4 + y * 2 + z] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//+Y
	destinationIndex.x += gVoxelVolumeDim.x;

	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[x + y * 4 + z * 2] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//-Y
	destinationIndex.x += gVoxelVolumeDim.x;

	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[x + 4 - y * 4 + z * 2] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//+Z
	destinationIndex.x += gVoxelVolumeDim.x;

	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[x + y * 2 + z * 4] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);
	
	//-Z
	destinationIndex.x += gVoxelVolumeDim.x;

	[unroll]
	for (x = 0; x <= 1; x++)
	{
		[unroll]
		for (y = 0; y <= 1; y++)
		{
			[unroll]
			for (z = 0; z <= 1; z++)
			{
				arrIndex = x + y * 2 + z * 4;
				colorArr[x + y * 2 + 4 - z * 4] = sampledColors[arrIndex];
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirectionBase(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);
}