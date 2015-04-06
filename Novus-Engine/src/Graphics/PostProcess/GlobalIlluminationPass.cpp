#include "GlobalIlluminationPass.h"
#include "Graphics/D3DRenderer.h"
#include "Graphics/StructuredBuffer.h"
#include "Application/EngineStatics.h"
#include "Graphics/RenderTargets/ShadowMapRenderTarget.h"

namespace novus
{

GlobalIlluminationPass::GlobalIlluminationPass()
	: mpGIShader(NULL),
	mpDestinationTexture(NULL),
	mpSourceShadowMap(NULL),
	mpSourceGBuffer(NULL),
	mpConstantBuffer(NULL),
	mLightDirection(0.0f, -1.0f, 0.0f)
{
}

GlobalIlluminationPass::~GlobalIlluminationPass()
{
	ReleaseCOM(mpConstantBuffer);
	NE_DELETE(mpDestinationTexture);
}

void GlobalIlluminationPass::Init(int width, int height)
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	InitShader(renderer);
	InitConstantBuffer(renderer);
	InitDestinationTexture(renderer, width, height);
}

void GlobalIlluminationPass::InitShader(D3DRenderer* renderer)
{
	mpGIShader = renderer->getShader("../Shaders/VXGI/GlobalIlluminationPass.hlsl");

	if (mpGIShader == NULL)
	{
		ShaderInfo GIShaderInfo[] = {
			{ ShaderType::Compute, "GlobalIllumEvaluation" },
			{ ShaderType::None, NULL }
		};

		mpGIShader = renderer->LoadShader(L"../Shaders/VXGI/GlobalIlluminationPass.hlsl", GIShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}
}

void GlobalIlluminationPass::InitConstantBuffer(D3DRenderer* renderer)
{
	if (mpConstantBuffer == NULL)
	{
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(CBGlobalIllumination);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		HR(renderer->device()->CreateBuffer(&bd, NULL, &mpConstantBuffer));
	}
}

void GlobalIlluminationPass::InitDestinationTexture(D3DRenderer* renderer, int width, int height)
{
	NE_DELETE(mpDestinationTexture);

	mpDestinationTexture = NE_NEW Texture2D();
	mpDestinationTexture->Init(renderer, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	mpDestinationTexture->setDebugName("Global Illumination RT");
}

void GlobalIlluminationPass::setGBuffer(GBuffer* gbuffer)
{
	mpSourceGBuffer = gbuffer;
}

void GlobalIlluminationPass::setShadowMap(ShadowMapRenderTarget* shadowMap)
{
	mpSourceShadowMap = shadowMap;
}

void GlobalIlluminationPass::setLightDirection(const Vector3& direction)
{
	mLightDirection = direction;
}

void GlobalIlluminationPass::Execute(D3DRenderer* renderer)
{
	if (mpSourceShadowMap == NULL)
	{
		NE_WARN("Shadow map reference is null.", "GlobalIlluminationPass");
		return;
	}
	if (mpSourceGBuffer == NULL)
	{
		NE_WARN("GBuffer reference is null.", "GlobalIlluminationPass");
		return;
	}

	unsigned int dispatchWidth = (mpDestinationTexture->getWidth() + 16 - 1) / 16;
	unsigned int dispatchHeight = (mpDestinationTexture->getHeight() + 16 - 1) / 16;

	D3D11_MAPPED_SUBRESOURCE resource;
	HR(renderer->context()->Map(mpConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	CBGlobalIllumination* constantBuffer = static_cast<CBGlobalIllumination*>(resource.pData);
	constantBuffer->LightColor = Vector4(2.0f);
	constantBuffer->LightDirection = mLightDirection;
	constantBuffer->ShadowIntensity = 1.0f;
	constantBuffer->WorldToShadow = mpSourceShadowMap->getSampleTransform();
	renderer->context()->Unmap(mpConstantBuffer, 0);

	renderer->ResetRenderTarget();
	renderer->setShader(mpGIShader);
	renderer->BindPerFrameBuffer();
	renderer->ResetRenderTarget();
	mpSourceGBuffer->BindTextures();

	renderer->setConstantBuffer(2, mpConstantBuffer);
	renderer->setTextureResource(5, mpSourceShadowMap->getTexture());

	ID3D11UnorderedAccessView* outputUav = mpDestinationTexture->getUnorderedAccessView();

	renderer->context()->CSSetUnorderedAccessViews(0, 1, &outputUav, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

	renderer->UnbindUAVs();
	renderer->UnbindTextureResources();
}

}