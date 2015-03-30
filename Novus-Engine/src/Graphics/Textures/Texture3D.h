//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TEXTURE3D_H
#define NOVUS_TEXTURE3D_H

#include "Application/Common.h"
#include "Resources/Resource.h"

namespace novus
{

class Texture3D : public Resource
{
	friend class D3DRenderer;

	NOVUS_OBJECT_DECL(Texture3D);

public:
	Texture3D();
	~Texture3D();

	void Init(D3DRenderer* renderer,
		int width,
		int height,
		int depth,
		DXGI_FORMAT format,
		int mipCount = 1,
		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		UINT miscFlags = 0,
		DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN);

	void DeInit();

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }
	int getDepth() const { return mDepth; }

	ID3D11Texture3D* getTexture() const { return mpTexture; }
	ID3D11ShaderResourceView* getResourceView() const { return mpResourceView; }
	ID3D11RenderTargetView* getRenderTargetView() const { return mpRenderTargetView; }
	ID3D11UnorderedAccessView* getUnorderedAccessView() const { return mpUnorderedAccessView; }

	void setDebugName(const std::string& name);

private:
	Texture3D(const Texture3D&);
	Texture3D& operator=(const Texture3D&);

private:
	int mWidth;
	int mHeight;
	int mDepth;

	ID3D11Texture3D* mpTexture;
	ID3D11ShaderResourceView* mpResourceView;
	ID3D11RenderTargetView* mpRenderTargetView;
	ID3D11UnorderedAccessView* mpUnorderedAccessView;
};

NOVUS_OBJECT_DEF(Texture3D);

}
#endif