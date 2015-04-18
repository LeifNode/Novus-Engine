//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Utils/ConstantBuffers.hlsl"
#include "VXGI/VoxelGICommon.hlsl"

RWTexture3D<uint> gVoxelVolume   : register(u0);
RWTexture3D<uint> gVoxelNormals  : register(u1);
RWTexture3D<uint> gVoxelEmissive : register(u2);

Texture2D<float4> gDiffuseTexture  : register(t0);
//Texture2D<float4> gNormalTexture   : register(t1);

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
	//float3 Tangent : TANGENT;
	float2 Tex   : TEXCOORD;
};

struct PS_INPUT
{
	float4 PosH  : SV_POSITION;
	float3 PosW  : POSITION;
	float3 Normal: NORMAL;
	//float3 Tangent : TANGENT;
	//float3 Bitangent: BITANGENT;
	float2 Tex   : TEXCOORD;
};

//Averages the color stored in a specific texel
void imageAtomicRGBA8Avg(RWTexture3D<uint> imgUI, uint3 coords, float4 val)
{
	val.rgb *= 255.0f; // Optimize following calculations
	uint newVal = convVec4ToRGBA8(val);
	uint prevStoredVal = 0;
	uint curStoredVal = 0;

	// Loop as long as destination value gets changed by other threads
	
	[allow_uav_condition] do //While loop does not work and crashes the graphics driver, but do while loop that does the same works; compiler error?
	{
		InterlockedCompareExchange(imgUI[coords], prevStoredVal, newVal, curStoredVal);

		if (curStoredVal == prevStoredVal)
			break;

		prevStoredVal = curStoredVal;
		float4 rval = convRGBA8ToVec4(curStoredVal);
			rval.xyz = (rval.xyz* rval.w); // Denormalize
		float4 curValF = rval + val; // Add new value
			curValF.xyz /= (curValF.w); // Renormalize
		newVal = convVec4ToRGBA8(curValF); 
		
		
	} while (true);
}

GS_INPUT VS(VS_INPUT vin)
{
	GS_INPUT vout;

	vout.PosL = vin.PosL;

	vout.Normal = mul((float3x3)gWorldInvTranspose, vin.Normal);
	//vout.Tangent = mul((float3x3)gWorldInvTranspose, vin.Tangent);

	vout.Tex = vin.Tex;
	vout.Tex.y = 1.0f - vout.Tex.y;

	return vout;
}

PS_INPUT initOutput()
{
	PS_INPUT input;

	input.PosH = float4(0.0f, 0.0f, 0.0f, 0.0f);
	input.PosW = float3(0.0f, 0.0f, 0.0f);
	input.Tex = float2(0.0f, 0.0f);

	return input;
}

[maxvertexcount(3)]
void GS(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;
	float4 outputPosH[3] = { float4(0.0f, 0.0f, 0.0f, 0.0f), 
							 float4(0.0f, 0.0f, 0.0f, 0.0f), 
							 float4(0.0f, 0.0f, 0.0f, 0.0f) };

	const float2 halfPixel = float2(1.0f, 1.0f) / 512.0f;

	float3 normal = cross(normalize(input[1].PosL - input[0].PosL), normalize(input[2].PosL - input[0].PosL)); //Get face normal

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


	//Find dominant axis
	int index = 0;
	int i = 0;

	[unroll]
	for (i = 1; i < 3; i++)
	{
		[flatten]
		if (axis[i] > axis[i - 1])
			index = i;
	}

	[unroll]
	for (i = 0; i < 3; i++)
	{
		float4 inputPosL;

		[flatten]
		switch (index)
		{
		case 0:
			inputPosL = float4(input[i].PosL.yzx, 1.0f);
			break;
		case 1:
			inputPosL = float4(input[i].PosL.zxy, 1.0f);
			break;
		case 2:
			inputPosL = float4(input[i].PosL.xyz, 1.0f);
			break;
		}

		outputPosH[i] = mul(gWorldViewProj, inputPosL);

		output.Tex = input[i].Tex;
		output.PosW = mul(gWorldView, float4(input[i].PosL, 1.0f)).xyz;
		output.PosH = outputPosH[i];
		output.Normal = input[i].Normal;
		//output.Tangent = input[i].Tangent;
		//output.Bitangent = cross(output.Normal, output.Tangent);

		triStream.Append(output);
	}

	/*float3 plane[2];
	float4 finalPos = float(0.0f).xxxx;

	[unroll]
	for (i = 0; i < 3; i++)
	{
		[flatten]
		const int last = (i - 1 < 0 ? 2 : i - 1);
		const int next = (i + 1 > 2 ? 0 : i + 1);

		plane[0] = cross(outputPosH[i].xyw - outputPosH[last].xyw, outputPosH[last].xyw);
		plane[1] = cross(outputPosH[next].xyw - outputPosH[i].xyw, outputPosH[i].xyw);

		plane[0].z -= dot(halfPixel.xy, abs(plane[0].xy));
		plane[1].z -= dot(halfPixel.xy, abs(plane[1].xy));

		finalPos.xyw = cross(plane[0], plane[1]);

		triStream.Append(output);
	}*/
}

void PS(PS_INPUT pin)
{
	float4 diffuseColor = gMaterial.Diffuse;
	float3 emissiveColor = gMaterial.Emissive;

	pin.Normal = normalize(pin.Normal);

	[flatten]
	if (gMaterial.HasDiffuseTexture)
		diffuseColor = pow(gDiffuseTexture.Sample(gSamplerState, pin.Tex), 2.2);

	/*[branch]
	if (gMaterial.HasNormalTexture)
	{
		pin.Tangent = normalize(pin.Tangent);
		pin.Bitangent = normalize(pin.Bitangent);

		float4 bumpSample = gNormalTexture.Sample(gSamplerState, pin.Tex);
		bumpSample = (bumpSample * 2.0f) - 1.0f;

		pin.Normal += bumpSample.x * pin.Tangent + bumpSample.y * pin.Bitangent;
		pin.Normal = normalize(pin.Normal);
	}*/

	pin.Normal = pin.Normal * 0.5f + 0.5f;

	int3 texDimensions;
	gVoxelVolume.GetDimensions(texDimensions.x, texDimensions.y, texDimensions.z);

	uint3 texIndex = uint3(((pin.PosW.x * 0.5) + 0.5f) * texDimensions.x, 
						   ((pin.PosW.y * 0.5) + 0.5f) * texDimensions.y, 
						   ((pin.PosW.z * 0.5) + 0.5f) * texDimensions.z);

	float4 normal = float4(pin.Normal, 1.0f);

	if (all(texIndex < texDimensions.xyz) && all(texIndex >= 0))
	{
		//gVoxelNormals[texIndex] = convVec4ToRGBA8(float4(pin.Normal * 255.0f, 1.0f));

		//imageAtomicRGBA8Avg(gVoxelVolume, texIndex, diffuseColor); //Two of these in sequence always hangs the graphics driver

		gVoxelVolume[texIndex] = convVec4ToRGBA8(diffuseColor * 255.0f);
		gVoxelEmissive[texIndex] = convVec4ToRGBA8(float4(emissiveColor.rgb, 1.0f) * 255.0f);
		imageAtomicRGBA8Avg(gVoxelNormals, texIndex, normal);
	}
}