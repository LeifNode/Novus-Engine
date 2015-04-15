//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

RWTexture3D<uint> gInputMip           : register(u0);
RWTexture3D<uint> gOutputMip          : register(u1);

static const float cGaussianWeights[] =
{
	//Top slice
	1 / 64.0f,
	1 / 32.0f,
	1 / 64.0f,
	1 / 32.0f,
	1 / 16.0f,
	1 / 32.0f,
	1 / 64.0f,
	1 / 32.0f,
	1 / 64.0f,

	//Center slice
	1 / 32.0f,
	1 / 16.0f,
	1 / 32.0f,
	1 / 16.0f,
	1 / 4.0f,
	1 / 16.0f,
	1 / 32.0f,
	1 / 16.0f,
	1 / 32.0f,

	//Bottom slice
	1 / 64.0f,
	1 / 32.0f,
	1 / 64.0f,
	1 / 32.0f,
	1 / 16.0f,
	1 / 32.0f,
	1 / 64.0f,
	1 / 32.0f,
	1 / 64.0f,
};

//[numthreads(8, 8, 8)] //512 threads per group
//void VoxelVolumeMipMap(uint3 dispatchThreadID : SV_DispatchThreadID)
//{
//	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//		int3 lowerMipIndex = dispatchThreadID * 2;
//		int3 sampleIndex;
//
//	[unroll]
//	for (int x = -1; x <= 1; x++)
//	{
//		[unroll]
//		for (int y = -1; y <= 1; y++)
//		{
//			[unroll]
//			for (int z = -1; z <= 1; z++)
//			{
//				sampleIndex = lowerMipIndex + int3(x, y, z); //Should probably offset by 0.5 and use a linear sampler
//				finalColor += toFloat4(gInputMip[sampleIndex]) * cGaussianWeights[x + 1 + (y + 1) * 3 + (z + 1) * 9];
//			}
//		}
//	}
//
//	gOutputMip[dispatchThreadID] = toUint(finalColor);
//}

//[numthreads(8, 8, 8)] //512 threads per group
//void VoxelVolumeMipMap(uint3 dispatchThreadID : SV_DispatchThreadID)
//{
//	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	int3 lowerMipIndex = dispatchThreadID * 2;
//	int3 sampleIndex;
//
//	float4 colorArr[8] = {
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f)
//	};
//
//	[unroll]
//	for (int x = 0; x <= 1; x++)
//	{
//		[unroll]
//		for (int y = 0; y <= 1; y++)
//		{
//			[unroll]
//			for (int z = 0; z <= 1; z++)
//			{
//				sampleIndex = lowerMipIndex + int3(x, y, z); //Should probably offset by 0.5 and use a linear sampler
//				colorArr[x + y * 2 + z * 4] = toFloat4(gInputMip[sampleIndex]);
//			}
//		}
//	}
//
//	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);
//
//	gOutputMip[dispatchThreadID] = toUint(finalColor);
//}

[numthreads(8, 8, 8)] //512 threads per group
void VoxelVolumeMipMap(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int3 lowerMipIndex = dispatchThreadID * 2;
	int3 sampleIndex;

	const int incrementDist = gVoxelVolumeDim.x * 2;
	uint3 destinationIndex = dispatchThreadID;

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

	//+X
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
				colorArr[x * 4 + y * 2 + z] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//-X
	destinationIndex.x += gVoxelVolumeDim.x;
	lowerMipIndex.x += incrementDist;

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
				colorArr[4 - x * 4 + y * 2 + z] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//+Y
	destinationIndex.x += gVoxelVolumeDim.x;
	lowerMipIndex.x += incrementDist;

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
				colorArr[x + y * 4 + z * 2] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//-Y
	destinationIndex.x += gVoxelVolumeDim.x;
	lowerMipIndex.x += incrementDist;

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
				colorArr[x + 4 - y * 4 + z * 2] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//+Z
	destinationIndex.x += gVoxelVolumeDim.x;
	lowerMipIndex.x += incrementDist;

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
				colorArr[x + y * 2 + z * 4] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);

	//+Z
	destinationIndex.x += gVoxelVolumeDim.x;
	lowerMipIndex.x += incrementDist;

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
				colorArr[x + y * 2 + 4 - z * 4] = toFloat4(gInputMip[sampleIndex]);
			}
		}
	}

	finalColor = filterAnsiotropicVoxelDirection(colorArr[0], colorArr[1], colorArr[2], colorArr[3], colorArr[4], colorArr[5], colorArr[6], colorArr[7]);

	gOutputMip[destinationIndex] = toUint(finalColor);
}