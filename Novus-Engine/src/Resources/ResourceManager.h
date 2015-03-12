//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_RESOURCE_MANAGER_H
#define NOVUS_RESOURCE_MANAGER_H

#include "IResource.h"

namespace novus
{

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	virtual void Register(IResource* resource);

protected:

};

}

#endif