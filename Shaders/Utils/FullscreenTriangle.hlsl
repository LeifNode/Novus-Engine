//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef FULLSCREEN_TRIANGLE_HLSL
#define FULLSCREEN_TRIANGLE_HLSL

//Left handed coordinate system
//void GetTriangleVertex(uint vertexId, out float4 posOut, out float2 texOut)
//{
//	posOut.x = (float)(vertexId / 2) * 4.0 - 1.0;
//	posOut.y = (float)(vertexId % 2) * 4.0 - 1.0;
//	posOut.z = 0.0;
//	posOut.w = 1.0;
//
//	texOut.x = (float)(vertexId / 2) * 2.0;
//	texOut.y = 1.0 - (float)(vertexId % 2) * 2.0;
//}

void GetTriangleVertex(uint vertexId, out float4 posOut, out float2 texOut)
{
	posOut.x = (float)(vertexId % 2) * 4.0 - 1.0;
	posOut.y = (float)(vertexId / 2) * 4.0 - 1.0;
	posOut.z = 0.0;
	posOut.w = 1.0;

	texOut.x = (float)(vertexId % 2) * 2.0;
	texOut.y = 1.0 - (float)(vertexId / 2) * 2.0;
}

#endif