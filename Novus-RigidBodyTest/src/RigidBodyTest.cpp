#include "RigidBodyTest.h"

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
#include <Graphics/LineRenderer.h>
#include <Graphics/SkyboxRenderer.h>
#include <Physics/Particle.h>
#include <Physics/PhysicsSystem.h>

using namespace novus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	RigidBodyTest* application = NE_NEW RigidBodyTest(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

RigidBodyTest::RigidBodyTest(HINSTANCE instance)
:
NovusApplication(instance),
mpMainShader(NULL),
mpSkyboxRenderer(NULL),
mpContactRenderer(NULL),
mContactCount(0),
mContactResolver(msMaxContacts * 8)
{
	mMainWndCaption = L"Novus Engine Rigid Body Test v0.1.85";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 4.9f, 1.4f));
	mpCamera->setSpeed(15.0f);

	mpCamera->setPosition(Vector3(3.0f, 2.0f, 3.0f) * 10.0f);
	mpCamera->LookAt(Vector3(0.0f, 0.0f, 0.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

RigidBodyTest::~RigidBodyTest()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
	NE_DELETE(mpContactRenderer);

	for (auto it = mCollisionHalfSpaces.begin(); it != mCollisionHalfSpaces.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mCollisionBoxes.begin(); it != mCollisionBoxes.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mPhysicsBodies.begin(); it != mPhysicsBodies.end(); ++it)
	{
		NE_DELETE(*it);
	}
}

bool RigidBodyTest::Init()
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

	InitPhysicsActors();

	mpContactRenderer = NE_NEW LineRenderer();
	mpContactRenderer->Init();
	mpContactRenderer->setLineWidth(0.05f);
	mpContactRenderer->setLineColor(Vector4(1.0f, 0.5f, 0.0f, 1.0f));

	return true;
}

void RigidBodyTest::InitShaders()
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

void RigidBodyTest::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreatePlane(1000.0f, 1000.0f, 4, 4, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
}

void RigidBodyTest::InitPhysicsActors()
{
	for (int i = 0; i < 100; i++)
	{
		RigidBody* body = NE_NEW RigidBody();
		//Set random position and rotation
		body->getTransform()->SetPosition(Vector3(Math::RandF(-10.0f, 10.0f), 10.0f, Math::RandF(-10.0f, 10.0f)));
		body->getTransform()->SetRotation(Quaternion::AxisAngle(Normalize(Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f))), Math::RandF(0.0f, 2.0f * Math::Pi)));
		body->setMass(10.0f);

		Vector3 halfScale = Vector3(0.5f);

		Matrix3 tensor = Matrix3::BoxIntertiaTensor(halfScale, body->getMass());
		body->setInertiaTensor(tensor);

		body->ClearAccumilators();

		body->CalculateDerivedData();

		mPhysicsBodies.push_back(body);

		CollisionBox* boxCollider = NE_NEW CollisionBox();
		boxCollider->body = body;
		boxCollider->halfSize = halfScale;
		boxCollider->CalculateInternals();

		mCollisionBoxes.push_back(boxCollider);
	}

	InitPhysicsBounds();
}

void RigidBodyTest::InitPhysicsBounds()
{
	float bounds = 20.0f;

	//Ground
	CollisionPlane* plane = NE_NEW CollisionPlane();
	plane->direction = Vector3(0.0f, 1.0f, 0.0f);
	plane->origin = Vector3();

	mCollisionHalfSpaces.push_back(plane);
}

void RigidBodyTest::ResetPhysicsSimulation()
{
	for (auto it = mCollisionHalfSpaces.begin(); it != mCollisionHalfSpaces.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mCollisionBoxes.begin(); it != mCollisionBoxes.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mPhysicsBodies.begin(); it != mPhysicsBodies.end(); ++it)
	{
		NE_DELETE(*it);
	}

	mCollisionHalfSpaces.clear();
	mCollisionBoxes.clear();
	mPhysicsBodies.clear();

	InitPhysicsActors();
	InitPhysicsBounds();
}

void RigidBodyTest::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &RigidBodyTest::OnKeyDown));
}

void RigidBodyTest::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &RigidBodyTest::OnKeyDown));
}

void RigidBodyTest::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	float scale;

	switch (dataPtr->getKey())
	{
	case KeyboardKey::KEY_ESC:
		ExitApp();
		break;
	case KeyboardKey::KEY_R:
		ResetPhysicsSimulation();
		break;
	}
}

void RigidBodyTest::OnResize()
{
	NovusApplication::OnResize();

	mpCamera->OnResize(getClientWidth(), getClientHeight());
}

void RigidBodyTest::Update(float dt)
{
	for (auto it = mPhysicsBodies.cbegin(); it != mPhysicsBodies.cend(); ++it)
	{
		(*it)->ClearAccumilators();
		(*it)->CalculateDerivedData();

		(*it)->AddForce(Vector3(0.0f, -9.8f, 0.0f) * (*it)->getMass());//Gravity
	}

	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));
	mpCamera->Update(dt);
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));

	mpRenderer->getDeferredRenderer()->Update(dt);

	UpdatePhysics(dt);

	UpdateContactRenderers();
	UpdatePointLights(dt);
}

void RigidBodyTest::UpdatePhysics(float dt)
{
	UpdateBodies(dt);

	GenerateContacts();

	mContactResolver.setIterations(mCollisionData.contactCount * 4);
	mContactResolver.ResolveContacts(mCollisionData.contactArray, mCollisionData.contactCount, dt);
}

void RigidBodyTest::UpdateContactRenderers()
{
	mpContactRenderer->points.Clear();

	for (unsigned int i = 0; i < mCollisionData.contactCount; i++)
	{
		mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, mContacts[i].contactPoint + mContacts[i].contactNormal));
	}

	mpContactRenderer->ReloadPoints();
}

void RigidBodyTest::UpdatePointLights(float dt)
{
	/*for (auto it = mPointLightParticles.cbegin(); it != mPointLightParticles.cend(); ++it)
	{
		PointLight light;
		light.Color = Vector3(1.0f);
		light.Intensity = 0.4f;
		light.PositionWorld = (*it)->getPosition();
		light.Radius = 0.0f;

		mpRenderer->getDeferredRenderer()->getLightManager()->AddLightForFrame(light);
	}*/
}

void RigidBodyTest::UpdateBodies(float dt)
{
	for (auto it = mCollisionBoxes.cbegin(); it != mCollisionBoxes.cend(); ++it)
	{
		(*it)->body->Integrate(dt);
		(*it)->CalculateInternals();
	}
}

void RigidBodyTest::GenerateContacts()
{
	unsigned int limit = msMaxContacts;
	mCollisionData.contactArray = mContacts;
	mCollisionData.Reset(msMaxContacts);
	mCollisionData.friction = 0.9f;
	mCollisionData.restitution = 0.6f;
	mCollisionData.tolerance = 0.1f;

	for (auto it = mCollisionBoxes.cbegin(); it != mCollisionBoxes.cend(); ++it)
	{
		for (auto plane = mCollisionHalfSpaces.cbegin(); plane != mCollisionHalfSpaces.cend(); ++plane)
		{
			if (!mCollisionData.hasMoreContacts()) return;

			CollisionDetector::BoxAndHalfSpace(*(*it), *(*plane), &mCollisionData);
		}
	}
}

void RigidBodyTest::Render()
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

	for (auto it = mCollisionBoxes.cbegin(); it != mCollisionBoxes.cend(); ++it)
	{
		perObject.World = (*it)->body->getTransform()->GetTransform();
		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
		perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		perObject.Material.Emissive = Vector3(0.5f);
		perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.271f);
		perObject.Material.Metallic = 0.0f;
		perObject.Material.Roughness = 0.12f;
		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(mpRenderer);
	}

	perObject.World = Matrix4::Translate(0.0f, 0.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(1.0f, 1.0f, 1.0f);
	perObject.Material.Emissive = Vector3(0.0f);
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Roughness = 0.2f;
	mpRenderer->setPerObjectBuffer(perObject);
	mPlaneRenderer.Render(mpRenderer);


	mpSkyboxRenderer->Render(mpRenderer);

	mpContactRenderer->Render(mpRenderer);

	mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}