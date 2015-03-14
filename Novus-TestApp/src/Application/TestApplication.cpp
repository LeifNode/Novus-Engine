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
	mpSkyboxRenderer(NULL),
	mpMesh(NULL)
{
	mMainWndCaption = L"Novus Engine Test App v0.1.65";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, -4.9f, 1.4f));
	mpCamera->setVelocity(3.0f);

	mpCamera->setPosition(Vector3(-3.6f, -4.0f, 3.6f));
	mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

TestApplication::~TestApplication()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
	NE_DELETE(mpMesh);
}

bool TestApplication::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	InitShader();
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

	MeshResourceManager* loader = NE_NEW MeshResourceManager();
	loader->Init();

	mpMesh = static_cast<StaticMesh*>(loader->Load(L"../Models/sibenik.obj"));

	if (mpMesh)
		std::cout << "Loaded mesh.\n";
	else
		std::cout << "Failed to load mesh.\n";

	NE_DELETE(loader);

	return true;
}

void TestApplication::InitShader()
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

	CBPerObject perObject;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.27f);
	perObject.Material.Roughness = 0.0f;
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Emissive = Vector3(0.0f);

	/*for (int x = -5; x <= 5; x++)
	{
		perObject.Material.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		perObject.Material.Roughness = Math::Clamp((x + 5) / 10.0f, 0.005f, 1.0f);
		perObject.Material.Metallic = 0.0f;

		perObject.World = Matrix4::Scale(0.1f, 0.1f, 0.1f) * 
						  Matrix4::Translate(static_cast<float>(x) / 4.5f, -4.9f, 0.0f);

		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(mpRenderer);
	}*/

	for (int x = -5; x <= 5; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			perObject.Material.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			perObject.Material.Roughness = Math::Clamp((x + 5) / 10.0f, 0.005f, 1.0f);
			perObject.Material.Metallic = static_cast<float>(z);

			perObject.World = Matrix4::Scale(0.1f, 0.1f, 0.1f) *
				Matrix4::Translate(static_cast<float>(x) / 2.0f, -4.9f, static_cast<float>(z) / 2.0f);

			perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
			perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

			mpRenderer->setPerObjectBuffer(perObject);

			mMeshRenderer.Render(mpRenderer);
		}
	}

	perObject.World = Matrix4::Translate(0.0f, -5.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(1.0f, 1.0f, 1.0f);
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Roughness = 0.15f;
	mpRenderer->setPerObjectBuffer(perObject);
	mPlaneRenderer.Render(mpRenderer);


	mpRenderer->setShader(mpStaticMeshShader);
	mpRenderer->BindPerFrameBuffer();
	perObject.World = Matrix4::Scale(1.0f) * Matrix4::RotateY(Math::Pi) * Matrix4::Translate(0.0f, -4.65f, 1.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.27f);
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Roughness = 0.3f;
	mpRenderer->setPerObjectBuffer(perObject);
	mpMesh->Render(mpRenderer);

	mpSkyboxRenderer->Render(mpRenderer);

	mpRenderer->RenderDeferredShading();
	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}