//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_IMODEL_LOADER_H
#define NOVUS_IMODEL_LOADER_H

#include "AssetTypes.h"

namespace novus
{

class IModelLoader
{
public:
	IModelLoader() {}
	virtual ~IModelLoader() {}

	virtual bool Load(void* data, size_t size)=0;
	//virtual bool Load(const std::wstring& path)=0;

	virtual assettypes::Scene* getScene() const=0;
};

}


#endif

