//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_MESH_LOADER_H
#define NOVUS_MESH_LOADER_H

#include "AssetTypes.h"

namespace novus
{
class File;

class MeshLoader
{
public:
	MeshLoader();
	virtual ~MeshLoader();

	virtual bool Load(class File* file)=0;
	//virtual bool Load(const std::wstring& path)=0;

	assettypes::Scene* getScene() const { return mpScene; }
	void DeleteScene();

protected:
	assettypes::Scene* mpScene;
};

}


#endif

