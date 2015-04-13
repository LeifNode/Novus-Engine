#include "DeferredRenderer.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/D3DRenderer.h"
#include "PrefilteredEnvironmentMap.h"

namespace novus
{

	DeferredRenderer::DeferredRenderer()
		: mpTiledDeferredShader(NULL),
		mpLUTShader(NULL),
		mpDebugOutputShader(NULL),
		mpPointSampler(NULL),
		mpEnvironmentSampler(NULL),
		mpEnvMap(NULL),
		mpBRDFLUT(NULL),
		mpSourceRenderTarget(NULL),
		mMaxLightCount(1024)
{
	mpHDRRenderTarget = NE_NEW Texture2D();
	mpLightManager = NE_NEW LightManager();
}

DeferredRenderer::~DeferredRenderer()
{
	ReleaseCOM(mpPointSampler);
	ReleaseCOM(mpEnvironmentSampler);
	NE_DELETE(mpLightManager);
	NE_DELETE(mpHDRRenderTarget);
	NE_DELETE(mpEnvMap);
	NE_DELETE(mpBRDFLUT);
}

void DeferredRenderer::Init(D3DRenderer* renderer, int width, int height)
{
	if (mpTiledDeferredShader == NULL) //This is the first init
	{
		ShaderInfo deferredShaderInfo[] = {
			{ ShaderType::Compute, "ComputeShaderTileCS" },
			{ ShaderType::None, NULL }
		};

		mpTiledDeferredShader = renderer->LoadShader(L"../Shaders/Deferred/ComputeTiledDeferred.hlsl", deferredShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);

		ShaderInfo LUTShaderInfo[] = {
			{ ShaderType::Compute, "PreIntegrateBRDFCS" },
			{ ShaderType::None, NULL }
		};

		mpLUTShader = renderer->LoadShader(L"../Shaders/Utils/PreIntegrateBRDF.hlsl", LUTShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);

		ShaderInfo debugShaderInfo[] = {
			{ ShaderType::Vertex, "VS" },
			{ ShaderType::Pixel, "PS" },
			{ ShaderType::None, NULL }
		};

		mpDebugOutputShader = renderer->LoadShader(L"../Shaders/Utils/DebugFullscreenTriangle.hlsl", debugShaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, NULL, 0);

		//mTestPointLights.clear();
		//mTestPointLights.reserve(mMaxLightCount);
		//for (int i = 0; i < mMaxLightCount; i++)
		//{
		//	PointLight light;
		//	light.Color = Vector3(Math::RandF(), Math::RandF(), Math::RandF());
		//	//light.Color = Vector3(1.0f, 1.0f, 1.0f);
		//	light.Intensity = Math::RandF(0.3f, 2.7f) * 0.1f;
		//	light.Radius = 0.0f;

		//	light.Range = sqrt(light.Intensity / 0.002f) - 1.0f + light.Radius;
		//	light.FalloffPow = 1;
		//	light.PositionWorld = Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f)) * 20.0f;
		//	light.PositionWorld.y = Math::RandF(0.8f, 9.5f);

		//	mTestPointLights.push_back(light);
		//}

		D3D11_SAMPLER_DESC samDesc;
		ZeroMemory(&samDesc, sizeof(samDesc));
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.MaxAnisotropy = 1;
		samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HR(renderer->device()->CreateSamplerState(&samDesc, &mpPointSampler));

		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HR(renderer->device()->CreateSamplerState(&samDesc, &mpEnvironmentSampler));

		mpEnvMap = NE_NEW PrefilteredEnvironmentMap();
		mpEnvMap->Init(renderer, L"../Textures/sunsetcube1024.dds");

		mLightBuffer.Init(renderer, mMaxLightCount, D3D11_BIND_SHADER_RESOURCE, true);

		mpBRDFLUT = NE_NEW Texture2D();
		mpBRDFLUT->Init(renderer, 256, 256, DXGI_FORMAT_R16G16_FLOAT, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
		mpBRDFLUT->setDebugName("BRDF LUT");

		ID3D11UnorderedAccessView* texUav = mpBRDFLUT->getUnorderedAccessView();

		renderer->setShader(mpLUTShader);
		renderer->context()->CSSetUnorderedAccessViews(0, 1, &texUav, 0);

		renderer->context()->Dispatch(mpBRDFLUT->getWidth() / 16, mpBRDFLUT->getHeight() / 16, 1);
	}

	mpHDRRenderTarget->Init(renderer, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET);
	mpHDRRenderTarget->setDebugName("HDR Render Target");

	mpSourceRenderTarget = mpHDRRenderTarget;
}

void DeferredRenderer::Update(float dt)
{
	/*for (int i = 0; i < mMaxLightCount; i++)
	{
		mTestPointLights[i].PositionWorld = Matrix3::RotateY(dt * 0.02f) * mTestPointLights[i].PositionWorld;
	}*/

	mpLightManager->ClearLights();
}

void DeferredRenderer::RenderDeferredShading(D3DRenderer* renderer)
{
	float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	renderer->context()->ClearRenderTargetView(mpHDRRenderTarget->getRenderTargetView(), clearColor);

	Matrix4 view = renderer->getPerFrameBuffer()->View;

	//Upload view-space lights
	PointLight* lightBufferPtr = mLightBuffer.Map(renderer);
	for (int i = 0; i < mMaxLightCount; i++)
	{
		if (i < mpLightManager->getLights().size())
		{
			PointLight light = mpLightManager->getLights()[i];
			light.PositionView = Vector3(Vector4(light.PositionWorld, 1.0f) * view);
			light.Range = light.Range = sqrt(light.Intensity / 0.003f) - 1.0f + light.Radius;
			//light.PositionView.z = -light.PositionView.z;

			lightBufferPtr[i] = light;
		}
		else
		{
			lightBufferPtr[i].Intensity = 0.0f;
			lightBufferPtr[i].Range = 0.0f;
		}
	}
	mLightBuffer.Unmap(renderer);

	renderer->setShader(mpTiledDeferredShader);
	renderer->BindPerFrameBuffer(); 
	renderer->getGBuffer()->BindTextures();
	renderer->ResetSamplerState();
	mpEnvMap->Bind(renderer, 5);
	renderer->setTextureResource(7, mpBRDFLUT);

	ID3D11ShaderResourceView* lightSRV = mLightBuffer.getSRV();
	renderer->context()->CSSetShaderResources(6, 1, &lightSRV);

	ID3D11UnorderedAccessView* outputUAV = mpHDRRenderTarget->getUnorderedAccessView();
	renderer->context()->CSSetUnorderedAccessViews(0, 1, &outputUAV, 0);

	//Compute shader tile group dimensions = 16x16
	unsigned int dispatchWidth = (mpHDRRenderTarget->getWidth() + 16 - 1) / 16;
	unsigned int dispatchHeight = (mpHDRRenderTarget->getHeight() + 16 - 1) / 16;

	renderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

	renderer->UnbindTextureResources();
	renderer->UnbindUAVs();
}

void DeferredRenderer::RenderDebugOutput(D3DRenderer* renderer)
{
	renderer->setShader(mpDebugOutputShader);
	renderer->ResetRenderTarget();
	renderer->ResetSamplerState();
	renderer->setSampler(0, mpPointSampler);
	renderer->setTextureResource(0, mpSourceRenderTarget);

	renderer->context()->IASetVertexBuffers(0, 0, 0, 0, 0);
	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	renderer->context()->Draw(3, 0);
}

}