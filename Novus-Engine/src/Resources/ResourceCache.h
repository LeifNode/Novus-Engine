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
#include "Application/Common.h"
#include <map>

namespace novus
{

class ResourceCache
{
	friend class NovusApplication;

public:
	void Init();

	template <typename T>
	T* getResource(const std::wstring& resourcePath);

private:
	ResourceCache() {};
	virtual ~ResourceCache();

	void InitManagers();

private:
	std::map<const Type*, ResourceManager*> mResouceManagers;
};

template <class T>
T* ResourceCache::getResource(const std::wstring& resourcePath)
{
	T* resource = NULL;

	const Type* type = T::GetStaticType();

	auto it = mResouceManagers.find(type);
	if (it != mResouceManagers.end())
	{
		resource = static_cast<T*>(it->second->Load(resourcePath));

		return resource;
	}
	else
	{
		char error[128];
		sprintf_s(error, 128, "Could not find resouce manager for type %s.", type->GetTypeName());
		NE_ERROR(error, "ResourceCache");

		return NULL;
	}

}

}

#endif