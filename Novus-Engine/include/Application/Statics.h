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

class NovusStatics
{
	friend class NovusApplication;

public:
	NovusStatics* GetInstance();



private:
	static NovusStatics* mpInstance;
};

}
#endif