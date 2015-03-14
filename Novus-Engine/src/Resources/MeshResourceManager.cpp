#include "MeshResourceManager.h"
#include "Utils/StringUtils.h"
#include "Graphics/StaticMesh.h"
#include "Mesh/AssetTypes.h"
#include "Mesh/OBJLoader.h"
#include "Mesh/Processing/MeshTriangulatePass.h"
#include "Mesh/Processing/MeshCalculateNormalsPass.h"
#include "Mesh/Processing/MeshCalculateTangentsPass.h"
#include "Utils/FileSystem/File.h"

namespace novus
{

MeshResourceManager::MeshResourceManager()
{
}

MeshResourceManager::~MeshResourceManager()
{
	for (auto it = mMeshLoaders.begin(); it != mMeshLoaders.end(); ++it)
	{
		NE_DELETE(it->second);
	}

	for (auto it = mProcesses.begin(); it != mProcesses.end(); ++it)
	{
		NE_DELETE((*it));
	}
}

void MeshResourceManager::Init()
{
	InitMeshLoaders();
	InitProcessingPipeline();
}

void MeshResourceManager::InitMeshLoaders()
{
	mMeshLoaders.insert(std::make_pair(L".obj", NE_NEW OBJLoader()));
}

void MeshResourceManager::InitProcessingPipeline()
{
	mProcesses.push_back(NE_NEW MeshTriangulatePass());
	mProcesses.push_back(NE_NEW MeshCalculateNormalsPass(false));
	mProcesses.push_back(NE_NEW MeshCalculateTangentsPass());
}

void MeshResourceManager::ProcessSceneMeshes(assettypes::Scene* scene)
{
	for (auto it = scene->mMeshes.cbegin(); it != scene->mMeshes.cend(); ++it)
	{
		for (auto process = mProcesses.cbegin(); process != mProcesses.cend(); ++process)
			(*process)->Execute(*it);
	}
}

Resource* MeshResourceManager::Load(const std::wstring& path)
{
	std::wstring extension = ParseExtensionFromString(path);

	MeshLoader* meshLoader = NULL;

	for (auto it = mMeshLoaders.cbegin(); it != mMeshLoaders.cend(); ++it)
	{
		if (it->first == extension)
		{
			meshLoader = it->second;
			break;
		}
	}

	if (meshLoader == NULL)
	{
		NE_ERROR("Could not find mesh loader for specified file type.", "MeshResouceLoader");
		return NULL;
	}

	assettypes::Scene* loadedScene = NULL;

	File* meshFile = NE_NEW File();
	meshFile->Load(path);

	//Load scene from file
	bool loaded = meshLoader->Load(meshFile);

	NE_DELETE(meshFile);

	if (!loaded)
		return NULL;

	loadedScene = meshLoader->getScene();

	//Run mesh pre-processing
	ProcessSceneMeshes(loadedScene);

	StaticMesh* newMesh = NE_NEW StaticMesh();

	//Load mesh data onto GPU
	newMesh->Init(loadedScene);

	//Free raw scene data since it is no longer used
	meshLoader->DeleteScene();

	return newMesh;
}

}