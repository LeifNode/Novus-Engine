//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_MESH_RESOURCE_LOADER
#define NOVUS_MESH_RESOURCE_LOADER

#include "IResourceLoader.h"

namespace novus
{

class MeshResourceLoader : public IResourceLoader
{
public:
	MeshResourceLoader();
	~MeshResourceLoader();

	//virtual IResource* Load(const std::wstring& path) override;
	virtual IResource* Load(void* memory, size_t size) override;
};

}

#endif
