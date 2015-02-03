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

class Texture2D
{
	friend class D3DRenderer;

public:
	Texture2D();
	~Texture2D();

	void Init(D3DRenderer* renderer, 
		      int width, 
			  int height,
			  DXGI_FORMAT format,
			  int mipCount = 1,
			  UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	void DeInit();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	ID3D11Texture2D* getTexture() const { return mpTexture; }
	ID3D11ShaderResourceView* getResourceView() const { return mpResourceView; }
	ID3D11RenderTargetView* getRenderTargetView() const { return mpRenderTargetView; }
	ID3D11UnorderedAccessView* getUnorderedAccessView() const { return mpUnorderedAccessView; }
	
	void setDebugName(const std::string& name);

private:
	Texture2D(const Texture2D&);
	Texture2D& operator=(const Texture2D&);

private:
	int mWidth;
	int mHeight;

	ID3D11Texture2D* mpTexture;
	ID3D11ShaderResourceView* mpResourceView;
	ID3D11RenderTargetView* mpRenderTargetView;
	ID3D11UnorderedAccessView* mpUnorderedAccessView;
};

}

#endif