//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================


#ifndef VXGI_COMMON_HLSL
#define VXGI_COMMON_HLSL

#include "Utils/Math.hlsl"

static const float cMipDirectionalOffsets[] = {
	0.0f,
	1.0f / 6.0f,
	2.0f / 6.0f,
	3.0f / 6.0f,
	4.0f / 6.0f,
	5.0f / 6.0f
};

cbuffer cbVoxelGI : register(b2)
{
	float4x4 gWorldToVoxelVolume;

	int3 gVoxelVolumeDim;
	int gVoxelMipCount;

	float3 vgiPad;
	float gVoxelScale; //Size of one voxel in meters
};

float4 convRGBA8ToVec4(uint val)
{
	return float4(float((val & 0x000000FF)), float((val & 0x0000FF00) >> 8U), float((val & 0x00FF0000) >> 16U), float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(float4 val)
{
	return (uint (val.w) & 0x000000FF) << 24U | (uint(val.z) & 0x000000FF) << 16U | (uint(val.y) & 0x000000FF) << 8U | (uint(val.x) & 0x000000FF);
}

uint toUint(float4 value)
{
	value *= 255.0f;
	return (uint (value.w) & 0x000000FF) << 24U | (uint(value.z) & 0x000000FF) << 16U | (uint(value.y) & 0x000000FF) << 8U | (uint(value.x) & 0x000000FF);
}

float4 toFloat4(uint value)
{
	return float4(float((value & 0x000000FF)), float((value & 0x0000FF00) >> 8U), float((value & 0x00FF0000) >> 16U), float((value & 0xFF000000) >> 24U)) / 255.0f;
}

//Based off of GGX roughness; gets angle that encompasses 90% of samples in the IBL image approximation
float calculateSpecularConeHalfAngle(float roughness2)
{
	return acos(sqrt(0.11111f / (roughness2 * roughness2 + 0.11111f)));
}

//float getMipLevelFromRadius(float radius, float voxelScale, float topMip)
//{
//	return log2(radius * voxelScale) + topMip;
//}

float3 worldToVoxelVolume(float3 worldPosition)
{
	return mul(gWorldToVoxelVolume, float4(worldPosition, 1.0f)).xyz * 0.5f + 0.5f;
}

float getMipLevelFromRadius(float radius)
{
	return log2((radius + 0.01f) * gVoxelScale) + gVoxelMipCount;
}

float4 sampleVoxelVolume(Texture3D<float4> voxelTexture, SamplerState voxelSampler, float3 worldPosition, float radius)
{
	float3 voxelPos = worldToVoxelVolume(worldPosition);
	float mipLevel = getMipLevelFromRadius(radius);

	return voxelTexture.SampleLevel(voxelSampler, voxelPos, mipLevel);
}	

float4 sampleVoxelVolumeAnisotropic(Texture3D<float4> voxelTexture, Texture3D<float4> voxelMips, SamplerState voxelSampler, float3 worldPosition, float radius, float3 direction)
{
	//direction = -direction;
	uint3 isNegative = (direction < 0.0f);
	float3 dirSq = direction * direction;

	float3 voxelPos = worldToVoxelVolume(worldPosition);

	float mipLevel = getMipLevelFromRadius(radius);
	float anisotropicMipLevel = mipLevel - 1.0f;

	//float4 mip0Sample = voxelTexture.SampleLevel(voxelSampler, voxelPos, mipLevel);

	voxelPos.x /= 6.0f;

	float4 xSample = voxelMips.SampleLevel(voxelSampler, voxelPos + float3(cMipDirectionalOffsets[isNegative.x], 0.0f, 0.0f), anisotropicMipLevel);
	float4 ySample = voxelMips.SampleLevel(voxelSampler, voxelPos + float3(cMipDirectionalOffsets[isNegative.y + 2], 0.0f, 0.0f), anisotropicMipLevel);
	float4 zSample = voxelMips.SampleLevel(voxelSampler, voxelPos + float3(cMipDirectionalOffsets[isNegative.z + 4], 0.0f, 0.0f), anisotropicMipLevel);

	float4 filteredColor = dirSq.x * xSample + dirSq.y * ySample + dirSq.z * zSample;

	//filteredColor = lerp(mip0Sample, filteredColor, clamp(mipLevel, 0.0f, 1.0f)); //This kills the performance
	filteredColor = filteredColor;
	//filteredColor.a = mip0Sample.a;

	filteredColor.rgb *= 11.0f;

	return filteredColor;
}

void accumilateColorOcclusion(float4 sampleColor, inout float3 colorAccum, inout float occlusionAccum)
{
	colorAccum = occlusionAccum * colorAccum + (1.0f - sampleColor.a) * sampleColor.a * sampleColor.rgb;
	occlusionAccum = occlusionAccum + (1.0f - occlusionAccum) * sampleColor.a;
}

//float4 filterAnsiotropicVoxelDirection(float4 f1, float4 f2, float4 f3, float4 f4, float4 b1, float4 b2, float4 b3, float4 b4)
//{
//	const float4 frontVoxels[] = {
//		f1,
//		f2,
//		f3,
//		f4,
//	};
//
//	const float4 backVoxels[] = {
//		b1,
//		b2,
//		b3,
//		b4,
//	};
//
//	float4 directionalAccum[] = {
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//		float4(0.0f, 0.0f, 0.0f, 0.0f),
//	};
//
//	int i = 0;
//
//	[unroll]
//	for (i = 0; i < 4; i++)
//	{
//		directionalAccum[i] = frontVoxels[i];
//	}
//
//	/*[unroll]
//	for (i = 0; i < 4; i++)
//	{
//		accumilateColorOcclusion(frontVoxels[i], directionalAccum[i].rgb, directionalAccum[i].a);
//	}
//*/
//	float totalOcclusion = 0.0f;
//
//	[unroll]
//	for (i = 0; i < 4; i++)
//	{
//		accumilateColorOcclusion(backVoxels[i], directionalAccum[i].rgb, directionalAccum[i].a);
//
//		totalOcclusion += directionalAccum[i].a;
//	}
//
//	/*float4 finalAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	[unroll]
//	for (i = 0; i < 4; i++)
//	{
//		finalAccum.rgb += directionalAccum[i].rgb * (directionalAccum[i].a / totalOcclusion);
//	}
//
//	finalAccum.a = totalOcclusion * 0.25f;
//
//	return finalAccum;*/
//
//	return (directionalAccum[0] + directionalAccum[1] + directionalAccum[2] + directionalAccum[3]) * 0.25f;
//}

float4 filterAnsiotropicVoxelDirection(float4 f1, float4 f2, float4 f3, float4 f4, float4 b1, float4 b2, float4 b3, float4 b4)
{
	/*const float4 frontVoxels[] = {
		f1,
		f2,
		f3,
		f4,
	};

	const float4 backVoxels[] = {
		b1,
		b2,
		b3,
		b4,
	};
*/
	float4 directionalAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);

	/*int i = 0;
	
	[unroll]
	for (i = 0; i < 4; i++)
	{
		directionalAccum.rgb += (frontVoxels[i].rgb + backVoxels[i].rgb * (1.0f - frontVoxels[i].a)) * 0.25f;
		directionalAccum.a += min(1.0f, frontVoxels[i].a + backVoxels[i].a) * 0.25f;
	}*/

	directionalAccum.rgb += (f1.rgb + b1.rgb * (1.0f - f1.a)) * 0.25f;
	directionalAccum.a += min(1.0f, f1.a + b1.a) * 0.25f;

	directionalAccum.rgb += (f2.rgb + b2.rgb * (1.0f - f2.a)) * 0.25f;
	directionalAccum.a += min(1.0f, f2.a + b2.a) * 0.25f;

	directionalAccum.rgb += (f3.rgb + b3.rgb * (1.0f - f3.a)) * 0.25f;
	directionalAccum.a += min(1.0f, f3.a + b3.a) * 0.25f;

	directionalAccum.rgb += (f4.rgb + b4.rgb * (1.0f - f4.a)) * 0.25f;
	directionalAccum.a += min(1.0f, f4.a + b4.a) * 0.25f;

	return directionalAccum;
}

float4 filterAnsiotropicVoxelDirectionBase(float4 f1, float4 f2, float4 f3, float4 f4, float4 b1, float4 b2, float4 b3, float4 b4)
{
	return filterAnsiotropicVoxelDirection(f1, f2, f3, f4, b1, b2, b3, b4);
}

static const float3 cVXGIConeSampleDirections[] =
{
	float3(0.0f, 1.0f, 0.0f),
	float3(0.0f, 0.5f, 0.866025f),
	float3(0.823639f, 0.5f, 0.267617f),
	float3(0.509037f, 0.5f, -0.7006629f),
	float3(-0.50937f, 0.5f, -0.7006629f),
	float3(-0.823639f, 0.5f, 0.267617f)
};

static const float cVXGIConeSampleWeights[] =
{
	PI / 4.0f,
	3 * PI / 20.0f,
	3 * PI / 20.0f,
	3 * PI / 20.0f,
	3 * PI / 20.0f,
	3 * PI / 20.0f,
};

#endif