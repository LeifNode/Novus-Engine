//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//#include "Common.hlsl"
#include "Utils/FullscreenTriangle.hlsl"

Texture2D<float4> textureDiffuse : register(t0);
SamplerState texSampler : register(s0);

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
}

TRIANGLE_OUT VS(uint vertexID : SV_VertexID)
{
	return FullscreenTriangle(vertexID);
}

float4 PS(TRIANGLE_OUT pin) : SV_Target
{
	/*float3 texColor = textureDiffuse.Sample(texSampler, pin.TexCoord).rgb;
	texColor *= 0.1;

	float ExposureBias = 1.0f;
	float3 curr = Uncharted2Tonemap(ExposureBias * texColor);

	float3 whiteScale = 1.0f / Uncharted2Tonemap(W);

	return float4(pow(curr * whiteScale, 1.0f / 2.2f), 1.0f);*/

	float3 texColor = textureDiffuse.Sample(texSampler, pin.TexCoord).rgb;
	texColor *= 1;

	float3 x = max(0, texColor - 0.004);
	float3 retColor = (x*(6.2*x + .5)) / (x*(6.2*x + 1.7) + 0.06);

	return float4(retColor, 1.0f);
}
