#include "SkyboxRenderer.h"
#include "D3DRenderer.h"
#include "Shaders/Shader.h"
#include "Textures/Texture2D.h"
#include "Application/EngineStatics.h"
#include "Geometry.h"

namespace novus
{

SkyboxRenderer::SkyboxRenderer()
:mpCubemap(NULL),
mpSkyboxShader(NULL),
mpMeshRenderer(NULL),
mpSamplerState(NULL)
{}

SkyboxRenderer::~SkyboxRenderer()
{
	ReleaseCOM(mpSamplerState);
	NE_DELETE(mpCubemap);
	NE_DELETE(mpMeshRenderer);
}

void SkyboxRenderer::Init(const wchar_t* cubemapPath)
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	mpCubemap = NE_NEW Texture2D();
	mpCubemap->Load(renderer, cubemapPath);

	mpMeshRenderer = NE_NEW MeshRenderer<Vector3>();

	std::vector<Vector3> vertices;
	std::vector<UINT> indices;

	GeometryGenerator::CreateSkybox(vertices, indices);

	mpMeshRenderer->Init(renderer, vertices, indices);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(renderer->device()->CreateSamplerState(&sampDesc, &mpSamplerState));

	mpSkyboxShader = renderer->getShader("../Shaders/Skybox.hlsl");

	if (mpSkyboxShader == NULL)
	{
		ShaderInfo shaderInfo[] = {
			{ ShaderType::Vertex, "VS" },
			{ ShaderType::Pixel, "PS" },
			{ ShaderType::None, NULL }
		};

		D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		mpSkyboxShader = renderer->LoadShader(L"../Shaders/Skybox.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription));
	}
}

void SkyboxRenderer::Render(D3DRenderer* renderer)
{
	renderer->setShader(mpSkyboxShader);
	renderer->setSampler(0, mpSamplerState);
	renderer->setTextureResource(0, mpCubemap);

	CBPerObject perObject;
	perObject.World = Matrix4(1.0f);
	perObject.WorldInvTranspose = Matrix4(1.0f);
	perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

	renderer->setPerObjectBuffer(perObject);

	mpMeshRenderer->Render(renderer);
}

}