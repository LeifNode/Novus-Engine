#include "TestApplication.h"

#include <Application/EngineStatics.h>
#include <Graphics/D3DRenderer.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>
#include <Resources/Font/Font.h>
#include <Resources/Font/FontManager.h>
#include <Math/Math.h>

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
	mTestFlt1(0.0f),
	mTestFlt2(0.0f),
	mTestDbl1(0.0),
	mTestDbl2(0.0)
{
	mMainWndCaption = L"Novus Engine Test App v0.0.1";

	mTestVec = Vector3(1.0f);
}

TestApplication::~TestApplication()
{
	UnhookInputEvents();
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
}

void TestApplication::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, mesh);

	mMeshRenderer.Initialize(mesh.Vertices, mesh.Indices, mpRenderer);
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
}

void TestApplication::Update(float dt)
{
	//GameTimer timer1;

	//XMMATRIX test1 = XMMatrixIdentity();

	//timer1.Start();

	//size_t iterCount = 10000000;

	//for (int i = 0; i < iterCount; i++)
	//{
	//	test1 = XMMatrixLookAtRH(XMVectorZero(), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f * dt, 0.0f, 0.0f)) * test1;

	//	//test1 = test1 * test1;
	//}

	//timer1.Tick();

	//std::cout << "Time 1: " << timer1.DeltaTime() << std::endl;


	//GameTimer timer2;

	//Matrix4 test2 = Matrix4(1.0f);

	//timer2.Start();

	//for (int i = 0; i < iterCount; i++)
	//{
	//	test2 = LookAt(Vector3(0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f * dt, 0.0f)) * test2;
	//}

	//timer2.Tick();

	//std::cout << "Time 2: " << timer2.DeltaTime() << std::endl;
}

void TestApplication::Render()
{
	mpRenderer->PreRender();
	mpRenderer->setShader(mpMainShader);

	CBPerFrame perFrame;
	perFrame.Projection = Perspective(45.0f, getClientWidth() / (float)getClientHeight(), 0.01f, 10000.0f);
	perFrame.ProjectionInv = Invert(perFrame.Projection);
	perFrame.View = LookToward(Vector3(0.0f, 1.0f, 1.0f), Normalize(Vector3(0.0f, 0.0f, 1.0f)), Vector3(0.0f, 1.0f, 0.0f));
	perFrame.ViewInv = Invert(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Invert(perFrame.ViewProj);

	Matrix4 rotate = RotateX(10.0f);

	mpRenderer->setPerFrameBuffer(perFrame);

	CBPerObject perObject;

	perObject.World = Matrix4(1.0f);
	perObject.WorldInvTranspose = Matrix4(1.0f);
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

	mpRenderer->setPerObjectBuffer(perObject);

	mMeshRenderer.Render(mpRenderer);

	mpRenderer->PostRender();
}