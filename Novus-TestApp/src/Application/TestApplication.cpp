#include "TestApplication.h"

#include <Application/EngineStatics.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>
#include <Input/InputSystem.h>
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
#include <Core/World.h>
#include <Core/Actor.h>
#include <Core/Components/StaticMeshComponent.h>
#include <Graphics/RenderTargets/ShadowMapRenderTarget.h>
#include <Graphics/RenderTargets/VoxelVolumeRenderTarget.h>
#include <Graphics/PostProcess/GlobalIlluminationPass.h>
#include <Graphics/VXGI/VoxelRadianceVolume.h>

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
	mpStaticMeshShader(NULL),
	mpDepthPassShader(NULL),
	mpVoxelizationShader(NULL),
	mpDebugRaymarchShader(NULL),
	mpSkyboxRenderer(NULL),
	mpMesh(NULL),
	mpVoxelTexture(NULL),
	mpShadowMap(NULL),
	mpVoxelVolume(NULL),
	mRenderVoxelization(false),
	mVoxelResolution(256),
	mpGlobalIlluminationRenderPass(NULL),
	mpRadianceVolume(NULL),
	mSceneVoxelized(false),
	mShadowOffsetX(0.0f),
	mShadowOffsetY(0.0f),
	mRequiresReinject(true)
{
	mMainWndCaption = L"Novus Engine Test App v0.1.65";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 4.9f, 1.4f));
	mpCamera->setVelocity(3.0f);

	mpCamera->setPosition(Vector3(3.0f, 2.0f, 0.0f));
	mpCamera->LookAt(Vector3(0.0f, 2.0f, -2.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

TestApplication::~TestApplication()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
	NE_DELETE(mpVoxelTexture);
	NE_DELETE(mpVoxelVolume);
	NE_DELETE(mpShadowMap);
	NE_DELETE(mpGlobalIlluminationRenderPass);
	NE_DELETE(mpRadianceVolume);
}

bool TestApplication::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	InitShaders();
	InitLights();

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

	mpShadowMap = NE_NEW ShadowMapRenderTarget();
	mpShadowMap->Init(2048, 2048);
	mpShadowMap->setDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));
	mpShadowMap->setPosition(Vector3(0.0f, 20.0f, 0.0f));
	mpShadowMap->setVolumeOrthographicBounds(40.0f, 40.0f, 60.0f);

	mpWorld->RegisterRenderTarget(mpShadowMap);

	mpSkyboxRenderer = NE_NEW SkyboxRenderer();
	mpSkyboxRenderer->Init(L"../Textures/sunsetcube1024.dds");

	mpVoxelVolume = NE_NEW VoxelVolumeRenderTarget();
	mpVoxelVolume->Init(mVoxelResolution);
	mpVoxelVolume->setBounds(30.0f, 30.0f, 30.0f);

	mpWorld->RegisterRenderTarget(mpVoxelVolume);

	mpRadianceVolume = NE_NEW VoxelRadianceVolume();
	mpRadianceVolume->Init(mpVoxelVolume, mpShadowMap);

	mpGlobalIlluminationRenderPass = NE_NEW GlobalIlluminationPass();
	mpGlobalIlluminationRenderPass->Init(getClientWidth(), getClientHeight());
	mpGlobalIlluminationRenderPass->setGBuffer(mpRenderer->getGBuffer());
	mpGlobalIlluminationRenderPass->setShadowMap(mpShadowMap);
	mpGlobalIlluminationRenderPass->setVoxelVolume(mpRadianceVolume);
	mpGlobalIlluminationRenderPass->setLightDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));

	mpMesh = mpResourceCache->getResource<StaticMesh>(L"../Models/sponza.obj");

	if (mpMesh)
		std::cout << "Loaded mesh.\n";
	else
		std::cout << "Failed to load mesh.\n";

	Actor* meshActor = NE_NEW Actor();
	StaticMeshComponent* meshComponent = NE_NEW StaticMeshComponent(mpMesh);
	meshComponent->setDisplayName("Mesh1");

	meshActor->AddComponent(meshComponent);
	meshActor->transform.SetScale(0.01f);

	mpWorld->AddActor(meshActor);

	return true;
}

void TestApplication::InitShaders()
{
	ShaderInfo shaderInfo[] = {
		{ ShaderType::Vertex, "VS" },
		{ ShaderType::Pixel, "PS" },
		{ ShaderType::None, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC staticMeshVertexDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mpStaticMeshShader = mpRenderer->LoadShader(L"../Shaders/StaticMeshShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, staticMeshVertexDescription, ARRAYSIZE(staticMeshVertexDescription));

	mpDepthPassShader = mpRenderer->LoadShader(L"../Shaders/DepthPassShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, staticMeshVertexDescription, ARRAYSIZE(staticMeshVertexDescription));

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

void TestApplication::InitLights()
{
	mLights.clear();
	mLights.reserve(256);
	for (int i = 0; i < 256; i++)
	{
		PointLight light;
		light.Color = Vector3(Math::RandF(), Math::RandF(), Math::RandF());
		//light.Color = Vector3(1.0f, 1.0f, 1.0f);
		light.Intensity = Math::RandF(0.3f, 2.7f) * 0.65f;
		light.Radius = 0.0f;

		light.FalloffPow = 1;
		light.PositionWorld = Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f)) * 20.0f;
		light.PositionWorld.y = Math::RandF(0.8f, 9.5f);

		mLights.push_back(light);
	}
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

	if (mpGlobalIlluminationRenderPass != NULL)
		mpGlobalIlluminationRenderPass->Init(getClientWidth(), getClientHeight());
}

void TestApplication::Update(float dt)
{
	mpCamera->Update(dt);

	mpRenderer->getDeferredRenderer()->Update(dt);

	auto endIt = mLights.end();
	for (auto it = mLights.begin(); it != endIt; ++it)
	{
		(*it).PositionWorld = Matrix3::RotateY(dt * 0.02f) * (*it).PositionWorld;
		mpRenderer->getDeferredRenderer()->getLightManager()->AddLightForFrame(*it);
	}

	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_Z))
	{
		mShadowOffsetX = Math::Clamp(mShadowOffsetX + 0.1f * dt, -1.0f, 1.0f);
		mpGlobalIlluminationRenderPass->setLightDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));
		mRequiresReinject = true;
	}
	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_X))
	{
		mShadowOffsetX = Math::Clamp(mShadowOffsetX - 0.1f * dt, -1.0f, 1.0f);
		mpGlobalIlluminationRenderPass->setLightDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));
		mRequiresReinject = true;
	}
	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_C))
	{
		mShadowOffsetY = Math::Clamp(mShadowOffsetY + 0.1f * dt, -1.0f, 1.0f);
		mpGlobalIlluminationRenderPass->setLightDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));
		mRequiresReinject = true;
	}
	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_V))
	{
		mShadowOffsetY = Math::Clamp(mShadowOffsetY - 0.1f * dt, -1.0f, 1.0f);
		mpGlobalIlluminationRenderPass->setLightDirection(Normalize(Vector3(mShadowOffsetY, -1.0f, mShadowOffsetX)));
		mRequiresReinject = true;
	}


	mpShadowMap->setDirection(mpGlobalIlluminationRenderPass->getLightDirection());

	EngineStatics::getWorld()->Update(dt);
}

void TestApplication::Render()
{
	mpRenderer->PreRender();


	if (!mSceneVoxelized)
	{
		mpRenderer->setShader(mpDepthPassShader);
		mpWorld->RenderScenePass(mpRenderer, RenderPass::Shadow);

		//Render voxelization
		mpRenderer->setShader(mpVoxelizationShader);
		mpWorld->RenderScenePass(mpRenderer, RenderPass::GraphicsPrepass);
	}
	else if (mRequiresReinject)
	{
		mpRenderer->setShader(mpDepthPassShader);
		mpWorld->RenderScenePass(mpRenderer, RenderPass::Shadow);
	}

	//Render meshes as normal
	CBPerFrame perFrame;

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

	mpSkyboxRenderer->Render(mpRenderer);

	mpGlobalIlluminationRenderPass->Execute(mpRenderer);

	if (mRequiresReinject)
	{
		mpRadianceVolume->InjectRadiance(mpRenderer);
		mRequiresReinject = false;
	}
	

	if (mRenderVoxelization)
	{
		mpRenderer->ResetRenderTarget();
		mpRenderer->UnbindTextureResources();
		mpRenderer->UnbindUAVs();

		mpRenderer->setShader(mpDebugRaymarchShader);
		mpRenderer->BindPerFrameBuffer();
		mpRenderer->ResetSamplerState();

		CBPerObject perObject;
		ZeroMemory(&perObject, sizeof(perObject));
		perObject.WorldViewProj = mpVoxelVolume->getWorldToVolume();

		mpRenderer->setPerObjectBuffer(perObject);

		ID3D11ShaderResourceView* voxelSRV = mpVoxelVolume->getTexture()->getResourceView();
		//ID3D11ShaderResourceView* voxelSRV = mpRadianceVolume->getRadianceVolume()->getResourceView();
		mpRenderer->context()->CSSetShaderResources(0, 1, &voxelSRV);

		ID3D11UnorderedAccessView* outputUAV = mpRenderer->getDeferredRenderer()->getHDRRenderTarget()->getUnorderedAccessView();
		mpRenderer->context()->CSSetUnorderedAccessViews(0, 1, &outputUAV, 0);

		//Compute shader tile group dimensions = 16x16
		unsigned int dispatchWidth = (getClientWidth() + 16 - 1) / 16;
		unsigned int dispatchHeight = (getClientHeight() + 16 - 1) / 16;

		mpRenderer->context()->Dispatch(dispatchWidth, dispatchHeight, 1);

		mpRenderer->UnbindTextureResources();
		mpRenderer->UnbindUAVs();

		mpRenderer->getDeferredRenderer()->setSourceRenderTarget(mpGlobalIlluminationRenderPass->getRenderTarget());
	}
	else
	{
		mpRenderer->RenderDeferredShading();

		mpRenderer->getDeferredRenderer()->setSourceRenderTarget(mpRenderer->getDeferredRenderer()->getHDRRenderTarget());
	}

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();

	mSceneVoxelized = true;
}