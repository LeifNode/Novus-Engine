//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

TextureCube  gCubemap : register (t0);
SamplerState gSampler : register (s0);

struct VS_INPUT
{
	float3 PositionL : POSITION;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float3 PositionL : POSITION;
};

PS_INPUT VS(VS_INPUT vin)
{
	PS_INPUT output;

	output.PositionH = mul(gWorldViewProj, vin.PositionL).xyww;
	output.PositionL = vin.PositionL;

	return output;
}

PS_GBUFFER_OUT PS(PS_INPUT pin)
{
	return PackGBuffer(float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, pow(gCubemap.Sample(gSampler, pin.PositionL).rgb, 2.2f));
}