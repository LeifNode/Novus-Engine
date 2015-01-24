//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef COMMON_HLSL
#define COMMON_HLSL

#include "Utils/Defines.hlsl"
#include "Utils/ConstantBuffers.hlsl"
#include "Utils/DeferredRendering.hlsl"
#include "Utils/Math.hlsl"

#ifdef DEFERRED_RENDERER
#if DEFERRED_RENDERER == 1
#define PS_RETURN_TYPE float4
#define PS_RETURN_TARGET : SV_Target
#else
#define PS_RETURN_TYPE PS_GBUFFER_OUT
#define PS_RETURN_TARGET
#endif
#endif

#endif