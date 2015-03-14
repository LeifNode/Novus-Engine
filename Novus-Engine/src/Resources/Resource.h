//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_IRESOURCE_H
#define NOVUS_IRESOURCE_H

#include "Utils/Objects/Type.h"

namespace novus
{
class Resource
{
	//Get amount of space resource takes on the HDD
	virtual size_t getDiskSize() const { return 0; }

	//Get amount of GPU memory this resource occupies
	virtual size_t getGPUSize() const { return 0; }

	//Get amount of RAM used by this resource
	virtual size_t getSize() const { return 0; }

	//virtual const Type* getStaticType() const = 0; 
};
}

#endif