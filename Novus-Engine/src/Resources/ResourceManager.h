//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_RESOURCE_MANAGER_H
#define NOVUS_RESOURCE_MANAGER_H

#include "Resource.h"
#include "Core/Object.h"

namespace novus
{

class ResourceManager : public Object
{
	NOVUS_OBJECT_DECL(ResourceManager);

	ResourceManager() {}
	virtual ~ResourceManager() {}

	virtual Resource* Load(const std::wstring& path)=0;
};

NOVUS_OBJECT_DEF(ResourceManager);

}

#endif