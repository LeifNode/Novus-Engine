//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//================================================================
//	Common.h: Should be included at the top of each header.
//	
//	Includes commonly used headers and functions.
//================================================================

#pragma once

#ifndef NOVUS_COMMON_H
#define NOVUS_COMMON_H

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifndef DEBUG_MEMORY_ALLOC
#define DEBUG_MEMORY_ALLOC
#endif

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>
#include <cassert>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include <string>
#include <algorithm>

#include "Utils/dxerr.h"

#include "Math/Math.h"
#include "Utils/Memory/Memory.h"
#include "Utils/Logging/Logger.h"

#include "3rdParty/FastDelegate.h"

using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

#ifndef ReleaseCOM
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#endif

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                                                      \
	{                                                                                  \
		HRESULT hr = (x);                                                              \
	if (FAILED(hr))                                                                    \
	{                                                                                  \
		DXTrace(const_cast<WCHAR*>((WCHAR*)__FILE__), (DWORD)__LINE__, hr, L#x, true); \
	}                                                                                  \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

#endif