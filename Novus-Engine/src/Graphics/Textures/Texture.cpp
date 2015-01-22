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