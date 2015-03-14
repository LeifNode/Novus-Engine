#include "TextureResourceManager.h"
#include "Texture/DDSTextureLoader.h"
#include "Application/EngineStatics.h"
#include "Graphics/D3DRenderer.h"
#include "Graphics/Textures/Texture2D.h"

namespace novus
{

TextureResouceManager::TextureResouceManager()
{
}

TextureResouceManager::~TextureResouceManager()
{
	for (auto it = mLoadedTextures.begin(); it != mLoadedTextures.end(); ++it)
	{
		NE_DELETE(it->second);
	}
}

Resource* TextureResouceManager::Load(const std::wstring& path)
{
	auto it = mLoadedTextures.find(path);

	if (it != mLoadedTextures.end())
	{
		return it->second;
	}

	D3DRenderer* renderer = EngineStatics::getRenderer();

	Texture2D* newTex = NE_NEW Texture2D();

	newTex->Load(renderer, path.c_str());

	mLoadedTextures[std::wstring(path)] = newTex;

	return newTex;
}

}