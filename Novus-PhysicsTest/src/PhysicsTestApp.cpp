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
#include <Physics/Particle.h>
#include <Physics/ParticleForceGenerator.h>
#include <Physics/PhysicsSystem.h>

#include "PlanetParticle.h"
#include "PlanetaryGravitationGenerator.h"
#include "PlanetParser.h"


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
	mpPhysicsSystem = NE_NEW PhysicsSystem();
}

PhysicsTestApplication::~PhysicsTestApplication()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpPhysicsSystem);
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

	mpPhysicsSystem->Init();

	InitSolarSystem();

	return true;
}

void PhysicsTestApplication::InitSolarSystem()
{
	PlanetaryGravitationGenerator* gravityGen = new PlanetaryGravitationGenerator(mpPhysicsSystem);
	mpPhysicsSystem->AddForceGenerator(gravityGen);

	PlanetParser parser;
	parser.Parse("./data/SolSystem.xml");

	for (auto it = parser.getPlanets().cbegin(); it != parser.getPlanets().cend(); ++it)
	{
		PlanetParticle* newPlanet = new PlanetParticle();

		newPlanet->setName(it->Name);
		newPlanet->setMass(static_cast<float>(it->Mass));
		newPlanet->setRadius(static_cast<float>(it->Radius));
		newPlanet->setPosition(Vector3d(it->Perihelion, 0.0, 0.0));
		newPlanet->setVelocity(Vector3d(0.0, 0.0, -it->PerihelionVelocity));
		
		mpPhysicsSystem->AddParticle(newPlanet);
		mpPhysicsSystem->AddRegistryEntry(newPlanet, gravityGen);
		mPlanets.push_back(newPlanet);
	}
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

	GeometryGenerator::CreateGrid(1.0f, 1.0f, 5, 5, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
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
	for (int i = 0; i < 200; i++)
		mpPhysicsSystem->Update(dt * 30758400.0f * 0.00001f);

	for (auto it = mPlanets.cbegin(); it != mPlanets.cend(); ++it)
	{
		if ((*it)->getName() == "Earth" || (*it)->getName() == "Earth's Moon")
			std::cout << "Planet: " << (*it)->getName() << "\nLocation: " << (*it)->getPosition().x << ", " << (*it)->getPosition().y << ", " << (*it)->getPosition().z << "\n";
	}

	mpCamera->Update(dt);

	mpRenderer->getDeferredRenderer()->Update(dt);
}

void PhysicsTestApplication::Render()
{
	mpRenderer->PreRender();
	mpRenderer->setShader(mpMainShader);

	CBPerFrame perFrame;
	perFrame.ScreenResolution = Vector2u(
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

	//Render plane
	perObject.World = Matrix4::Scale(100.0f, 1.0f, 100.0f) * Matrix4::Translate(0.0f, -5.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;

	perObject.Material.Diffuse = Vector4(1.0f);
	perObject.Material.SpecularColor = Vector3(1.0f);
	perObject.Material.Roughness = 0.4f;
	perObject.Material.Emissive = Vector3(0.0f);

	mpRenderer->setPerObjectBuffer(perObject);

	//mPlaneRenderer.Render(mpRenderer);

	//Render planets

	for (auto it = mPlanets.cbegin(); it != mPlanets.cend(); ++it)
	{
		perObject.World = Matrix4::Scale(static_cast<float>((*it)->getRadius()) / 1.0e6f) * Matrix4::Translate(static_cast<Vector3>((*it)->getPosition() / 1.0e7));
		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
		perObject.Material.Roughness = 0.5f;

		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(mpRenderer);
	}


	mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}