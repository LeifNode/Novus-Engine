#include "GBufferRenderTarget.h"
#include "Graphics/GBuffer.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

GBufferRenderTarget::GBufferRenderTarget(bool primaryRenderTarget)
{
	mpGBuffer = NE_NEW GBuffer();
}

GBufferRenderTarget::~GBufferRenderTarget()
{
	NE_DELETE(mpGBuffer);
}

void GBufferRenderTarget::Init(int width, int height)
{
	mpGBuffer->Init(width, height);
}

void GBufferRenderTarget::BindTarget(D3DRenderer* renderer)
{
	mpGBuffer->ClearRenderTargets();
	mpGBuffer->BindRenderTargets();
}

void GBufferRenderTarget::UnbindTarget(D3DRenderer* renderer)
{
	renderer->ResetRenderTarget();
}

RenderPass::Type GBufferRenderTarget::GetRenderPass() const
{
	return mIsPrimaryRenderTarget ? RenderPass::Final : RenderPass::RenderTargets;
}

bool GBufferRenderTarget::ShouldRenderActor(const Actor* actor) const
{
	return true;
}

}