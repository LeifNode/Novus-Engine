#include "Texture2D.h"
#include "Graphics/D3DRenderer.h"

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
	UINT bindFlags)
{
	DeInit();

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = mipCount != 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	renderer->device()->CreateTexture2D(&desc, NULL, &mpTexture);

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = mipCount;
		srvDesc.Texture2D.MostDetailedMip = 0;

		renderer->device()->CreateShaderResourceView(mpTexture, &srvDesc, &mpResourceView);
	}

	if (bindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		renderer->device()->CreateRenderTargetView(mpTexture, &rtvDesc, &mpRenderTargetView);
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		renderer->device()->CreateUnorderedAccessView(mpTexture, &uavDesc, &mpUnorderedAccessView);
	}
}

void Texture2D::DeInit()
{
	ReleaseCOM(mpUnorderedAccessView);
	ReleaseCOM(mpRenderTargetView);
	ReleaseCOM(mpResourceView);
	ReleaseCOM(mpTexture);
}

void Texture2D::setDebugName(const std::string& name)
{
	if (mpTexture)
		mpTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
}

}//namespace novus