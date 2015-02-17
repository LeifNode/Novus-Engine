#include "Common.hlsl"
#include "Utils/Defines.hlsl"

Texture2D textAtlus : register(t0);

SamplerState textSampler : register(s0);

struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 TexTL : TEXCOORD0;
	float2 TexBR : TEXCOORD1;
	float2 Dimensions : DIMENSION;
};

struct GS_INPUT
{
	float3 Pos : POSITION;
	float2 TexTL : TEXCOORD0;
	float2 TexBR : TEXCOORD1;
	float2 Dimensions : DIMENSION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexUV : TEXCOORD;
	float3 Normal : NORMAL;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output = (GS_INPUT)0;

	output.Pos = input.Pos;
	output.TexTL = input.TexTL;
	output.TexBR = input.TexBR;
	output.Dimensions = input.Dimensions;

	return output;
}

[maxvertexcount(4)]
void GS(point GS_INPUT input[1], uint primId : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	output.Normal = mul(gWorldInvTranspose, float4(0.0f, 0.0f, 1.0f, 0.0f)).xyz;

	float2 texCoords[4] =
	{
		float2(input[0].TexTL.x, input[0].TexTL.y),
		float2(input[0].TexTL.x, input[0].TexBR.y),
		float2(input[0].TexBR.x, input[0].TexTL.y),
		float2(input[0].TexBR.x, input[0].TexBR.y),
	};

	float2 dimensions = input[0].Dimensions;

		float3 offsets[4] =
	{
		float3(0.0f, dimensions.y, 0.0f),
		float3(0.0f, 0.0f, 0.0f),
		float3(dimensions.x, dimensions.y, 0.0f),
		float3(dimensions.x, 0.0f, 0.0f),
	};

	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		output.Pos = mul(gWorldViewProj, float4(input[0].Pos + offsets[i], 1.0f));
		output.TexUV = texCoords[i];

		triStream.Append(output);
	}
}

//PS_GBUFFER_OUT PS(PS_INPUT input)
//{
//	float4 color = float2(1.0f, textAtlus.Sample(textSampler, input.TexUV).r).xxxy;
//
//	clip(color.a < 0.01f ? -1 : 1);//Avoid blending on complete transparency
//
//	return PackGBuffer(float4(1.0, 1.0, 1.0, 1.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 0.0, 0.0, float3(1.0f, 1.0f, 1.0f));
//}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = float2(1.0f, textAtlus.Sample(textSampler, input.TexUV).r).xxxy;

	clip(color.a < 0.01f ? -1 : 1);//Handle transparency

	return color;
}