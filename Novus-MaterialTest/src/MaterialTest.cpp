#include "MaterialTest.h"

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
#include <Resources/ResourceCache.h>
#include <Graphics/StaticMesh.h>
#include <Input/InputSystem.h>

using namespace novus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MaterialTest* application = NE_NEW MaterialTest(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

MaterialTest::MaterialTest(HINSTANCE instance)
:
NovusApplication(instance),
mpMainShader(NULL),
mpSkyboxRenderer(NULL),
mpModelMesh(NULL),
mModelRoughness(0.04f)
{
	mMainWndCaption = L"Novus Engine Material Test v0.1.66";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 4.9f, 1.4f));
	mpCamera->setSpeed(0.5f);

	mpCamera->setPosition(Vector3(0.0f, 2.0f, 3.0f));
	mpCamera->LookAt(Vector3(0.0f, 0.0f, 0.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

MaterialTest::~MaterialTest()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
}

bool MaterialTest::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	InitShaders();
	InitMesh();
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

	mpSkyboxRenderer = NE_NEW SkyboxRenderer();
	mpSkyboxRenderer->Init(L"../Textures/sunsetcube1024.dds");

	return true;
}

void MaterialTest::InitShaders()
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mpMainShader = mpRenderer->LoadShader(L"../Shaders/GenericShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription));
}

void MaterialTest::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateGeosphere(1.0f, 4, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreatePlane(1000.0f, 1000.0f, 4, 4, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	mpModelMesh = mpResourceCache->getResource<StaticMesh>(L"../Models/dragon.obj");

	StaticMeshMaterial modelMat;
	modelMat.RenderMaterial.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	modelMat.RenderMaterial.SpecularColor = Vector3(0.725f, 0.58f, 0.271f);
	modelMat.RenderMaterial.Emissive = Vector3(0.0f, 0.0f, 0.0f);
	modelMat.RenderMaterial.Metallic = 1.0f;
	modelMat.RenderMaterial.Roughness = mModelRoughness;

	mpModelMesh->setMaterial(modelMat);
}

void MaterialTest::InitLights()
{
	mLights.clear();
	mLights.reserve(128);
	for (int i = 0; i < 128; i++)
	{
		PointLight light;
		light.Color = Vector3(Math::RandF(), Math::RandF(), Math::RandF());
		//light.Color = Vector3(1.0f, 1.0f, 1.0f);
		light.Intensity = Math::RandF(0.3f, 2.7f) * 0.25f;
		light.Radius = 0.0f;

		light.FalloffPow = 1;
		light.PositionWorld = Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(0.0f, 0.5f), Math::RandF(-1.0f, 1.0f)) * 8.0f;
		light.PositionWorld.y = Math::RandF(0.2f, 4.0f);

		mLights.push_back(light);
	}
}

void MaterialTest::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &MaterialTest::OnKeyDown));
}

void MaterialTest::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &MaterialTest::OnKeyDown));
}

void MaterialTest::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KeyboardKey::KEY_ESC)
	{
		ExitApp();
	}
}

void MaterialTest::OnResize()
{
	NovusApplication::OnResize();

	mpCamera->OnResize(getClientWidth(), getClientHeight());
}

void MaterialTest::Update(float dt)
{
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));
	mpCamera->Update(dt);
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));

	mpRenderer->getDeferredRenderer()->Update(dt);

	auto endIt = mLights.end();
	for (auto it = mLights.begin(); it != endIt; ++it)
	{
		(*it).PositionWorld = Matrix3::RotateY(dt * 0.01f) * (*it).PositionWorld;
		mpRenderer->getDeferredRenderer()->getLightManager()->AddLightForFrame(*it);
	}

	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_Z))
	{
		mModelRoughness = Math::Clamp(mModelRoughness - dt * 0.3f, 0.01f, 1.0f);
	}
	if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_X))
	{
		mModelRoughness = Math::Clamp(mModelRoughness + dt * 0.3f, 0.01f, 1.0f);
	}

	StaticMeshMaterial modelMat;
	modelMat.RenderMaterial.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	modelMat.RenderMaterial.SpecularColor = Vector3(1.0f);
	modelMat.RenderMaterial.Emissive = Vector3(0.0f, 0.0f, 0.0f);
	modelMat.RenderMaterial.Metallic = 0.04f;
	modelMat.RenderMaterial.Roughness = mModelRoughness;

	mpModelMesh->setMaterial(modelMat);
}

void MaterialTest::Render()
{
	mpRenderer->PreRender();
	mpRenderer->setShader(mpMainShader);

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

	mpRenderer->setPerFrameBuffer(perFrame);
	mpRenderer->BindPerFrameBuffer();

	CBPerObject perObject;
	perObject.Material.HasDiffuseTexture = false;
	perObject.Material.HasNormalTexture = false;
	perObject.Material.HasSpecularTexture = false;
	perObject.Material.Emissive = Vector3(0.0f);

	for (int x = -5; x <= 5; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			//perObject.Material.Diffuse = Vector4(0.5f, 1.0f, 0.4f, 1.0f);
			perObject.Material.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			perObject.Material.SpecularColor = (z == 0) ? Vector3(0.5f) : Vector3(0.725f, 0.58f, 0.271f);
			perObject.Material.Roughness = Math::Clamp((x + 5) / 10.0f, 0.005f, 1.0f);
			perObject.Material.Metallic = static_cast<float>(Math::Min(z + 0.05f, 1.0f));

			perObject.World = Matrix4::Scale(0.1f, 0.1f, 0.1f) *
				Matrix4::Translate(static_cast<float>(x) / 2.0f, 0.1f, static_cast<float>(z) / 2.0f);

			perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
			perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

			mpRenderer->setPerObjectBuffer(perObject);

			mMeshRenderer.Render(mpRenderer);
		}
	}

	//Plane
	perObject.World = Matrix4::Translate(0.0f, 0.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(0.3f);
	perObject.Material.Metallic = 0.05f;
	perObject.Material.Roughness = 0.15f;
	mpRenderer->setPerObjectBuffer(perObject);
	mPlaneRenderer.Render(mpRenderer);

	//Buddha
	perObject.World = Matrix4::RotateY(-Math::Pi / 2.0f) * Matrix4::Scale(2.1f) * Matrix4::Translate(0.0f, 0.55f, -0.7f); 
	mpRenderer->PushTransform(perObject.World);
	mpModelMesh->Render(mpRenderer);
	mpRenderer->PopTransform();

	mpSkyboxRenderer->Render(mpRenderer);

	mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}