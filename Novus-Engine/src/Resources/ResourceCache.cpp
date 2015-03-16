#include "ResourceCache.h"
#include "MeshResourceManager.h"
#include "TextureResourceManager.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Textures/Texture2D.h"

namespace novus
{
ResourceCache::~ResourceCache()
{
	for (auto it = mResouceManagers.begin(); it != mResouceManagers.end(); ++it)
	{
		NE_DELETE(it->second);
	}
}

void ResourceCache::Init()
{
	InitManagers();
}

void ResourceCache::InitManagers()
{
	MeshResourceManager* meshResourceManager = NE_NEW MeshResourceManager();
	meshResourceManager->Init();
	mResouceManagers[StaticMesh::GetStaticType()] = meshResourceManager;

	TextureResouceManager* textureResourceManager = NE_NEW TextureResouceManager();
	mResouceManagers[Texture2D::GetStaticType()] = textureResourceManager;
}

}