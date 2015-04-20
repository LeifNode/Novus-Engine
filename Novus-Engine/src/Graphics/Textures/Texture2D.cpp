#include "Texture2D.h"
#include "Application/Common.h"
#include "Graphics/D3DRenderer.h"
#include "Resources/Texture/DDSTextureLoader.h"

namespace novus
{

Texture2D::Texture2D()
	:
	mWidth(0),
	mHeight(0),
	mpTexture(NULL),
	mpResourceView(NULL),
	mpRenderTargetView(NULL),
	mpUnorderedAccessView(NULL)
{}

Texture2D::~Texture2D()
{
	DeInit();
}

void Texture2D::Init(D3DRenderer* renderer,
	int width,
	int height,
	DXGI_FORMAT format,
	int mipCount,
	UINT bindFlags, 
	UINT miscFlags,
	DXGI_FORMAT srvFormat,
	DXGI_FORMAT rtvFormat,
	DXGI_FORMAT uavFormat)
{
	DeInit();

	mWidth = width;
	mHeight = height;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipCount;
	desc.ArraySize = (miscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) > 0 ? 6 : 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (mipCount > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0) | miscFlags;

	HR(renderer->device()->CreateTexture2D(&desc, NULL, &mpTexture));

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = srvFormat == DXGI_FORMAT_UNKNOWN ? format : srvFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = mipCount;
		srvDesc.Texture2D.MostDetailedMip = 0;

		HR(renderer->device()->CreateShaderResourceView(mpTexture, &srvDesc, &mpResourceView));
	}

	if (bindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = rtvFormat == DXGI_FORMAT_UNKNOWN ? format : rtvFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		HR(renderer->device()->CreateRenderTargetView(mpTexture, &rtvDesc, &mpRenderTargetView));
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = uavFormat == DXGI_FORMAT_UNKNOWN ? format : uavFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		HR(renderer->device()->CreateUnorderedAccessView(mpTexture, &uavDesc, &mpUnorderedAccessView));
	}
}

void Texture2D::DeInit()
{
	ReleaseCOM(mpUnorderedAccessView);
	ReleaseCOM(mpRenderTargetView);
	ReleaseCOM(mpResourceView);
	ReleaseCOM(mpTexture);
}

void Texture2D::Load(D3DRenderer* renderer, const wchar_t* filePath)
{
	ID3D11Resource* textureResource;
	ID3D11ShaderResourceView* textureSRV;

	DirectX::CreateDDSTextureFromFile(renderer->device(), NULL, filePath, &textureResource, &textureSRV);

	if (textureResource == NULL || textureSRV == NULL)
	{
		char errStr[256];
		sprintf(errStr, "Failed to load texture %s.", filePath);

		NE_CRITICAL(errStr, "Texture2D");
		return;
	}

	D3D11_RESOURCE_DIMENSION dimension;
	textureResource->GetType(&dimension);

	if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	{
		ReleaseCOM(textureSRV);
		ReleaseCOM(textureResource);

		NE_CRITICAL("Invalid texture dimension.", "Texture2D");
	}

	mpTexture = static_cast<ID3D11Texture2D*>(textureResource);
	mpResourceView = textureSRV;

	D3D11_TEXTURE2D_DESC texDesc;

	mpTexture->GetDesc(&texDesc);//This should just be switched out for a get file data function when I get DirectXTex set up 

	mWidth = texDesc.Width;
	mHeight = texDesc.Height;
}

void Texture2D::setDebugName(const std::string& name)
{
	if (mpTexture)
		mpTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
}

}//namespace novus