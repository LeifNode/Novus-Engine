//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef STATICS_H
#define STATICS_H

#include "Common.h"

namespace novus
{

class D3DRenderer;
class NovusApplication;

class EngineStatics
{
	friend class NovusApplication;

public:
	static NovusApplication* getApplication();
	static D3DRenderer* getRenderer();

private:
	static NovusApplication* mspApplication;
	static D3DRenderer* mspRenderer;
};

}
#endif