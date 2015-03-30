//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_D3D_PROFILER_H
#define NOVUS_D3D_PROFILER_H

#include "Graphics/D3DRenderer.h"
#include "Application/EngineStatics.h"

#define BEGIN_GRAPHICS_PROFILED_SECTION(name) \
	(\
		\
	)\

#define END_GRAPHICS_PROFILED_SECTION(name) \
	(\
	\
	)\

namespace novus
{

class D3DProfiler
{
public:
	D3DProfiler();
	~D3DProfiler();

	

private:

};

}

#endif