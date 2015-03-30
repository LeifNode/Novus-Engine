//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Handles sampling from GBuffer for lighting

#ifndef DEFERRED_SHADING_HLSL
#define DEFERRED_SHADING_HLSL

#include "Utils/Defines.hlsl"
#include "Utils/ConstantBuffers.hlsl"
#include "Lighting/Lights.hlsl"
#include "Deferred/GBuffer.hlsl"

TextureCube       EnvironmentProbe : register(t5);

Texture2D<float2> BRDF_LUT         : register(t7);

RWTexture2D<float4> OutputTexture  : register(u0);

SamplerState PointSampler          : register(s0);
SamplerState EnvironmentSampler    : register(s1);

StructuredBuffer<PointLight> gPointLights : register(t6);

#endif