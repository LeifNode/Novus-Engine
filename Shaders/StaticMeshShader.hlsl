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

	float3 norm = normalize(vin.Normal);
	float3 tangent = normalize(vin.Tangent);

	vout.Normal = mul((float3x3)gWorldInvTranspose, norm);
	vout.Tangent = mul((float3x3)gWorldInvTranspose, tangent);
	vout.Bitangent = mul((float3x3)gWorldInvTranspose, cross(norm, tangent));

	//vout.Tex = mul(gTextureTransform, float4(vin.Tex, 0.0f, 1.0f)).xy;
	vout.Tex = vin.Tex;

	return vout;
}

//float4 PS(PS_INPUT pin) : SV_Target
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f) * dot(normalize(float3(1.0f, 1.0f, 1.0f)), pin.Normal);
//}

PS_GBUFFER_OUT PS(PS_INPUT pin)
{
	float4 diffuseColor = gMaterial.Diffuse;

	[flatten]
	if (gMaterial.HasDiffuseTexture)
		diffuseColor = gDiffuseTexture.Sample(gSamplerState, pin.Tex);

	return PackGBuffer(diffuseColor, normalize(pin.Normal), gMaterial.SpecularColor, gMaterial.Roughness, gMaterial.Metallic, gMaterial.Emissive);
}