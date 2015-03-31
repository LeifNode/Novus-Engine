//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#include "Common.hlsl"

cbuffer cbPerLine : register(b2)
{
	uint gPointCount;
	uint3 pad;
	float4 gLineColor;
	float gLineWidth;
	float3 pad2;
};

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct GS_INPUT
{
	float3 PositionW : POSITION;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float Id : VERTEXID;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output;

	output.PositionW = mul(gWorld, float4(input.Position, 1.0f)).xyz;

	return output;
}

[maxvertexcount(4)]
void GS(lineadj GS_INPUT points[4], uint primID : SV_PrimitiveID, inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;

	int i = 0;

	float3 midPoint;
	float3 toEye;
	float distToEye;
	float3 toNextPoint;
	float halfLineWidth = gLineWidth * 0.5f;

	float3 upAxis;
	float3 neighborUpAxis[3];

	midPoint = (points[1].PositionW + points[2].PositionW) * 0.5f;

	toEye = midPoint - gEyePosition;
	distToEye = length(toEye);
	toEye /= distToEye;

	toNextPoint = normalize(points[1].PositionW - points[2].PositionW);
	upAxis = normalize(cross(toEye, toNextPoint));

	[unroll]
	for (i = 0; i < 3; i += 2)
	{
		midPoint = (points[i].PositionW + points[i + 1].PositionW) * 0.5f;
		toEye = normalize(midPoint - gEyePosition);
		toNextPoint = points[i].PositionW - points[i + 1].PositionW;

		float nextPointLength = dot(toNextPoint, toNextPoint);


		[flatten]
		if (nextPointLength == 0.0f)
			neighborUpAxis[i] = upAxis;
		else
			neighborUpAxis[i] = normalize(cross(toEye, normalize(toNextPoint)));
	}

	/*const float3 neighborPoints[4] =
	{
		points[1].PositionW + neighborUpAxis[0] * halfLineWidth,
		points[1].PositionW - neighborUpAxis[0] * halfLineWidth,
		points[2].PositionW + neighborUpAxis[2] * halfLineWidth,
		points[2].PositionW - neighborUpAxis[2] * halfLineWidth,
	};*/
	
	
	const float3 neighborPoints[4] =
	{
		points[1].PositionW + upAxis * halfLineWidth,
		points[1].PositionW - upAxis * halfLineWidth,
		points[2].PositionW + upAxis * halfLineWidth,
		points[2].PositionW - upAxis * halfLineWidth,
	};

	const float3 outPoints[4] =
	{
		(points[1].PositionW + upAxis * halfLineWidth + neighborPoints[0]) * 0.5f,
		(points[1].PositionW - upAxis * halfLineWidth + neighborPoints[1]) * 0.5f,
		(points[2].PositionW + upAxis * halfLineWidth + neighborPoints[2]) * 0.5f,
		(points[2].PositionW - upAxis * halfLineWidth + neighborPoints[3]) * 0.5f,
	};

	[unroll]
	for (i = 0; i < 4; i++)
	{
		output.PositionH = mul(gViewProj, float4(outPoints[i], 1.0f));

		output.Id = primID;

		triStream.Append(output);
	}

	//Basic line segment calculations without attempting to maintain contiguity between segments
	/*int i = 0;
	float halfLineWidth = gLineWidth * 0.5f;

	const float4 screenPoints[2] =
	{
	mul(gViewProj, float4(points[1], 1.0f)),
	mul(gViewProj, float4(points[2], 1.0f)),
	};


	float2 toPoint = normalize(screenPoints[0].xy - screenPoints[1].xy);
	float4 offsetVec = float4(normalize(float2(-toPoint.y, toPoint.x)), 0.0f, 0.0f);

	const float4 finalPoints[4] =
	{
	screenPoints[0] + offsetVec * halfLineWidth * screenPoints[0].w,
	screenPoints[0] - offsetVec * halfLineWidth * screenPoints[0].w,
	screenPoints[1] + offsetVec * halfLineWidth * screenPoints[1].w,
	screenPoints[1] - offsetVec * halfLineWidth * screenPoints[1].w,
	};

	[unroll]
	for (i = 0; i < 4; i++)
	{
	output.PositionH = finalPoints[i];
	output.Id = primID;

	triStream.Append(output);
	}*/
}

PS_GBUFFER_OUT PS(PS_INPUT input)
{
	//float4 color = gLineColor + float4(0.0f, frac((float)input.Id * 0.0001f), frac((float)input.Id * 0.0003f), 0.0f);

	float3 color = gLineColor;/* +float4(0.0f, frac(input.Id * 0.001f), frac(input.Id * 0.0003f), 0.0f);*/
	//float4 color = float4(input.PositionH.yyy / 720.0f, 1.0f);

	return PackGBuffer(float4(0.0, 0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 0.0, 0.0, color);
}