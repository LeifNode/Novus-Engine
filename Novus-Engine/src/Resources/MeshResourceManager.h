//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_MESH_RESOURCE_MANAGER_H
#define NOVUS_MESH_RESOURCE_MANAGER_H

#include "ResourceManager.h"
#include "Mesh/MeshLoader.h"
#include "Mesh/Processing/IMeshProcess.h"
#include <map>
#include <string>

namespace novus
{

class MeshResourceManager : public ResourceManager
{
	NOVUS_OBJECT_DECL(MeshResourceManager);
public:
	MeshResourceManager();
	~MeshResourceManager();

	void Init();

	Resource* Load(const std::wstring& path) override;

private:
	void InitMeshLoaders();
	void InitProcessingPipeline();

	void ProcessSceneMeshes(assettypes::Scene* scene);

private:
	std::map<std::wstring, class StaticMesh*> mLoadedMeshes;

	std::map<std::wstring, MeshLoader*> mMeshLoaders;
	std::vector<IMeshProcess*> mProcesses;
};

NOVUS_OBJECT_DEF(MeshResourceManager);

}

#endif
