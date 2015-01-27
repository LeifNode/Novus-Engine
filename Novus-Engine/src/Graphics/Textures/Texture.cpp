#include "Texture.h"

using novus::Texture;

Texture::Texture()
	:
	mWidth(0),
	mHeight(0),
	mpTexture(NULL),
	mpResourceView(NULL)
{}

Texture::~Texture()
{
	ReleaseCOM(mpResourceView);
	ReleaseCOM(mpTexture);
}

void Texture::SetDebugName(const std::string& name)
{
	mpTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
}