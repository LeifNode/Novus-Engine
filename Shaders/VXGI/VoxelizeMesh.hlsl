//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Utils/ConstantBuffers.hlsl"

RWTexture3D<float4> gVoxelVolume   : register(u0);

Texture2D<float4> gDiffuseTexture  : register(t0);

SamplerState gSamplerState         : register(s0);

struct VS_INPUT
{
	float3 PosL  : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float2 Tex : TEXCOORD;
};

struct GS_INPUT
{
	float3 PosL  : POSITION;
	float3 Normal: NORMAL;
	float2 Tex   : TEXCOORD;
};

struct PS_INPUT
{
	float4 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
	float2 Tex   : TEXCOORD;
};

GS_INPUT VS(VS_INPUT vin)
{
	GS_INPUT vout;

	vout.PosL = vin.PosL;
	//vout.PosH = (vout.PosH + 1.0f) * 0.5f;

	vout.Normal = vin.Normal;

	vout.Tex = vin.Tex;
	vout.Tex.y = 1.0f - vout.Tex.y;

	return vout;
}

[maxvertexcount(16)]
void GS(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	float3 normal = normalize(input[0].Normal + input[1].Normal + input[2].Normal);

		////X axis
		//float4x4 axisProjections[] = {
		//	float4x4(gWorldViewProj._m00, 0.0f, 0.0f, 0.0f,
		//			 0.0f, gWorldViewProj._m11, 0.0f, 0.0f,
		//			 0.0f, 0.0f, gWorldViewProj._m22, gWorldViewProj._m23,
		//			 0.0f, 0.0f, 0.0f, 1.0f),

		//	float4x4(0.0f, gWorldViewProj._m00, 0.0f, 0.0f,
		//			 gWorldViewProj._m11, 0.0f, 0.0f, 0.0f,
		//			 0.0f, 0.0f, gWorldViewProj._m22, gWorldViewProj._m23,
		//			 0.0f, 0.0f, 0.0f, 1.0f),

		//	float4x4(gWorldViewProj._m00, 0.0f, 0.0f, 0.0f,
		//			 0.0f, gWorldViewProj._m11, 0.0f, 0.0f,
		//			 0.0f, 0.0f, gWorldViewProj._m22, gWorldViewProj._m23,
		//			 0.0f, 0.0f, 0.0f, 1.0f)
		//};

	float axis[] = {
		abs(dot(normal, float3(1.0f, 0.0f, 0.0f))),
		abs(dot(normal, float3(0.0f, 1.0f, 0.0f))),
		abs(dot(normal, float3(0.0f, 0.0f, 1.0f))),
	};


	//Choose dominant axis
	int index = 0;

	[unroll]
	for (int i = 1; i < 3; i++)
	{
		[flatten]
		if (axis[i] > axis[i - 1])
			index = i;
	}

	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		switch (index)
		{
		case 0:
			output.PosH = mul(gWorldViewProj, float4(input[i].PosL.yzx, 1.0f));
			break;
		case 1:
			output.PosH = mul(gWorldViewProj, float4(input[i].PosL.xzy, 1.0f));
			break;
		case 2:
			output.PosH = mul(gWorldViewProj, float4(input[i].PosL.xyz, 1.0f));
			break;
		}

		output.Tex = input[i].Tex;
		output.PosW = mul(gWorldViewProj, float4(input[i].PosL, 1.0f));

		triStream.Append(output);
	}
}

void PS(PS_INPUT pin)
{
	float4 diffuseColor = gMaterial.Diffuse;

	[flatten]
	if (gMaterial.HasDiffuseTexture)
		diffuseColor = gDiffuseTexture.Sample(gSamplerState, pin.Tex);

	int3 texDimensions;
	gVoxelVolume.GetDimensions(texDimensions.x, texDimensions.y, texDimensions.z);

	uint3 texIndex = uint3(((pin.PosW.x * 0.5) + 0.5f) * texDimensions.x, (1.0f - pin.PosW.y) * texDimensions.y * 0.5, pin.PosW.z * texDimensions.z);

	gVoxelVolume[texIndex] = diffuseColor;
	//imageAtomicRGBA8Avg(gVoxelVolume, texIndex, diffuseColor);
}



//float4 convRGBA8ToVec4(uint val)
//{
//	return float4(float((val & 0x000000FF)), float((val & 0x0000FF00) >> 8U), float((val & 0x00FF0000) >> 16U), float((val & 0xFF000000) >> 24U));
//}
//
//uint convVec4ToRGBA8(float4 val)
//{
//	return (uint (val.w) & 0x000000FF) << 24U | (uint(val.z) & 0x000000FF) << 16U | (uint(val.y) & 0x000000FF) << 8U | (uint(val.x) & 0x000000FF);
//}
//
////Averages the color stored in a specific texel
//void imageAtomicRGBA8Avg(RWTexture3D<uint> imgUI, uint3 coords, float4 val)
//{
//	val.rgb *= 255.0f; // Optimize following calculations
//	uint newVal = convVec4ToRGBA8(val);
//	uint prevStoredVal = 0;
//	uint currStoredVal;
//
//	InterlockedCompareExchange(imgUI[coords], prevStoredVal, newVal, currStoredVal);
//
//	// Loop as long as destination value gets changed by other threads
//	[allow_uav_condition]
//	while (currStoredVal != prevStoredVal)
//	{
//		InterlockedCompareExchange(imgUI[coords], prevStoredVal, newVal, currStoredVal);
//		prevStoredVal = currStoredVal;
//		float4 rval = convRGBA8ToVec4(currStoredVal);
//			rval.xyz = (rval.xyz* rval.w); // Denormalize
//		float4 curValF = rval + val; // Add new value
//			curValF.xyz /= (curValF.w); // Renormalize
//		newVal = convVec4ToRGBA8(curValF);
//	}
//}