//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_IRESOURCE_LOADER_H
#define NOVUS_IRESOURCE_LOADER_H

#include <string>
#include "IResource.h"

namespace novus
{

	class IResourceLoader
	{
	public:
		IResourceLoader();
		virtual ~IResourceLoader();

		virtual IResource* Load(const std::wstring& path) = 0;
		virtual IResource* Load(void* memory, size_t size) = 0;


	};

}

#endif