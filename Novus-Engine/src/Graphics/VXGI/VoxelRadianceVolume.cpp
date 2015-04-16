#include "VoxelRadianceVolume.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/D3DRenderer.h"
#include "Application/EngineStatics.h"
#include "Graphics/RenderTargets/VoxelVolumeRenderTarget.h"
#include "Graphics/RenderTargets/ShadowMapRenderTarget.h"

namespace novus
{

VoxelRadianceVolume::VoxelRadianceVolume()
:mpVoxelFilterShader(NULL),
mpRadianceInjectionShader(NULL),
mpVoxelAnisotropicFilterShader(NULL),
mpCopyOcclusionShader(NULL),
mpSourceVolume(NULL),
mpSourceShadowMap(NULL),
mpVoxelRadianceVolume(NULL),
mpDebugPositionOut(NULL),
mpAnisotropicMips(NULL)
{
}

VoxelRadianceVolume::~VoxelRadianceVolume()
{
	NE_DELETE(mpVoxelRadianceVolume);
	NE_DELETE(mpAnisotropicMips);
	NE_DELETE(mpDebugPositionOut);

	for (auto it = mMipUavs.begin(); it != mMipUavs.end(); ++it)
	{
		ReleaseCOM((*it));
	}

	for (auto it = mAnisotropicMipUavs.begin(); it != mAnisotropicMipUavs.end(); ++it)
	{
		ReleaseCOM((*it));
	}
}

void VoxelRadianceVolume::Init(VoxelVolumeRenderTarget* sourceVolume, ShadowMapRenderTarget* sourceShadowMap)
{
	mpSourceVolume = sourceVolume;
	mpSourceShadowMap = sourceShadowMap;

	D3DRenderer* renderer = EngineStatics::getRenderer();

	InitShaders(renderer);
	InitRadianceVolume(renderer);
	InitConstantBuffers(renderer);
	InitMipUAVs(renderer);
}

void VoxelRadianceVolume::InitShaders(D3DRenderer* renderer)
{
	mpRadianceInjectionShader = renderer->getShader("../Shaders/VXGI/InjectRadianceFromDirectionalLight.hlsl");
	if (mpRadianceInjectionShader == NULL)
	{
		ShaderInfo injectionShaderInfo[] = {
			{ ShaderType::Compute, "LightInjectionCS" },
			{ ShaderType::None, NULL }
		};

		mpRadianceInjectionShader = renderer->LoadShader(L"../Shaders/VXGI/InjectRadianceFromDirectionalLight.hlsl", injectionShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}

	mpVoxelFilterShader = renderer->getShader("../Shaders/VXGI/FilerVoxelVolumeMIP.hlsl");
	if (mpVoxelFilterShader == NULL)
	{
		ShaderInfo filterShaderInfo[] = {
			{ ShaderType::Compute, "VoxelVolumeMipMap" },
			{ ShaderType::None, NULL }
		};

		mpVoxelFilterShader = renderer->LoadShader(L"../Shaders/VXGI/FilerVoxelVolumeMIP.hlsl", filterShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}

	mpVoxelAnisotropicFilterShader = renderer->getShader("../Shaders/VXGI/FilerVoxelVolumeMIPAnisotropic.hlsl");
	if (mpVoxelAnisotropicFilterShader == NULL)
	{
		ShaderInfo filterShaderInfo[] = {
			{ ShaderType::Compute, "VoxelVolumeMipMap" },
			{ ShaderType::None, NULL }
		};

		mpVoxelAnisotropicFilterShader = renderer->LoadShader(L"../Shaders/VXGI/FilerVoxelVolumeMIPAnisotropic.hlsl", filterShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}

	mpVoxelFilterBaseMipShader = renderer->getShader("../Shaders/VXGI/FilterVoxelBaseMIPAnisotropic.hlsl");
	if (mpVoxelFilterBaseMipShader == NULL)
	{
		ShaderInfo filterShaderInfo[] = {
			{ ShaderType::Compute, "VoxelVolumeMipMap" },
			{ ShaderType::None, NULL }
		};

		mpVoxelFilterBaseMipShader = renderer->LoadShader(L"../Shaders/VXGI/FilterVoxelBaseMIPAnisotropic.hlsl", filterShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}

	mpCopyOcclusionShader = renderer->getShader("../Shaders/VXGI/CopyVoxelOcclusion.hlsl");
	if (mpCopyOcclusionShader == NULL)
	{
		ShaderInfo copyShaderInfo[] = {
			{ ShaderType::Compute, "CopyVoxelOcclusionCS" },
			{ ShaderType::None, NULL }
		};

		mpCopyOcclusionShader = renderer->LoadShader(L"../Shaders/VXGI/CopyVoxelOcclusion.hlsl", copyShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}
}

void VoxelRadianceVolume::InitRadianceVolume(D3DRenderer* renderer)
{
	int resolution = mpSourceVolume->getResolution();

	mpVoxelRadianceVolume = NE_NEW Texture3D();
	mpVoxelRadianceVolume->Init(renderer,
								resolution,
								resolution,
								resolution,
								DXGI_FORMAT_R8G8B8A8_TYPELESS,
								Math::MipMapCount(resolution, resolution),
								D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS,
								0,
								DXGI_FORMAT_R8G8B8A8_UNORM,
								DXGI_FORMAT_R8G8B8A8_UNORM,
								DXGI_FORMAT_R32_UINT);

	mpVoxelRadianceVolume->setDebugName("Radiance Voxel Volume");

	int halfResolution = resolution / 2;

	mpAnisotropicMips = NE_NEW Texture3D();
	mpAnisotropicMips->Init(renderer,
		halfResolution * 6,
		halfResolution,
		halfResolution,
		DXGI_FORMAT_R8G8B8A8_TYPELESS,
		Math::MipMapCount(halfResolution, halfResolution),
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS,
		0,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R32_UINT);

	mpAnisotropicMips->setDebugName("Radiance Voxel Volume Anisotropic Mips");

	mpDebugPositionOut = NE_NEW Texture2D();
	mpDebugPositionOut->Init(renderer,
		mpSourceShadowMap->getTexture()->getWidth(),
		mpSourceShadowMap->getTexture()->getHeight(),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		1,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS);

	mpDebugPositionOut->setDebugName("World Position Out Texture");
}

void VoxelRadianceVolume::InitConstantBuffers(D3DRenderer* renderer)
{
	mRadianceInjectionBuffer.Init(renderer);
	mFilterBuffer.Init(renderer);
}

void VoxelRadianceVolume::InitMipUAVs(D3DRenderer* renderer)
{
	int mipCount = Math::MipMapCount(mpSourceVolume->getResolution(), mpSourceVolume->getResolution());

	for (int i = 0; i < mipCount; i++)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R32_UINT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.MipSlice = i;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.WSize = -1;

		ID3D11UnorderedAccessView* mipUav;

		HR(renderer->device()->CreateUnorderedAccessView(mpVoxelRadianceVolume->getTexture(), &uavDesc, &mipUav));

		mMipUavs.push_back(mipUav);
	}

	for (int i = 0; i < mipCount - 1; i++)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R32_UINT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.MipSlice = i;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.WSize = -1;

		ID3D11UnorderedAccessView* mipUav;

		HR(renderer->device()->CreateUnorderedAccessView(mpAnisotropicMips->getTexture(), &uavDesc, &mipUav));

		mAnisotropicMipUavs.push_back(mipUav);
	}
}

void VoxelRadianceVolume::CopyVoxelResource()
{

}

void VoxelRadianceVolume::ClearColor(D3DRenderer* renderer)
{
	Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	renderer->context()->ClearRenderTargetView(mpVoxelRadianceVolume->getRenderTargetView(), &clearColor.x);

	ID3D11UnorderedAccessView* sourceUav = mpSourceVolume->getTexture()->getUnorderedAccessView();
	ID3D11UnorderedAccessView* targetUav = mpVoxelRadianceVolume->getUnorderedAccessView();

	unsigned int dispatchWidth = (mpVoxelRadianceVolume->getWidth() + 8 - 1) / 8;
	unsigned int dispatchHeight = (mpVoxelRadianceVolume->getHeight() + 8 - 1) / 8;
	unsigned int dispatchDepth = (mpVoxelRadianceVolume->getDepth() + 8 - 1) / 8;

	ID3D11UnorderedAccessView* outputUavs[] =
	{
		sourceUav,
		targetUav
	};

	renderer->setShader(mpCopyOcclusionShader);

	renderer->context()->CSSetUnorderedAccessViews(0, 2, outputUavs, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, dispatchDepth);

	ID3D11UnorderedAccessView* nullUavs[] =
	{
		NULL,
		NULL
	};

	renderer->context()->CSSetUnorderedAccessViews(0, 2, nullUavs, 0);
}

void VoxelRadianceVolume::InjectRadiance(D3DRenderer* renderer)
{
	ClearColor(renderer);

	unsigned int dispatchWidth = (mpSourceShadowMap->getTexture()->getWidth() + 16 - 1) / 16;
	unsigned int dispatchHeight = (mpSourceShadowMap->getTexture()->getHeight() + 16 - 1) / 16;

	CBLightInjectionPass injectionCB;
	injectionCB.LightColor = Vector3(1.0f, 0.8952f, 0.8666f);
	injectionCB.LightDirection = mpSourceShadowMap->getDirection();
	injectionCB.LightIntensity = 1.0f;
	injectionCB.ShadowMapDimensions = Vector2(mpSourceShadowMap->getTexture()->getWidth(), mpSourceShadowMap->getTexture()->getHeight());
	injectionCB.ShadowToWorld = Matrix4::Inverse(mpSourceShadowMap->getRenderTransform());
	injectionCB.ShadowToVoxelVolume = injectionCB.ShadowToWorld * mpSourceVolume->getWorldToVolume();
	injectionCB.VoxelVolumeDimensions = Vector3i(mpSourceVolume->getResolution());

	CBLightInjectionPass* constantBuffer = mRadianceInjectionBuffer.Map(renderer);

	*constantBuffer = injectionCB;
	mRadianceInjectionBuffer.Unmap(renderer);

	renderer->setShader(mpRadianceInjectionShader);
	renderer->BindPerFrameBuffer();
	renderer->ResetRenderTarget();
	mRadianceInjectionBuffer.Bind(3, renderer);

	ID3D11UnorderedAccessView* outputUavs[] =
	{
		mpVoxelRadianceVolume->getUnorderedAccessView(),
		mpDebugPositionOut->getUnorderedAccessView()
	};

	renderer->setTextureResource(0, mpSourceShadowMap->getTexture());
	renderer->setTextureResource(1, mpSourceVolume->getTexture());
	renderer->setTextureResource(2, mpSourceVolume->getNormalTexture());
	renderer->context()->CSSetUnorderedAccessViews(0, 2, outputUavs, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

	renderer->UnbindUAVs();

	FilterMips(renderer);
}

void VoxelRadianceVolume::FilterMips(D3DRenderer* renderer)
{
	for (int i = 1; i < mMipUavs.size(); i++)
	{
		FilerMipLevel(renderer, i);
	}

	FilterBaseMip(renderer);

	for (int i = 1; i < mAnisotropicMipUavs.size(); i++)
	{
		FilerAnisotropicMipLevel(renderer, i);
	}
}

void VoxelRadianceVolume::FilerMipLevel(D3DRenderer* renderer, int level)
{
	if (level == 0 || level >= mMipUavs.size())
		return;

	ID3D11UnorderedAccessView* sourceUav = mMipUavs[level - 1];
	ID3D11UnorderedAccessView* targetUav = mMipUavs[level];

	unsigned int mipFraction = pow(2, level);

	unsigned int dispatchWidth = (mpVoxelRadianceVolume->getWidth() / mipFraction + 8 - 1) / 8;
	unsigned int dispatchHeight = (mpVoxelRadianceVolume->getHeight() / mipFraction + 8 - 1) / 8;
	unsigned int dispatchDepth = (mpVoxelRadianceVolume->getDepth() / mipFraction + 8 - 1) / 8;

	ID3D11UnorderedAccessView* outputUavs[] =
	{
		sourceUav,
		targetUav
	};

	renderer->setShader(mpVoxelFilterShader);

	renderer->context()->CSSetUnorderedAccessViews(0, 2, outputUavs, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, dispatchDepth);
}

void VoxelRadianceVolume::FilterBaseMip(D3DRenderer* renderer)
{
	ID3D11UnorderedAccessView* sourceUav = mpVoxelRadianceVolume->getUnorderedAccessView();
	ID3D11UnorderedAccessView* targetUav = mAnisotropicMipUavs[0];

	unsigned int mipFraction = 2;

	unsigned int dispatchWidth = (mpVoxelRadianceVolume->getWidth() / mipFraction + 8 - 1) / 8;
	unsigned int dispatchHeight = (mpVoxelRadianceVolume->getHeight() / mipFraction + 8 - 1) / 8;
	unsigned int dispatchDepth = (mpVoxelRadianceVolume->getDepth() / mipFraction + 8 - 1) / 8;

	CBVoxelGI* buffer = mFilterBuffer.Map(renderer);
	ZeroMemory(buffer, sizeof(CBVoxelGI));

	buffer->VoxelVolumeDim = Vector3i(mpVoxelRadianceVolume->getWidth() / mipFraction);
	mFilterBuffer.Unmap(renderer);

	ID3D11UnorderedAccessView* outputUavs[] =
	{
		sourceUav,
		targetUav
	};

	renderer->setShader(mpVoxelFilterBaseMipShader);

	mFilterBuffer.Bind(2, renderer);
	renderer->context()->CSSetUnorderedAccessViews(0, 2, outputUavs, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, dispatchDepth);
}

void VoxelRadianceVolume::FilerAnisotropicMipLevel(D3DRenderer* renderer, int level)
{
	if (level == 0 || level >= mAnisotropicMipUavs.size())
		return;

	ID3D11UnorderedAccessView* sourceUav = mAnisotropicMipUavs[level - 1];
	ID3D11UnorderedAccessView* targetUav = mAnisotropicMipUavs[level];

	unsigned int mipFraction = pow(2, level);

	unsigned int dispatchWidth = ((mpAnisotropicMips->getWidth() / mipFraction) / 6 + 8 - 1) / 8;
	unsigned int dispatchHeight = (mpAnisotropicMips->getHeight() / mipFraction + 8 - 1) / 8;
	unsigned int dispatchDepth = (mpAnisotropicMips->getDepth() / mipFraction + 8 - 1) / 8;

	CBVoxelGI* buffer = mFilterBuffer.Map(renderer);
	ZeroMemory(buffer, sizeof(CBVoxelGI));

	buffer->VoxelVolumeDim = Vector3i(mpVoxelRadianceVolume->getWidth() / (mipFraction * 2));
	mFilterBuffer.Unmap(renderer);

	ID3D11UnorderedAccessView* outputUavs[] =
	{
		sourceUav,
		targetUav
	};

	renderer->setShader(mpVoxelAnisotropicFilterShader);

	mFilterBuffer.Bind(2, renderer);

	renderer->context()->CSSetUnorderedAccessViews(0, 2, outputUavs, 0);

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, dispatchDepth);
}

}