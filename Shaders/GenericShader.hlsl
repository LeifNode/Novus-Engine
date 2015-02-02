//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

Texture2D textureDiffuse : register(t0);
Texture2D textureNormal  : register(t1);
Texture2D textureSpecular: register(t2);

SamplerState mainSampler : register(s0);

struct VS_INPUT
{
	float3 PosL  : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 PosH  : SV_POSITION;
	float3 PosW	 : POSITION;
	float3 Normal: NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent: BITANGENT;
	float2 Tex   : TEXCOORD;
};

PS_INPUT VS(VS_INPUT vin)
{
	PS_INPUT vout;

	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	vout.Normal = mul(normalize(vin.Normal), (float3x3)gWorldInvTranspose);
	vout.Tangent = mul(normalize(vin.Tangent), (float3x3)gWorldInvTranspose);
	vout.Bitangent = mul(normalize(vin.Bitangent), (float3x3)gWorldInvTranspose);

	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTextureTransform).xy;

	return vout;
}


//float4 PS(PS_INPUT pin) : SV_Target
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f) * dot(normalize(float3(1.0f, 1.0f, 1.0f)), pin.Normal);
//}

PS_GBUFFER_OUT PS(PS_INPUT pin)
{
	return PackGBuffer(float4(1.0f, 1.0f, 1.0f, 1.0f), pin.Normal, float3(1.0f, 0.0f, 0.0f), 0.5f, float3(0.0f, 0.0f, 0.0f));
}