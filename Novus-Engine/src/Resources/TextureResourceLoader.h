//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_TEXTURE_RESOURCE_LOADER_H
#define NOVUS_TEXTURE_RESOURCE_LOADER_H

#include "IResourceLoader.h"

namespace novus
{

class TextureResouceLoader : public IResourceLoader
{
public:
	IResource* Load(const std::wstring& path) override;
};

}


#endif
