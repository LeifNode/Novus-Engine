//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_TEXTURE_RESOURCE_LOADER_H
#define NOVUS_TEXTURE_RESOURCE_LOADER_H

#include "ResourceManager.h"
#include <map>

namespace novus
{
class Texture2D;

class TextureResouceManager : public ResourceManager
{
	NOVUS_OBJECT_DECL(TextureResouceManager);
public:
	TextureResouceManager();
	~TextureResouceManager();

	Resource* Load(const std::wstring& path) override;

private:
	std::map<std::wstring, Texture2D*> mLoadedTextures;
};

NOVUS_OBJECT_DEF(TextureResouceManager);

}


#endif
