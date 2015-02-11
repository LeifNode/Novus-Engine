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

	vout.PosW = mul(gWorld, float4(vin.PosL, 1.0f)).xyz;

	vout.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));

	vout.Normal = mul((float3x3)gWorldInvTranspose, normalize(vin.Normal));
	vout.Tangent = mul((float3x3)gWorldInvTranspose, normalize(vin.Tangent));
	vout.Bitangent = mul((float3x3)gWorldInvTranspose, normalize(vin.Bitangent));

	vout.Tex = mul(gTextureTransform, float4(vin.Tex, 0.0f, 1.0f)).xy;

	return vout;
}


//float4 PS(PS_INPUT pin) : SV_Target
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f) * dot(normalize(float3(1.0f, 1.0f, 1.0f)), pin.Normal);
//}

PS_GBUFFER_OUT PS(PS_INPUT pin)
{
	return PackGBuffer(gMaterial.Diffuse, normalize(pin.Normal), gMaterial.SpecularColor, gMaterial.Roughness, gMaterial.Metallic, gMaterial.Emissive);
}