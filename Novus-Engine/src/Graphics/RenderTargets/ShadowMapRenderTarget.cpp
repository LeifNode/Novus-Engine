#include "ShadowMapRenderTarget.h"
#include "Graphics/D3DRenderer.h"
#include "Graphics/Textures/Texture2D.h"
#include "Application/EngineStatics.h"
#include "Core/Actor.h"

namespace novus
{

ShadowMapRenderTarget::ShadowMapRenderTarget()
	: mpRenderTarget(NULL),
	mpDepthView(NULL),
	mpRasterizerState(NULL),
	mTransformDirty(true)
{
}

ShadowMapRenderTarget::~ShadowMapRenderTarget()
{
	ReleaseCOM(mpRasterizerState);
	ReleaseCOM(mpDepthView);
	NE_DELETE(mpRenderTarget);
}

void ShadowMapRenderTarget::Init(int width, int height)
{
	ReleaseCOM(mpDepthView);
	NE_DELETE(mpRenderTarget);

	mpRenderTarget = NE_NEW Texture2D();

	mpRenderTarget->Init(EngineStatics::getRenderer(), 
						 width, 
						 height, 
						 DXGI_FORMAT_R32_TYPELESS,
						 1, 
						 D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL, 
						 0,
						 DXGI_FORMAT_R32_FLOAT);
	mpRenderTarget->setDebugName("Shadow Map");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.Flags = 0;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	HR(EngineStatics::getRenderer()->device()->CreateDepthStencilView(mpRenderTarget->getTexture(), &dsvd, &mpDepthView));

	if (mpRasterizerState == NULL)
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FrontCounterClockwise = true;
		//rasterDesc.DepthBias = 12746; //Expected depth bias (0.01 in this case) / 0.0000007845
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.SlopeScaledDepthBias = 1.0f;

		HR(EngineStatics::getRenderer()->device()->CreateRasterizerState(&rasterDesc, &mpRasterizerState));
	}
}

void ShadowMapRenderTarget::BindTarget(D3DRenderer* renderer)
{
	CBPerFrame perFrame;

	perFrame.Projection = mProjectionMatrix;
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	perFrame.ViewProj = getRenderTransform();
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);
	perFrame.View = mViewMatrix;
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	//perFrame.ScreenResolution = Vector2u(mpRenderTarget->getWidth(), mpRenderTarget->getHeight());
	perFrame.EyePosition = mPosition;

	renderer->setPerFrameBuffer(perFrame);

	CD3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(mpRenderTarget->getWidth());
	viewport.Height = static_cast<float>(mpRenderTarget->getHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	renderer->context()->RSSetViewports(1, &viewport);

	renderer->context()->RSSetState(mpRasterizerState);

	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	renderer->context()->OMSetRenderTargets(1, renderTargets, mpDepthView);

	renderer->context()->ClearDepthStencilView(mpDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMapRenderTarget::UnbindTarget(D3DRenderer* renderer)
{
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	renderer->context()->OMSetRenderTargets(1, renderTargets, NULL);
	renderer->ResetRasterizerState();
}

RenderPass::Type ShadowMapRenderTarget::GetRenderPass() const
{
	return RenderPass::Shadow;
}

bool ShadowMapRenderTarget::ShouldRenderActor(const Actor* actor) const
{
	return actor->getCastShadow();
}

void ShadowMapRenderTarget::setPosition(const Vector3& position)
{
	mTransformDirty = true;
	mPosition = position;
}

void ShadowMapRenderTarget::setDirection(const Vector3& direction)
{
	mTransformDirty = true;
	mDirection = direction;
}

void ShadowMapRenderTarget::setVolumePerspectiveBounds(float fovAngleY, float aspectHByW, float nearZ, float farZ)
{
	mTransformDirty = true;
	mProjectionMatrix = Matrix4::Perspective(fovAngleY, aspectHByW, nearZ, farZ);
}

void ShadowMapRenderTarget::setVolumeOrthographicBounds(float width, float height, float depth)
{
	mTransformDirty = true;
	mProjectionMatrix = Matrix4::Orthographic(width, height, 0.0f, depth);
}

Matrix4 ShadowMapRenderTarget::getRenderTransform()
{
	//if (mTransformDirty)
	//{
		mViewMatrix = Matrix4::LookToward(mPosition, -mDirection, Vector3(0.0f, 1.0f, 0.0f));
		mTransform = mViewMatrix * mProjectionMatrix;

		mTransformDirty = false;
	//}

	return mTransform;
}

Matrix4 ShadowMapRenderTarget::getSampleTransform()
{
	//if (mTransformDirty)
	//{
		Matrix4 renderTransform = getRenderTransform();

		Matrix4 clipToSampleTransform = Matrix4(0.5f, 0.0f, 0.0f, 0.0f,
												0.0f, -0.5f, 0.0f, 0.0f,
												0.0f, 0.0f, 1.0f, 0.0f,
												0.5f, 0.5f, 0.0f, 1.0f);

		mSampleTransform = renderTransform * clipToSampleTransform;
	//}

	return mSampleTransform;
}

}