//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef CONSTANT_BUFFERS_HLSL
#define CONSTANT_BUFFERS_HLSL

#include "Lighting/Lights.hlsl"

cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gViewInv;
	float4x4 gProjection;
	float4x4 gProjectionInv;

	float4x4 gViewProj;
	float4x4 gViewProjInv;

	uint2 gScreenDimensions;
	float2 gClipNearFar;

	float3 gHeadPosition;
	float gpad1;
	float3 gEyePosition;
	float gpad2;
	float3 gEyeDirection;
	float gpad3;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gTextureTransform;
	Material gMaterial;
};

#endif