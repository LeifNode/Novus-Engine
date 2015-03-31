//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef DEFINES_HLSL
#define DEFINES_HLSL

#ifndef DEFERRED_RENDERER
#define DEFERRED_RENDERER 1
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 1024
#endif

#ifndef ENV_MAP_SAMPLES
#define ENV_MAP_SAMPLES 1024
#endif

#ifndef BRDF_SAMPLES
#define BRDF_SAMPLES 512
#endif

#define COMPUTE_SHADER_TILE_GROUP_DIM 16

#define COMPUTE_SHADER_TILE_GROUP_SIZE (COMPUTE_SHADER_TILE_GROUP_DIM * COMPUTE_SHADER_TILE_GROUP_DIM)

#endif //!defined(DEFINES_HLSL)