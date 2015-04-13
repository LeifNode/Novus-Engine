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
#include "Shaders/Shader.h"
#include "ConstantBuffers.h"
#include "Textures/Texture2D.h"
#include "Textures/Texture3D.h"
#include "Math/MatrixStack.h"

namespace novus
{

class Transform;
class D3DLocalInclude;

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
	friend class DeferredRenderer;
	
public:
	D3DRenderer();
	~D3DRenderer();

	bool Init();
	void OnResize();

	ID3D11Device* device() const { return mpd3dDevice; }
	ID3D11DeviceContext* context() const { return mpd3dImmediateContext; }
	ID3D11RenderTargetView* renderTarget() const { return mpRenderTarget; }
	IDXGISwapChain* swapChain() const { return mpSwapChain; }

	void setTextureResource(int index, Texture2D*);
	void setTextureResource(int index, Texture3D*);
	void setTextureResources(Texture2D** texArray, int startSlot, unsigned count);
	void setTextureResources(ID3D11ShaderResourceView** texArray, int startSlot, unsigned count);
	void setSampler(int index, ID3D11SamplerState* samplerState);
	void setConstantBuffer(int index, ID3D11Buffer*);
	void setPerFrameBuffer(CBPerFrame& buffer);
	void setPerObjectBuffer(CBPerObject& buffer);
	void BindPerFrameBuffer();

	void UnbindTextureResources();
	void UnbindUAVs();

	//Texture2D* CreateTexture(UINT format, int width, int height);
	Texture2D* CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, DXGI_FORMAT resViewFmt = DXGI_FORMAT_UNKNOWN);
	bool CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, ID3D11Texture2D** textureOut, ID3D11ShaderResourceView** resourceOut, DXGI_FORMAT resViewFmt = DXGI_FORMAT_UNKNOWN);
	//Texture2D* CreateTexture(int width, int height, DXGI_FORMAT colorFormat, )
	//RenderTarget* CreateRenderTarget(int width, int height, bool useDepthBuffer = true);
	//void setRenderTarget(RenderTarget* target);

	void setBlendState(bool blendingEnabled);
	void setDepthStencilState(DepthStencilState::Type state);

	void setViewport(int x, int y, int width, int height, int depth = 1);

	const CBPerFrame* getPerFrameBuffer() const { return &mPerFrameData; }

	void setShader(Shader* shader);
	void setShader(const std::string& name);
	Shader* getShader(const std::string& name);
	Shader* getActiveShader() const { return mpActiveShader; }

	void UnbindShader();

	void ResetSamplerState();
	void ResetRenderTarget();
	void ResetRasterizerState();

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

	void ClearDepth();

	void PreRender();
	void PostRender();
	void RenderDeferredShading();

	void PushTransform(const Transform& transform);
	void PushTransform(const Matrix4& transform);
	void PopTransform();
	Matrix4 GetTopTransform() const;
	Matrix4 GetTopTransformInverse() const;

	GBuffer* getGBuffer() const { return mpGBuffer; }
	ID3D11DepthStencilView* getDepthView() const { return mpDepthStencilView; }
	DeferredRenderer* getDeferredRenderer() const { return mpDeferredRenderer; }
	//OVRManager* getHMD() const { return mpOVRManager; }
	bool isUsingHMD() const;

private:
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, 
								  LPCSTR szEntryPoint, 
								  LPCSTR szShaderModel, 
								  const D3D_SHADER_MACRO* defines, 
								  ID3DBlob** ppBlobOut);

	void InitDepthStencilStates();

private:
	//OVRManager* mpOVRManager;
	bool mUseHMD;

	GBuffer* mpGBuffer;

	DeferredRenderer* mpDeferredRenderer;

	UINT m4xMsaaQuality;

	D3DLocalInclude* mpIncludeDir;

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
	ID3D11RasterizerState* mpRasterizerState;
	ID3D11BlendState* mpBlendStateAlpha;
	ID3D11BlendState* mpBlendStateOpaque;

	ID3D11DepthStencilState* mpDepthStencilStates[DepthStencilState::COUNT];

	bool mEnable4xMsaa;


	float mClearColor[4];

	Shader* mpActiveShader;
	//ID3D11SamplerState* mCurrentSamplerState;

	std::map<std::string, Shader*> mLoadedShaders;

	MatrixStack mMatrixStack;
};

}

#endif