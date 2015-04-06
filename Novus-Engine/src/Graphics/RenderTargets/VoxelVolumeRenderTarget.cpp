#include "VoxelVolumeRenderTarget.h"
#include "Graphics/Textures/Texture3D.h"
#include "Application/EngineStatics.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

VoxelVolumeRenderTarget::VoxelVolumeRenderTarget()
: mpVoxelTexture(NULL),
mpNormalTexture(NULL),
mpEmissiveTexture(NULL),
mpRasterizerState(NULL),
mResolution(0),
mBounds(10.0f)
{
	setBounds(10.0f, 10.0f, 10.0f);
}

VoxelVolumeRenderTarget::~VoxelVolumeRenderTarget()
{
	ReleaseCOM(mpRasterizerState);
	NE_DELETE(mpVoxelTexture);
	NE_DELETE(mpNormalTexture);
	NE_DELETE(mpEmissiveTexture);
}

void VoxelVolumeRenderTarget::Init(int resolution)
{
	NE_DELETE(mpVoxelTexture);
	NE_DELETE(mpNormalTexture);
	NE_DELETE(mpEmissiveTexture);

	mResolution = resolution;

	D3DRenderer* renderer = EngineStatics::getRenderer();

	mpVoxelTexture = NE_NEW Texture3D();
	mpVoxelTexture->Init(renderer,
						 resolution,
						 resolution,
						 resolution,
						 DXGI_FORMAT_R8G8B8A8_TYPELESS,
						 //Math::MipMapCount(resolution, resolution),
						 1,
						 D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS,
						 0,
						 DXGI_FORMAT_R8G8B8A8_UNORM,
						 DXGI_FORMAT_R8G8B8A8_UNORM,
						 DXGI_FORMAT_R32_UINT);

	mpVoxelTexture->setDebugName("Diffuse Voxel Volume");

	mpNormalTexture = NE_NEW Texture3D();
	mpNormalTexture->Init(renderer,
						  resolution,
						  resolution,
						  resolution,
						  DXGI_FORMAT_R8G8B8A8_TYPELESS,
						  //Math::MipMapCount(resolution, resolution),
						  1,
						  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS,
						  0,
						  DXGI_FORMAT_R8G8B8A8_UNORM,
						  DXGI_FORMAT_R8G8B8A8_UNORM,
						  DXGI_FORMAT_R32_UINT);

	mpNormalTexture->setDebugName("Normal Voxel Volume");

	mpEmissiveTexture = NE_NEW Texture3D();
	mpEmissiveTexture->Init(renderer,
							resolution,
							resolution,
							resolution,
							DXGI_FORMAT_R8G8B8A8_TYPELESS,
							//Math::MipMapCount(resolution, resolution),
							1,
							D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS,
							0,
							DXGI_FORMAT_R8G8B8A8_UNORM,
							DXGI_FORMAT_R8G8B8A8_UNORM,
							DXGI_FORMAT_R32_UINT);

	mpEmissiveTexture->setDebugName("Emissive Voxel Volume");

	if (mpRasterizerState == NULL)
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FrontCounterClockwise = true;

		HR(EngineStatics::getRenderer()->device()->CreateRasterizerState(&rasterDesc, &mpRasterizerState));
	}
}

void VoxelVolumeRenderTarget::setBounds(float width, float height, float depth)
{
	//mProjection = Matrix4::OrthographicOffCenter(width / 2.0f, -width / 2.0f, -height / 2.0f, height / 2.0f, -depth / 2.0f, depth / 2.0f);

	mBounds = Vector3(width, height, depth);

	BuildWorldToVolumeProj();

	mProjection = Matrix4::Orthographic(2.0f, 2.0f, 1.0f, -1.0f);
	//mProjection = mWorldToVolume * Matrix4::OrthographicOffCenter(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

void VoxelVolumeRenderTarget::BuildWorldToVolumeProj()
{
	mWorldToVolume[0][0] = 2.0f / mBounds.x;
	mWorldToVolume[0][1] = 0.0f;
	mWorldToVolume[0][2] = 0.0f;
	mWorldToVolume[0][3] = 0.0f;

	mWorldToVolume[1][0] = 0.0f;
	mWorldToVolume[1][1] = 2.0f / mBounds.y;
	mWorldToVolume[1][2] = 0.0f;
	mWorldToVolume[1][3] = 0.0f;

	mWorldToVolume[2][0] = 0.0f;
	mWorldToVolume[2][1] = 0.0f;
	mWorldToVolume[2][2] = 2.0f / mBounds.z;
	mWorldToVolume[2][3] = 0.0f;

	mWorldToVolume[3] = Vector4(mPosition, 1.0f);
}

void VoxelVolumeRenderTarget::setPosition(const Vector3& position)
{
	mPosition = position;
}

void VoxelVolumeRenderTarget::BindTarget(D3DRenderer* renderer)
{
	CBPerFrame perFrame;

	Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	renderer->context()->ClearRenderTargetView(mpVoxelTexture->getRenderTargetView(), &clearColor.x);
	renderer->context()->ClearRenderTargetView(mpNormalTexture->getRenderTargetView(), &clearColor.x);
	renderer->context()->ClearRenderTargetView(mpEmissiveTexture->getRenderTargetView(), &clearColor.x);

	perFrame.ScreenResolution = Vector2_t<unsigned int>(static_cast<unsigned int>(mResolution), static_cast<unsigned int>(mResolution));
	perFrame.ClipNearFar = Vector2(0.0f, 1.0f);
	perFrame.Projection = mProjection;
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	perFrame.View = mWorldToVolume;
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);

	renderer->setPerFrameBuffer(perFrame);
	renderer->UnbindTextureResources();

	ID3D11UnorderedAccessView* voxelUAVs[] =
	{
		mpVoxelTexture->getUnorderedAccessView(),
		mpNormalTexture->getUnorderedAccessView(),
		mpEmissiveTexture->getUnorderedAccessView()
	};

	renderer->context()->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 3, voxelUAVs, 0);
	renderer->setViewport(0, 0, mResolution, mResolution);
	renderer->context()->RSSetState(mpRasterizerState);
}

void VoxelVolumeRenderTarget::UnbindTarget(D3DRenderer* renderer)
{
	ID3D11UnorderedAccessView* voxelUAVs[] =
	{
		NULL,
		NULL,
		NULL
	};

	renderer->context()->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 3, voxelUAVs, 0);//This is probably not necessary
	renderer->ResetRasterizerState();
}

RenderPass::Type VoxelVolumeRenderTarget::GetRenderPass() const
{
	return RenderPass::GraphicsPrepass;
}

bool VoxelVolumeRenderTarget::ShouldRenderActor(const Actor* actor) const
{
	return true;
}

}