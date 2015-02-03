#include "PhysicsTestApp.h"

#include <Application/EngineStatics.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>
#include <Resources/Font/Font.h>
#include <Resources/Font/FontManager.h>
#include <Math/Math.h>
#include <Graphics/Camera.h>
#include <Graphics/D3DRenderer.h>
#include <Graphics/PostProcess/DeferredRenderer.h>

using namespace novus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	PhysicsTestApplication* application = NE_NEW PhysicsTestApplication(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

PhysicsTestApplication::PhysicsTestApplication(HINSTANCE instance)
:
NovusApplication(instance),
mpMainShader(NULL),
mpTiledDeferredShader(NULL)
{
	mMainWndCaption = L"Physics Test v0.0.1";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 1.0f, 1.0f));
}

PhysicsTestApplication::~PhysicsTestApplication()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
}

bool PhysicsTestApplication::Init()
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

void PhysicsTestApplication::InitShader()
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

void PhysicsTestApplication::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateGeosphere(1.0f, 2, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
}

void PhysicsTestApplication::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &PhysicsTestApplication::OnKeyDown));
}

void PhysicsTestApplication::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &PhysicsTestApplication::OnKeyDown));
}

void PhysicsTestApplication::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KeyboardKey::KEY_ESC)
	{
		ExitApp();
	}
}

void PhysicsTestApplication::OnResize()
{
	NovusApplication::OnResize();

	mpCamera->OnResize(getClientWidth(), getClientHeight());
}

void PhysicsTestApplication::Update(float dt)
{
	mpCamera->Update(dt);

	mCurrentRotation = Quaternion::AxisAngle(Normalize(Vector3(1.0f, 1.0f, 1.0f)), dt) * mCurrentRotation;
	mCurrentRotation = Quaternion::Normalize(mCurrentRotation);

	mpRenderer->getDeferredRenderer()->Update(dt);
}

void PhysicsTestApplication::Render()
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

	mpRenderer->setPerFrameBuffer(perFrame);

	CBPerObject perObject;

	/*for (int x = -10; x < 10; x++)
	{
		for (int y = -10; y < 10; y++)
		{
			for (int z = -10; z < 10; z++)
			{
				perObject.World = Quaternion::ToMatrix(mCurrentRotation) *
					Matrix4::Scale(0.1f, 0.1f, 0.1f) *
					Matrix4::Translate(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

				perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
				perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

				mpRenderer->setPerObjectBuffer(perObject);

				mMeshRenderer.Render(mpRenderer);
			}
		}
	}
*/

	perObject.World = Matrix4(1.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

	mpRenderer->setPerObjectBuffer(perObject);

	mMeshRenderer.Render(mpRenderer);
	mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}