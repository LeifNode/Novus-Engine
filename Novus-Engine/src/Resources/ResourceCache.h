//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_IRESOURCE_CACHE_H
#define NOVUS_IRESOURCE_CACHE_H

#include "Resource.h"
#include "ResourceManager.h"

namespace novus
{

class ResourceCache
{
public:
	ResourceCache() {};
	virtual ~ResourceCache() {};

	template <IResource T>
	T* getResource(const std::wstring& resourcePath);

private:

};

template <class T>
T* ResourceCache::getResource(const std::wstring& resourcePath)
{
	T* resource = NULL;

	
}

}

#endif