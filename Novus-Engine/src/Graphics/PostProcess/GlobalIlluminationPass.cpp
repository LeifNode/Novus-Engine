#include "GlobalIlluminationPass.h"
#include "Graphics/D3DRenderer.h"
#include "Graphics/StructuredBuffer.h"
#include "Application/EngineStatics.h"
#include "Graphics/RenderTargets/ShadowMapRenderTarget.h"
#include "Graphics/RenderTargets/VoxelVolumeRenderTarget.h"
#include "Graphics/VXGI/VoxelRadianceVolume.h"

namespace novus
{

GlobalIlluminationPass::GlobalIlluminationPass()
	: mpGIShader(NULL),
	mpDestinationTexture(NULL),
	mpSourceShadowMap(NULL),
	mpSourceGBuffer(NULL),
	mpConstantBuffer(NULL),
	mpVXGIBuffer(NULL),
	mpConeTracingSamplerState(NULL),
	mpRadianceVolume(NULL),
	mLightDirection(0.0f, -1.0f, 0.0f)
{
}

GlobalIlluminationPass::~GlobalIlluminationPass()
{
	ReleaseCOM(mpConeTracingSamplerState);
	ReleaseCOM(mpConstantBuffer);
	ReleaseCOM(mpVXGIBuffer);
	NE_DELETE(mpDestinationTexture);
}

void GlobalIlluminationPass::Init(int width, int height)
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	InitShader(renderer);
	InitConstantBuffers(renderer);
	InitDestinationTexture(renderer, width, height);
	InitSamplerState(renderer);
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

void GlobalIlluminationPass::InitConstantBuffers(D3DRenderer* renderer)
{
	if (mpConstantBuffer == NULL)
	{
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(CBShadowPass);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		HR(renderer->device()->CreateBuffer(&bd, NULL, &mpConstantBuffer));
	}

	if (mpVXGIBuffer == NULL)
	{
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(CBVoxelGI);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		HR(renderer->device()->CreateBuffer(&bd, NULL, &mpVXGIBuffer));
	}
}

void GlobalIlluminationPass::InitDestinationTexture(D3DRenderer* renderer, int width, int height)
{
	NE_DELETE(mpDestinationTexture);

	mpDestinationTexture = NE_NEW Texture2D();
	mpDestinationTexture->Init(renderer, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	mpDestinationTexture->setDebugName("Global Illumination RT");
}

void GlobalIlluminationPass::InitSamplerState(D3DRenderer* renderer)
{
	if (mpConeTracingSamplerState == NULL)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxAnisotropy = 0;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 0.0f;
		sampDesc.BorderColor[1] = 0.0f;
		sampDesc.BorderColor[2] = 0.0f;
		sampDesc.BorderColor[3] = 1.0f;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HR(renderer->device()->CreateSamplerState(&sampDesc, &mpConeTracingSamplerState));
	}
}

void GlobalIlluminationPass::setGBuffer(GBuffer* gbuffer)
{
	mpSourceGBuffer = gbuffer;
}

void GlobalIlluminationPass::setShadowMap(ShadowMapRenderTarget* shadowMap)
{
	mpSourceShadowMap = shadowMap;
}

void GlobalIlluminationPass::setVoxelVolume(VoxelRadianceVolume* voxelVolume)
{
	mpRadianceVolume = voxelVolume;
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
	CBShadowPass* constantBuffer = static_cast<CBShadowPass*>(resource.pData);
	constantBuffer->LightColor = Vector4(2.0f);
	constantBuffer->LightDirection = mLightDirection;
	constantBuffer->ShadowIntensity = 1.0f;
	constantBuffer->WorldToShadow = mpSourceShadowMap->getSampleTransform();
	renderer->context()->Unmap(mpConstantBuffer, 0);

	HR(renderer->context()->Map(mpVXGIBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	CBVoxelGI* vxgiBuffer = static_cast<CBVoxelGI*>(resource.pData);

	VoxelVolumeRenderTarget* voxelVolume = mpRadianceVolume->getSourceVolume();

	int volumeResolution = voxelVolume->getResolution();
	float voxelScale = voxelVolume->getBounds().x * (1.0f / static_cast<float>(volumeResolution));

	vxgiBuffer->WorldToVoxelVolume = voxelVolume->getWorldToVolume();
	vxgiBuffer->VoxelVolumeDim = Vector3i(volumeResolution);
	vxgiBuffer->VoxelMipCount = Math::MipMapCount(volumeResolution, volumeResolution) - 1;
	vxgiBuffer->VoxelScale = voxelScale * 0.5f;

	renderer->context()->Unmap(mpVXGIBuffer, 0);

	renderer->ResetRenderTarget();
	renderer->setShader(mpGIShader);
	renderer->BindPerFrameBuffer();
	renderer->ResetRenderTarget();
	mpSourceGBuffer->BindTextures();

	renderer->setConstantBuffer(2, mpVXGIBuffer);
	renderer->setConstantBuffer(3, mpConstantBuffer);

	renderer->setTextureResource(5, mpSourceShadowMap->getTexture());
	renderer->setTextureResource(6, mpRadianceVolume->getRadianceVolume());

	ID3D11UnorderedAccessView* outputUav = mpDestinationTexture->getUnorderedAccessView();

	renderer->context()->CSSetUnorderedAccessViews(0, 1, &outputUav, 0);
	renderer->setSampler(1, mpConeTracingSamplerState);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

	renderer->UnbindUAVs();
	renderer->UnbindTextureResources();
	renderer->ResetSamplerState();
}

}