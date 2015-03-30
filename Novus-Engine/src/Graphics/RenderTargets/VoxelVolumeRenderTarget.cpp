#include "VoxelVolumeRenderTarget.h"
#include "Graphics/Textures/Texture3D.h"
#include "Application/EngineStatics.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

VoxelVolumeRenderTarget::VoxelVolumeRenderTarget()
: mpVoxelTexture(NULL),
mResolution(0),
mBounds(1.0f)
{

}

VoxelVolumeRenderTarget::~VoxelVolumeRenderTarget()
{
	NE_DELETE(mpVoxelTexture);
}

void VoxelVolumeRenderTarget::Init(int resolution)
{
	NE_DELETE(mpVoxelTexture);

	mResolution = resolution;

	D3DRenderer* renderer = EngineStatics::getRenderer();

	mpVoxelTexture = NE_NEW Texture3D();
	mpVoxelTexture->Init(renderer,
						 resolution,
						 resolution,
						 resolution,
						 DXGI_FORMAT_R8G8B8A8_UNORM,
						 Math::MipMapCount(resolution, resolution),
						 D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS);

	mpVoxelTexture->setDebugName("Voxel Volume");
}

void VoxelVolumeRenderTarget::setBounds(float width, float height, float depth)
{
	mProjection = Matrix4::OrthographicOffCenter(width / 2.0f, -width / 2.0f, -height / 2.0f, height / 2.0f, -depth / 2.0f, depth / 2.0f);
}

void VoxelVolumeRenderTarget::BuildWorldToVolumeProj()
{
	mWorldToVolume[0][0] = (1.0f / mBounds.x) * 2.0f;
	mWorldToVolume[0][1] = 0.0f;
	mWorldToVolume[0][2] = 0.0f;
	mWorldToVolume[0][3] = 0.0f;

	mWorldToVolume[1][0] = 0.0f;
	mWorldToVolume[1][1] = (1.0f / mBounds.y) * 2.0f;
	mWorldToVolume[1][2] = 0.0f;
	mWorldToVolume[1][3] = 0.0f;

	mWorldToVolume[2][0] = 0.0f;
	mWorldToVolume[2][1] = 0.0f;
	mWorldToVolume[2][2] = (1.0f / mBounds.z) * 2.0f;
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

	perFrame.ScreenResolution = Vector2_t<unsigned int>(static_cast<unsigned int>(mResolution), static_cast<unsigned int>(mResolution));
	perFrame.ClipNearFar = Vector2(0.0f, 1.0f);
	perFrame.Projection = mProjection;
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	perFrame.View = Matrix4::Translate(-mPosition);
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);

	renderer->setPerFrameBuffer(perFrame);

	ID3D11UnorderedAccessView* voxelUAV = mpVoxelTexture->getUnorderedAccessView();
	renderer->context()->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 1, &voxelUAV, 0);
	renderer->setViewport(0, 0, mResolution, mResolution);
}

void VoxelVolumeRenderTarget::UnbindTarget(D3DRenderer* renderer)
{
	//TODO: Unbind UAV?
}

RenderPass::Type VoxelVolumeRenderTarget::GetRenderPass() const
{
	return RenderPass::GraphicsPrepass;
}

bool VoxelVolumeRenderTarget::ShouldRender(const Actor* actor) const
{
	return true;
}

}