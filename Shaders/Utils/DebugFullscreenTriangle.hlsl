//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"
#include "Utils/FullscreenTriangle.hlsl"

Texture2D<float4> textureDiffuse : register(t0);
SamplerState texSampler : register(s0);

TRIANGLE_OUT VS(uint vertexID : SV_VertexID)
{
	return FullscreenTriangle(vertexID);
}

float4 PS(TRIANGLE_OUT pin) : SV_Target
{
	//return float4(pin.TexCoord.xy, 0.0f, 1.0f);
	return float4(pow(textureDiffuse.Sample(texSampler, pin.TexCoord), 1.0f / 2.2f));
}
