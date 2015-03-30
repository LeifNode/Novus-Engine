//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

Texture2D<float4> gDiffuseTexture   : register(t0);

SamplerState gSamplerState          : register(s0);

struct VS_INPUT
{
	float3 PosL  : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 PosH  : SV_POSITION;
	float2 Tex   : TEXCOORD;
};

PS_INPUT VS(VS_INPUT vin)
{
	PS_INPUT vout;

	vout.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));

	vout.Tex = vin.Tex;
	vout.Tex.y = 1.0f - vout.Tex.y;

	return vout;
}

void PS(PS_INPUT pin)
{
	float4 diffuseColor = gMaterial.Diffuse;

	if (gMaterial.HasDiffuseTexture)
		diffuseColor = gDiffuseTexture.Sample(gSamplerState, pin.Tex);

	clip(diffuseColor.a - 0.15f);
}