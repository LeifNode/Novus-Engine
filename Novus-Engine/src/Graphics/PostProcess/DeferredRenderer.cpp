#include "DeferredRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

DeferredRenderer::DeferredRenderer()
	: mpTiledDeferredShader(NULL),
	mpDebugOutputShader(NULL),
	mpPointSampler(NULL)
{
	mpHDRRenderTarget = new Texture2D();
}

DeferredRenderer::~DeferredRenderer()
{
	ReleaseCOM(mpPointSampler);
	NE_DELETE(mpHDRRenderTarget);
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

		ShaderInfo debugShaderInfo[] = {
			{ ShaderType::Vertex, "VS" },
			{ ShaderType::Pixel, "PS" },
			{ ShaderType::None, NULL }
		};

		mpDebugOutputShader = renderer->LoadShader(L"../Shaders/Utils/DebugFullscreenTriangle.hlsl", debugShaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, NULL, 0);

		mTestPointLights.clear();
		mTestPointLights.reserve(1028);
		for (int i = 0; i < 1028; i++)
		{
			PointLight light;
			light.Color = Vector3(Math::RandF(0.0f, 1.0f), Math::RandF(0.0f, 1.0f), Math::RandF(0.0f, 1.0f));
			light.Intensity = 1.5f; 
			light.Range = sqrt(light.Intensity / 0.0001f) - 1.0f;
			light.FalloffPow = 1;
			light.Radius = 0.0f;
			light.PositionView = Vector3(Math::RandF(-1.0f, 1.0f), 0.01f, Math::RandF(-1.0f, 1.0f)) * 50.0f;

			mTestPointLights.push_back(light);
		}

		D3D11_SAMPLER_DESC samDesc;
		ZeroMemory(&samDesc, sizeof(samDesc));
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.MaxAnisotropy = 1;
		samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samDesc.MaxLOD = D3D11_FLOAT32_MAX;

		renderer->device()->CreateSamplerState(&samDesc, &mpPointSampler);
	}

	mpHDRRenderTarget->Init(renderer, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET);
	mpHDRRenderTarget->setDebugName("HDR Render Target");

	mLightBuffer.Init(renderer, 1028, D3D11_BIND_SHADER_RESOURCE, true);
}

void DeferredRenderer::Update(float dt)
{
	for (int i = 0; i < 1028; i++)
	{
		mTestPointLights[i].PositionView = Matrix3::RotateY(dt * 0.2f) * mTestPointLights[i].PositionView;
	}
}

void DeferredRenderer::RenderDeferredShading(D3DRenderer* renderer)
{
	float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	renderer->context()->ClearRenderTargetView(mpHDRRenderTarget->getRenderTargetView(), clearColor);

	Matrix4 view = renderer->getPerFrameBuffer()->View;

	//Upload view-space lights
	PointLight* lightBufferPtr = mLightBuffer.Map(renderer);
	for (int i = 0; i < 1028; i++)
	{
		PointLight light = mTestPointLights[i];
		light.PositionView = Vector3(Vector4(light.PositionView, 1.0f) * view);
		light.PositionView.z = -light.PositionView.z;

		lightBufferPtr[i] = light;
	}
	mLightBuffer.Unmap(renderer);

	renderer->setShader(mpTiledDeferredShader);
	renderer->BindPerFrameBuffer(); 
	renderer->getGBuffer()->BindTextures();
	ID3D11ShaderResourceView* lightSRV = mLightBuffer.getSRV();
	renderer->context()->CSSetShaderResources(5, 1, &lightSRV);

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
	renderer->setTextureResource(0, mpHDRRenderTarget);

	renderer->context()->IASetVertexBuffers(0, 0, 0, 0, 0);
	renderer->context()->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
	renderer->context()->Draw(3, 0);
}

}