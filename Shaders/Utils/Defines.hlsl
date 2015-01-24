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

#ifndef SPECULAR_POW_RANGE_MIN
#define SPECULAR_POW_RANGE_MIN 1.0
#endif

#ifndef SPECULAR_POW_RANGE_MAX
#define SPECULAR_POW_RANGE_MAX 160.0
#endif

#ifndef EMISSIVE_RANGE_MIN //(1 / 16)
#define EMISSIVE_RANGE_MIN 0.0625
#endif

#ifndef EMISSIVE_RANGE_MAX
#define EMISSIVE_RANGE_MAX 16.0
#endif

#endif //!defined(DEFINES_HLSL)