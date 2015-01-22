#include "GBuffer.h"
#include "D3DRenderer.h"
#include "Application/EngineStatics.h"

using novus::GBuffer;

GBuffer::GBuffer()
	:
	mWidth(0),
	mHeight(0),
	mpDiffuseTexture(NULL),
	mpDepthStencilTexture(NULL),
	mpNormalTexture(NULL),
	mpRoughnessTexture(NULL),
	mpEmissiveTexture(NULL),
	mpDiffuseResourceView(NULL),
	mpDepthStencilResourceView(NULL),
	mpNormalResourceView(NULL),
	mpRoughnessResourceView(NULL),
	mpEmissiveResourceView(NULL),
	mpDiffuseTargetView(NULL),
	mpDepthStencilTargetView(NULL),
	mpNormalTargetView(NULL),
	mpRoughnessTargetView(NULL),
	mpEmissiveTargetView(NULL),
	mpDepthStencilState(NULL),
	mpSamplerState(NULL)
{
}

GBuffer::~GBuffer()
{
	DeInit();
}

void GBuffer::Initialize(int width, int height)
{
	//OnResize(width, height);

	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = TRUE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	desc.FrontFace = stencilMarkOp;
	desc.BackFace = stencilMarkOp;

	EngineStatics::getRenderer()->device()->CreateDepthStencilState(&desc, &mpDepthStencilState);

	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;

	EngineStatics::getRenderer()->device()->CreateSamplerState(&samDesc, &mpSamplerState);
}

void GBuffer::DeInit()
{
	ReleaseCOM(mpDiffuseTargetView);
	ReleaseCOM(mpDepthStencilTargetView);
	ReleaseCOM(mpNormalTargetView);
	ReleaseCOM(mpRoughnessTargetView);
	ReleaseCOM(mpEmissiveTargetView);

	ReleaseCOM(mpDiffuseResourceView);
	ReleaseCOM(mpDepthStencilResourceView);
	ReleaseCOM(mpNormalResourceView);
	ReleaseCOM(mpRoughnessResourceView);
	ReleaseCOM(mpEmissiveResourceView);

	ReleaseCOM(mpDiffuseTexture);
	ReleaseCOM(mpDepthStencilTexture);
	ReleaseCOM(mpNormalTexture);
	ReleaseCOM(mpRoughnessTexture);
	ReleaseCOM(mpEmissiveTexture);

	ReleaseCOM(mpDepthStencilState);
	ReleaseCOM(mpSamplerState);
}

void GBuffer::OnResize(int width, int height)
{
	if (width <= 0 || height <= 0 || (width == mWidth && height == mHeight)) //Don't need to resize
		return;

	ReleaseCOM(mpDiffuseTargetView);
	ReleaseCOM(mpDepthStencilTargetView);
	ReleaseCOM(mpNormalTargetView);
	ReleaseCOM(mpRoughnessTargetView);
	ReleaseCOM(mpEmissiveTargetView);

	ReleaseCOM(mpDiffuseResourceView);
	ReleaseCOM(mpDepthStencilResourceView);
	ReleaseCOM(mpNormalResourceView);
	ReleaseCOM(mpRoughnessResourceView);
	ReleaseCOM(mpEmissiveResourceView);

	ReleaseCOM(mpDiffuseTexture);
	ReleaseCOM(mpDepthStencilTexture);
	ReleaseCOM(mpNormalTexture);
	ReleaseCOM(mpRoughnessTexture);
	ReleaseCOM(mpEmissiveTexture);

	D3DRenderer* renderer = EngineStatics::getRenderer();

	static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
	static const DXGI_FORMAT diffuseTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalTextureFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT roughnessTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT emissiveTextureFormat = DXGI_FORMAT_R16G16B16A16_UNORM;

	static const DXGI_FORMAT depthStencilRenderViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const DXGI_FORMAT diffuseRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT normalRenderViewFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT roughnessRenderViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT emissiveRenderViewFormat = DXGI_FORMAT_R16G16B16A16_UNORM;

	//Initialize textures
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	desc.Format = depthStencilTextureFormat;
	renderer->CreateTexture(&desc, &mpDepthStencilTexture, &mpDepthStencilResourceView, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Format = diffuseTextureFormat;
	renderer->CreateTexture(&desc, &mpDiffuseTexture, &mpDiffuseResourceView);

	desc.Format = normalTextureFormat;
	renderer->CreateTexture(&desc, &mpNormalTexture, &mpNormalResourceView);

	desc.Format = roughnessTextureFormat;
	renderer->CreateTexture(&desc, &mpRoughnessTexture, &mpRoughnessResourceView);

	desc.Format = emissiveTextureFormat;
	renderer->CreateTexture(&desc, &mpEmissiveTexture, &mpEmissiveResourceView);

	//Create resource views
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd =
	{
		depthStencilRenderViewFormat,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};

	renderer->device()->CreateDepthStencilView(mpDepthStencilTexture, &dsvd, &mpDepthStencilTargetView);

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		diffuseRenderViewFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	renderer->device()->CreateRenderTargetView(mpDiffuseTexture, &rtsvd, &mpDiffuseTargetView);

	rtsvd.Format = normalRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpNormalTexture, &rtsvd, &mpNormalTargetView);

	rtsvd.Format = roughnessRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpRoughnessTexture, &rtsvd, &mpRoughnessTargetView);

	rtsvd.Format = emissiveRenderViewFormat;
	renderer->device()->CreateRenderTargetView(mpEmissiveTexture, &rtsvd, &mpEmissiveTargetView);

	mWidth = width;
	mHeight = height;

	std::cout << "GBuffer render targets initialized.\n";
}

void GBuffer::bindRenderTargets()
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	renderer->UnbindTextureResources(); //Avoid having render targets bound as input textures

	ID3D11RenderTargetView* renderTargets[4] =
	{
		mpDiffuseTargetView,
		mpNormalTargetView,
		mpRoughnessTargetView,
		mpEmissiveTargetView
	};

	renderer->context()->OMSetRenderTargets(4, renderTargets, mpDepthStencilTargetView);
}

void GBuffer::bindTextures()
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	ID3D11ShaderResourceView* texArray[5] = {
		mpDiffuseResourceView,
		mpNormalResourceView,
		mpRoughnessResourceView,
		mpEmissiveResourceView,
		mpDepthStencilResourceView
	};

	renderer->setTextureResources(texArray, 0, 5);
}

void GBuffer::bindSampler()
{
	EngineStatics::getRenderer()->setSampler(0, mpSamplerState);
}

void GBuffer::clearRenderTargets()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	ID3D11DeviceContext* context = EngineStatics::getRenderer()->context();

	context->ClearDepthStencilView(mpDepthStencilTargetView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->ClearRenderTargetView(mpDiffuseTargetView, clearColor);
	context->ClearRenderTargetView(mpNormalTargetView, clearColor);
	context->ClearRenderTargetView(mpRoughnessTargetView, clearColor);
	context->ClearRenderTargetView(mpEmissiveTargetView, clearColor);
}