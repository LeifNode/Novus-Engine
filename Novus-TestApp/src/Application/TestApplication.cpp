#include "TestApplication.h"

#include <Application/EngineStatics.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>
#include <Resources/Font/Font.h>
#include <Resources/Font/FontManager.h>
#include <Math/Math.h>
#include <Graphics/Camera.h>
#include <Graphics/D3DRenderer.h>
#include <Graphics/PostProcess/DeferredRenderer.h>
#include <Graphics/Textures/Texture2D.h>
#include <Graphics/SkyboxRenderer.h>
#include <Resources/Mesh/OBJLoader.h>
#include <Graphics/StaticMesh.h>
#include <Resources/MeshResourceManager.h>
#include <Resources/ResourceCache.h>
#include <Graphics/Textures/Texture3D.h>

using namespace novus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	TestApplication* application = NE_NEW TestApplication(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

TestApplication::TestApplication(HINSTANCE instance)
	:
	NovusApplication(instance),
	mpMainShader(NULL),
	mpStaticMeshShader(NULL),
	mpVoxelizationShader(NULL),
	mpDebugRaymarchShader(NULL),
	mpSkyboxRenderer(NULL),
	mpMesh(NULL),
	mpVoxelTexture(NULL),
	mRenderVoxelization(false)
{
	mMainWndCaption = L"Novus Engine Test App v0.1.65";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 4.9f, 1.4f));
	mpCamera->setVelocity(3.0f);

	mpCamera->setPosition(Vector3(3.0f, 2.0f, 0.0f));
	mpCamera->LookAt(Vector3(0.0f, 0.0f, 0.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

TestApplication::~TestApplication()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
	NE_DELETE(mpVoxelTexture);
}

bool TestApplication::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	InitShaders();
	InitMesh();

	novus::Font* verdana = mpFontManager->LoadFont(
		"verdana",
		"../Fonts/verdana.ttf",
		"../Fonts/verdanab.ttf",
		"../Fonts/verdanai.ttf",
		"../Fonts/verdanaz.ttf");

	verdana->LoadGlyphs(24, novus::FontType::Normal);
	verdana->LoadGlyphs(24, novus::FontType::Bold);
	verdana->LoadGlyphs(24, novus::FontType::Italic);
	verdana->LoadGlyphs(24, novus::FontType::BoldItalic);

	mpSkyboxRenderer = NE_NEW SkyboxRenderer();
	mpSkyboxRenderer->Init(L"../Textures/sunsetcube1024.dds");

	mpVoxelTexture = NE_NEW Texture3D();
	mpVoxelTexture->Init(mpRenderer, 512, 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM, Math::MipMapCount(256, 256), D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS);
	mpVoxelTexture->setDebugName("Voxel Volume");

	mpMesh = mpResourceCache->getResource<StaticMesh>(L"../Models/sponza.obj");

	if (mpMesh)
		std::cout << "Loaded mesh.\n";
	else
		std::cout << "Failed to load mesh.\n";

	return true;
}

void TestApplication::InitShaders()
{
	ShaderInfo shaderInfo[] = {
		{ ShaderType::Vertex, "VS" },
		{ ShaderType::Pixel, "PS" },
		{ ShaderType::None, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mpMainShader = mpRenderer->LoadShader(L"../Shaders/GenericShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription));

	D3D11_INPUT_ELEMENT_DESC staticMeshVertexDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mpStaticMeshShader = mpRenderer->LoadShader(L"../Shaders/StaticMeshShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, staticMeshVertexDescription, ARRAYSIZE(staticMeshVertexDescription));

	ShaderInfo voxelShaderInfo[] = {
		{ ShaderType::Vertex, "VS" },
		{ ShaderType::Geometry, "GS" },
		{ ShaderType::Pixel, "PS" },
		{ ShaderType::None, NULL }
	};

	mpVoxelizationShader = mpRenderer->LoadShader(L"../Shaders/VXGI/VoxelizeMesh.hlsl", voxelShaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, staticMeshVertexDescription, ARRAYSIZE(staticMeshVertexDescription));
	
	ShaderInfo raymarchShaderInfo[] = {
		{ ShaderType::Compute, "RaymarchCS" },
		{ ShaderType::None, NULL }
	};

	mpDebugRaymarchShader = mpRenderer->LoadShader(L"../Shaders/VXGI/DebugRaymarchVolume.hlsl", raymarchShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
}

void TestApplication::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateGeosphere(1.0f, 4, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreatePlane(1000.0f, 1000.0f, 4, 4, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
}

void TestApplication::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &TestApplication::OnKeyDown));
}

void TestApplication::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &TestApplication::OnKeyDown));
}

void TestApplication::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KeyboardKey::KEY_ESC)
	{
		ExitApp();
	}
	if (dataPtr->getKey() == KeyboardKey::KEY_F)
		mRenderVoxelization = !mRenderVoxelization;
}

void TestApplication::OnResize()
{
	NovusApplication::OnResize();

	mpCamera->OnResize(getClientWidth(), getClientHeight());
}

void TestApplication::Update(float dt)
{
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));
	mpCamera->Update(dt);
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));

	mCurrentRotation = Quaternion::AxisAngle(Normalize(Vector3(1.0f, 1.0f, 1.0f)), dt) * mCurrentRotation;
	mCurrentRotation = Quaternion::Normalize(mCurrentRotation);

	mpRenderer->getDeferredRenderer()->Update(dt);
}

void TestApplication::Render()
{
	mpRenderer->PreRender();
	mpRenderer->setShader(mpMainShader);

	CBPerFrame perFrame;

	Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	mpRenderer->context()->ClearRenderTargetView(mpVoxelTexture->getRenderTargetView(), &clearColor.x);

	//Render voxelization
	Matrix4 voxelProjection = Matrix4::OrthographicOffCenter(15.0f, -15.0f, -15.0f, 15.0f, -15.0f, 15.0f);

	perFrame.ScreenResolution = Vector2_t<unsigned int>(
		static_cast<unsigned int>(getClientWidth()),
		static_cast<unsigned int>(getClientHeight()));
	perFrame.ClipNearFar = Vector2(mpCamera->getNear(), mpCamera->getFar());
	perFrame.Projection = voxelProjection;
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	//perFrame.View = Matrix4::LookToward(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	perFrame.View = Matrix4(1.0f);
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);
	perFrame.EyePosition = mpCamera->getPosition();

	mpRenderer->setShader(mpVoxelizationShader);
	mpRenderer->setPerFrameBuffer(perFrame);

	ID3D11UnorderedAccessView* voxelUAV = mpVoxelTexture->getUnorderedAccessView();
	mpRenderer->context()->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 1, &voxelUAV, 0);
	mpRenderer->setViewport(0, 0, 512, 512);
	mpRenderer->PushTransform(Matrix4::Scale(0.01f));
	mpMesh->Render(mpRenderer);
	mpRenderer->PopTransform();

	//Render meshes as normal
	perFrame.ScreenResolution = Vector2_t<unsigned int>(
		static_cast<unsigned int>(getClientWidth()), 
		static_cast<unsigned int>(getClientHeight()));
	perFrame.ClipNearFar = Vector2(mpCamera->getNear(), mpCamera->getFar());
	perFrame.Projection = mpCamera->getProj();
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	perFrame.View = mpCamera->getView();
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);
	perFrame.EyePosition = mpCamera->getPosition();

	mpRenderer->setViewport(0, 0, getClientWidth(), getClientHeight());
	mpRenderer->setPerFrameBuffer(perFrame);
	mpRenderer->BindPerFrameBuffer();
	mpRenderer->ClearDepth();
	mpRenderer->getGBuffer()->BindRenderTargets();
	mpRenderer->setShader(mpStaticMeshShader);
	mpRenderer->PushTransform(Matrix4::Scale(0.01f));
	mpMesh->Render(mpRenderer);
	mpRenderer->PopTransform();

	mpRenderer->UnbindTextureResources();

	CBPerObject perObject;
	perObject.Material.HasDiffuseTexture = false;
	perObject.Material.HasNormalTexture = false;
	perObject.Material.HasSpecularTexture = false;
	perObject.Material.Emissive = Vector3(0.0f);

	/*for (int x = -5; x <= 5; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			perObject.Material.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			perObject.Material.Roughness = Math::Clamp((x + 5) / 10.0f, 0.005f, 1.0f);
			perObject.Material.Metallic = static_cast<float>(z);

			perObject.World = Matrix4::Scale(0.1f, 0.1f, 0.1f) *
				Matrix4::Translate(static_cast<float>(x) / 2.0f, 2.0f, static_cast<float>(z) / 2.0f);

			perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
			perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

			mpRenderer->setPerObjectBuffer(perObject);

			mMeshRenderer.Render(mpRenderer);
		}
	}*/

	/*perObject.World = Matrix4::Translate(0.0f, -5.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(1.0f, 1.0f, 1.0f);
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Roughness = 0.15f;
	mpRenderer->setPerObjectBuffer(perObject);
	mPlaneRenderer.Render(mpRenderer);*/


	mpSkyboxRenderer->Render(mpRenderer);

	if (mRenderVoxelization)
	{
		mpRenderer->ResetRenderTarget();
		mpRenderer->UnbindTextureResources();
		mpRenderer->UnbindUAVs();

		mpRenderer->setShader(mpDebugRaymarchShader);
		mpRenderer->BindPerFrameBuffer();
		mpRenderer->ResetSamplerState();

		ID3D11ShaderResourceView* voxelSRV = mpVoxelTexture->getResourceView();
		mpRenderer->context()->CSSetShaderResources(0, 1, &voxelSRV);

		ID3D11UnorderedAccessView* outputUAV = mpRenderer->getDeferredRenderer()->getHDRRenderTarget()->getUnorderedAccessView();
		mpRenderer->context()->CSSetUnorderedAccessViews(0, 1, &outputUAV, 0);

		//Compute shader tile group dimensions = 16x16
		unsigned int dispatchWidth = (getClientWidth() + 16 - 1) / 16;
		unsigned int dispatchHeight = (getClientHeight() + 16 - 1) / 16;

		mpRenderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

		mpRenderer->UnbindTextureResources();
		mpRenderer->UnbindUAVs();
	}
	else
		mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}