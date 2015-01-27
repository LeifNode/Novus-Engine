//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TEXTURE_H
#define NOVUS_TEXTURE_H

#include "Application/Common.h"

namespace novus
{

class Texture
{
	friend class D3DRenderer;

public:
	~Texture();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	ID3D11Texture2D* getD3DTexture() const { return mpTexture; }
	ID3D11ShaderResourceView* getResourceView() const { return mpResourceView; }
	
	void SetDebugName(const std::string& name);

private:
	Texture();

private:
	int mWidth;
	int mHeight;

	ID3D11Texture2D* mpTexture;
	ID3D11ShaderResourceView* mpResourceView;
};

}

#endif