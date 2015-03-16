#include "Texture3D.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

Texture3D::Texture3D()
	:
	mWidth(0),
	mHeight(0),
	mpTexture(NULL),
	mpResourceView(NULL),
	mpRenderTargetView(NULL),
	mpUnorderedAccessView(NULL)
{}

Texture3D::~Texture3D()
{
	DeInit();
}

void Texture3D::Init(D3DRenderer* renderer,
	int width,
	int height,
	int depth,
	DXGI_FORMAT format,
	int mipCount,
	UINT bindFlags,
	UINT miscFlags)
{
	DeInit();

	mWidth = width;
	mHeight = height;
	mDepth = depth;

	D3D11_TEXTURE3D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = mipCount;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (mipCount > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0) | miscFlags;

	renderer->device()->CreateTexture3D(&desc, NULL, &mpTexture);

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = mipCount;
		srvDesc.Texture3D.MostDetailedMip = 0;

		renderer->device()->CreateShaderResourceView(mpTexture, &srvDesc, &mpResourceView);
	}

	if (bindFlags & D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtvDesc.Texture3D.MipSlice = 0;
		rtvDesc.Texture3D.FirstWSlice = 0;
		rtvDesc.Texture3D.WSize = -1;

		renderer->device()->CreateRenderTargetView(mpTexture, &rtvDesc, &mpRenderTargetView);
	}

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.WSize = -1;

		renderer->device()->CreateUnorderedAccessView(mpTexture, &uavDesc, &mpUnorderedAccessView);
	}
}

void Texture3D::DeInit()
{
	ReleaseCOM(mpUnorderedAccessView);
	ReleaseCOM(mpRenderTargetView);
	ReleaseCOM(mpResourceView);
	ReleaseCOM(mpTexture);
}

void Texture3D::setDebugName(const std::string& name)
{
	if (mpTexture)
		mpTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
}

}