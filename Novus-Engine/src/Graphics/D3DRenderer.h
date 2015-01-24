//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_D3D_RENDERER_H
#define NOVUS_D3D_RENDERER_H

#include "Application/Common.h"
#include "GBuffer.h"
#include "Shaders/IShader.h"
#include "Shader.h"
#include "ConstantBuffers.h"
#include "Textures\Texture.h"

namespace novus
{

namespace DepthStencilState
{
	enum Type
	{
		Invalid = -1,
		Default,
		Particle,
		COUNT,
	};
}

class D3DRenderer
{
	friend class OVRRenderer;
	
public:
	D3DRenderer();
	~D3DRenderer();

	bool Init();
	void OnResize();

	ID3D11Device* device() const { return mpd3dDevice; }
	ID3D11DeviceContext* context() const { return mpd3dImmediateContext; }
	ID3D11RenderTargetView* renderTarget() const { return mpRenderTarget; }
	IDXGISwapChain* swapChain() const { return mpSwapChain; }

	void setTextureResource(int index, Texture*);
	void setTextureResources(Texture** texArray, int startSlot, unsigned count);
	void setTextureResources(ID3D11ShaderResourceView** texArray, int startSlot, unsigned count);
	void setSampler(int index, ID3D11SamplerState* samplerState);
	void setConstantBuffer(int index, ID3D11Buffer*);
	void setPerFrameBuffer(CBPerFrame& buffer);
	void setPerObjectBuffer(CBPerObject& buffer);
	void bindPerFrameBuffer();

	void UnbindTextureResources();

	//Texture* CreateTexture(UINT format, int width, int height);
	Texture* CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, DXGI_FORMAT resViewFmt = DXGI_FORMAT_UNKNOWN);
	bool CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, ID3D11Texture2D** textureOut, ID3D11ShaderResourceView** resourceOut, DXGI_FORMAT resViewFmt = DXGI_FORMAT_UNKNOWN);
	//Texture* CreateTexture(int width, int height, DXGI_FORMAT colorFormat, )
	//RenderTarget* CreateRenderTarget(int width, int height, bool useDepthBuffer = true);
	//void setRenderTarget(RenderTarget* target);

	void setBlendState(bool blendingEnabled);
	void setDepthStencilState(DepthStencilState::Type state);

	void setViewport(int width, int height, int x, int y);

	const CBPerFrame* getPerFrameBuffer() const { return &mPerFrameData; }

	void setShader(Shader* shader);
	void setShader(const std::string& name);
	Shader* getShader(const std::string& name);
	Shader* getActiveShader() const { return mpActiveShader; }

	void UnbindShader();

	void ResetSamplerState();
	void ResetRenderTarget();

	Shader* LoadShader(const WCHAR* filePath,
					   const ShaderInfo* shaderInfo, 
					   D3D_PRIMITIVE_TOPOLOGY primitiveTopology, 
					   const D3D11_INPUT_ELEMENT_DESC* vertexDescription, 
					   int vertexDescriptionSize, 
					   const D3D_SHADER_MACRO* defines = NULL);

	Shader* LoadShaderUnmanaged(const WCHAR* filePath, 
								const ShaderInfo* shaderInfo, 
								D3D_PRIMITIVE_TOPOLOGY primitiveTopology, 
								const D3D11_INPUT_ELEMENT_DESC* vertexDescription, 
								int vertexDescriptionSize, 
								const D3D_SHADER_MACRO* defines = NULL);

	//void Clear(RenderTarget* target);

	void PreRender();
	void PostRender();
	void RenderDeferredLighting();

	//void pushTransform(Transform& transform);
	//void popTransform();
	//XMMATRIX getTopTransform() const;
	//XMMATRIX getTopTransformInverse() const;

	GBuffer* getGBuffer() const { return mpGBuffer; }
	//OVRManager* getHMD() const { return mpOVRManager; }
	bool isUsingHMD() const;

private:
	static HRESULT CompileShaderFromFile(const WCHAR* szFileName, 
										 LPCSTR szEntryPoint, 
										 LPCSTR szShaderModel, 
										 const D3D_SHADER_MACRO* defines, 
										 ID3DBlob** ppBlobOut);

	void InitDepthStencilStates();

private:
	//OVRManager* mpOVRManager;
	bool mUseHMD;

	GBuffer* mpGBuffer;
	//DeferredRenderer* mpDeferredRenderer;

	UINT m4xMsaaQuality;

	ID3D11Device* mpd3dDevice;
	ID3D11DeviceContext* mpd3dImmediateContext;
	ID3D11Debug* mpd3dDebug;
	IDXGISwapChain* mpSwapChain;
	ID3D11Texture2D* mpDepthStencilBuffer;
	ID3D11RenderTargetView* mpRenderTarget;
	ID3D11DepthStencilView* mpDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	ID3D11Buffer* mpPerFrameBuffer;
	CBPerFrame mPerFrameData;
	ID3D11Buffer* mpPerObjectBuffer;
	ID3D11SamplerState* mpSamplerState;
	ID3D11BlendState* mpBlendStateAlpha;
	ID3D11BlendState* mpBlendStateOpaque;

	ID3D11DepthStencilState* mpDepthStencilStates[DepthStencilState::COUNT];

	bool mEnable4xMsaa;


	float mClearColor[4];

	Shader* mpActiveShader;
	//ID3D11SamplerState* mCurrentSamplerState;

	std::map<std::string, Shader*> mLoadedShaders;

	//MatrixStack mMatrixStack;
};

}

#endif