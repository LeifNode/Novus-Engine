//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_GBUFFER_H
#define NOVUS_GBUFFER_H

#include "Application/Common.h"

namespace novus
{

class GBuffer
{
	friend class D3DRenderer;

public:
	GBuffer();
	~GBuffer();

	void BindRenderTargets();
	void BindTextures();
	void BindSampler();

	void ClearRenderTargets();

	void Init(int width, int height);
	void OnResize(int width, int height);

private:
	GBuffer(const GBuffer&);
	GBuffer& operator= (const GBuffer&);

	void DeInit();

private:
	int mWidth, mHeight;

	ID3D11Texture2D* mpDiffuseTexture;
	ID3D11Texture2D* mpDepthStencilTexture;
	ID3D11Texture2D* mpNormalTexture;
	ID3D11Texture2D* mpRoughnessTexture;
	ID3D11Texture2D* mpEmissiveTexture;

	ID3D11ShaderResourceView* mpDiffuseResourceView;
	ID3D11ShaderResourceView* mpDepthStencilResourceView;
	ID3D11ShaderResourceView* mpNormalResourceView;
	ID3D11ShaderResourceView* mpRoughnessResourceView;
	ID3D11ShaderResourceView* mpEmissiveResourceView;

	ID3D11DepthStencilView* mpDepthStencilTargetView;
	ID3D11RenderTargetView* mpDiffuseTargetView;
	ID3D11RenderTargetView* mpNormalTargetView;
	ID3D11RenderTargetView* mpRoughnessTargetView;
	ID3D11RenderTargetView* mpEmissiveTargetView;

	ID3D11UnorderedAccessView* mpDepthStencilUAV;
	ID3D11UnorderedAccessView* mpDiffuseUAV;
	ID3D11UnorderedAccessView* mpNormalUAV;
	ID3D11UnorderedAccessView* mpRoughnessUAV;
	ID3D11UnorderedAccessView* mpEmissiveUAV;

	ID3D11DepthStencilState* mpDepthStencilState;
	ID3D11SamplerState* mpSamplerState;
};

}

#endif