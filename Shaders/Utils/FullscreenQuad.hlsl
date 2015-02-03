//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef FULLSCREEN_QUAD_HLSL
#define FULLSCREEN_QUAD_HLSL

struct QUAD_OUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord  : TEXCOORD;
};

QUAD_OUT FullscreenQuad(uint vertexId)
{
	QUAD_OUT quadOut;

	const float4 quadPositions[4] =
	{
		float4(-1.0, -1.0, 0.0, 1.0),
		float4(1.0, -1.0, 0.0, 1.0),
		float4(-1.0, 1.0, 0.0, 1.0),
		float4(1.0, 1.0, 0.0, 1.0),
	};

	const float2 quadTexCoords[4] =
	{
		float2(0.0, 1.0),
		float2(1.0, 1.0),
		float2(0.0, 0.0),
		float2(1.0, 0.0),
	};

	quadOut.Position = quadPositions[vertexId];
	quadOut.TexCoord = quadTexCoords[vertexId];

	return quadOut;
}

#endif