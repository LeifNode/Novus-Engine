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
#include <Input/InputSystem.h>

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
mContactResolver(msMaxContacts * 8),
mRecordNormals(false)
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

	for (auto it = mCollisionSpheres.begin(); it != mCollisionSpheres.end(); ++it)
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

	GeometryGenerator::CreateSphere(1.0f, 30, 30, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreateBox(2.0f, 2.0f, 2.0f, mesh);

	mMeshRenderer2.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreatePlane(1000.0f, 1000.0f, 4, 4, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
}

void RigidBodyTest::InitPhysicsActors()
{
	/*for (int i = 0; i < 10; i++)
	{
		AddSphereActor(Vector3(Math::RandF(-10.0f, 10.0f), 10.0f, Math::RandF(-10.0f, 10.0f)), 1.0f, 50.0f);
	}*/

	InitPhysicsBounds();
	InitPhysicsBoxes();

}

void RigidBodyTest::AddSphereActor(const Vector3& position, float radius, float mass)
{
	RigidBody* body = NE_NEW RigidBody();
	//Set random position and rotation
	body->getTransform()->SetPosition(position);
	body->getTransform()->SetRotation(Quaternion::AxisAngle(Normalize(Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f))), Math::RandF(0.0f, 2.0f * Math::Pi)));
	body->getTransform()->SetScale(radius);
	body->setMass(mass * radius);

	Vector3 halfScale = Vector3(0.5f);

	//Matrix3 tensor = Matrix3::BoxInertiaTensor(halfScale, body->getMass());
	Matrix3 tensor = Matrix3::SphereSolidInertiaTensor(radius, body->getMass());
	body->setInertiaTensor(tensor);

	body->ClearAccumilators();

	body->CalculateDerivedData();

	mPhysicsBodies.push_back(body);

	CollisionSphere* boxCollider = NE_NEW CollisionSphere();
	boxCollider->body = body;
	boxCollider->radius = radius;
	//boxCollider->halfSize = halfScale;
	boxCollider->CalculateInternals();

	mCollisionSpheres.push_back(boxCollider);
}

void RigidBodyTest::InitPhysicsBounds()
{
	float bounds = 20.0f;

	//Ground
	CollisionPlane* plane = NE_NEW CollisionPlane();
	plane->direction = Vector3(0.0f, 1.0f, 0.0f);
	//plane->origin = Vector3();
	plane->offset = 0.0f;

	mCollisionHalfSpaces.push_back(plane);
}

void RigidBodyTest::InitPhysicsBoxes()
{
	for (int i = 0; i < 400; i++)
	{
		RigidBody* body = NE_NEW RigidBody();
		//Set random position and rotation
		body->getTransform()->SetPosition(Vector3(Math::RandF(-1.0f, 1.0f), 0.0f, Math::RandF(-1.0f, 1.0f)) * 90.0f);
		body->getTransform()->SetRotation(Quaternion::AxisAngle(Normalize(Vector3(Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f), Math::RandF(-1.0f, 1.0f))), Math::RandF(0.0f, 2.0f * Math::Pi)));
		body->getTransform()->SetScale(Vector3(Math::RandF(), Math::RandF(), Math::RandF()) * 8.0f);
		body->setInverseMass(0.0f);

		Vector3 halfScale = Vector3(1.0f);

		//Matrix3 tensor = Matrix3::BoxInertiaTensor(halfScale, 1.0f);

		Matrix3 tensor = Matrix3::SphereSolidInertiaTensor(1.0f, 1.0f);
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

	for (auto it = mCollisionSpheres.begin(); it != mCollisionSpheres.end(); ++it)
	{
		NE_DELETE(*it);
	}

	for (auto it = mPhysicsBodies.begin(); it != mPhysicsBodies.end(); ++it)
	{
		NE_DELETE(*it);
	}

	mCollisionHalfSpaces.clear();
	mCollisionBoxes.clear();
	mCollisionSpheres.clear();
	mPhysicsBodies.clear();

	InitPhysicsActors();
	InitPhysicsBounds();
	InitPhysicsBoxes();
	mpContactRenderer->points.Clear();
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

	switch (dataPtr->getKey())
	{
	case KeyboardKey::KEY_ESC:
		ExitApp();
		break;
	case KeyboardKey::KEY_R:
		ResetPhysicsSimulation();
		break;
	case KeyboardKey::KEY_N:
		AddSphereActor(Vector3(Math::RandF(-1.0f, 1.0f) * 30.0f, 30.0f, Math::RandF(-1.0f, 1.0f) * 30.0f), Math::RandF(0.7f, 1.5f), 50.0f);
		break;
	case KeyboardKey::KEY_P:
		mRecordNormals = !mRecordNormals;
		mpContactRenderer->points.Clear();
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

		if ((*it)->getInverseMass() != 0.0f)
			(*it)->AddForce(Vector3(0.0f, -9.8f, 0.0f) * (*it)->getMass());//Gravity

		if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_X))
		{
			(*it)->AddForceAtPoint(Vector3(0.0f, -9.8f, 0.0f), (*it)->getTransform()->GetPosition() + Vector3(1.0f, 0.0f, 0.0f));
			mpContactRenderer->points.AddLine(LineSegment((*it)->getTransform()->GetPosition() + Vector3(1.0f, 0.0f, 0.0f), (*it)->getTransform()->GetPosition() + Vector3(1.0f, -1.0f, 0.0f)));
		}
		if (mpInputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_C))
		{
			(*it)->AddForceAtPoint(Vector3(0.0f, -9.8f, 0.0f), (*it)->getTransform()->GetPosition() + Vector3(-1.0f, 0.0f, 0.0f));
			mpContactRenderer->points.AddLine(LineSegment((*it)->getTransform()->GetPosition() + Vector3(-1.0f, 0.0f, 0.0f), (*it)->getTransform()->GetPosition() + Vector3(-1.0f, -1.0f, 0.0f)));
		}
	}

	mpCamera->Update(dt);

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
	//mpContactRenderer->points.Clear();

	for (unsigned int i = 0; i < mCollisionData.contactCount; i++)
	{
		Vector3 endPt = mContacts[i].contactPoint + mContacts[i].contactNormal;
		Vector3 toStartPt = Normalize(mContacts[i].contactPoint - endPt);
		Vector3 arrowEndAxisX = Cross(toStartPt, fabsf((toStartPt).y) < 0.98f ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f));
		Vector3 arrowEndAxisY = Cross(arrowEndAxisX, toStartPt);

		Vector3 arrowEndPt1 = endPt + (arrowEndAxisX + toStartPt) * 0.25f;
		Vector3 arrowEndPt2 = endPt + (-arrowEndAxisX + toStartPt) * 0.25f;
		Vector3 arrowEndPt3 = endPt + (arrowEndAxisY + toStartPt) * 0.25f;
		Vector3 arrowEndPt4 = endPt + (-arrowEndAxisY + toStartPt) * 0.25f;

		Vector3 arrowEndPt5 = mContacts[i].contactPoint + (arrowEndAxisX) * 0.25f;
		Vector3 arrowEndPt6 = mContacts[i].contactPoint + (-arrowEndAxisX) * 0.25f;
		Vector3 arrowEndPt7 = mContacts[i].contactPoint + (arrowEndAxisY) * 0.25f;
		Vector3 arrowEndPt8 = mContacts[i].contactPoint + (-arrowEndAxisY) * 0.25f;

		if (mRecordNormals)
			mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, endPt));

		//mpContactRenderer->points.AddLine(LineSegment(endPt, arrowEndPt1));
		//mpContactRenderer->points.AddLine(LineSegment(endPt, arrowEndPt2));
		//mpContactRenderer->points.AddLine(LineSegment(endPt, arrowEndPt3));
		//mpContactRenderer->points.AddLine(LineSegment(endPt, arrowEndPt4));

		//mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, arrowEndPt5));
		//mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, arrowEndPt6));
		//mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, arrowEndPt7));
		//mpContactRenderer->points.AddLine(LineSegment(mContacts[i].contactPoint, arrowEndPt8));
			

		PointLight light;
		light.Color = Vector3(1.0f);
		light.Intensity = 2.4f;
		light.PositionWorld = mContacts[i].contactPoint + mContacts[i].contactNormal * 0.1f;
		light.Radius = 0.0f;

		mpRenderer->getDeferredRenderer()->getLightManager()->AddLightForFrame(light);

		light.Color = Vector3(1.0f);
		light.Intensity = 2.4f;
		light.PositionWorld = mContacts[i].contactPoint + mContacts[i].contactNormal * -0.1f;
		light.Radius = 0.0f;

		mpRenderer->getDeferredRenderer()->getLightManager()->AddLightForFrame(light);
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

	for (auto it = mCollisionSpheres.cbegin(); it != mCollisionSpheres.cend(); ++it)
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
	mCollisionData.tolerance = 0.01f;

	for (auto it = mCollisionSpheres.cbegin(); it != mCollisionSpheres.cend(); ++it)
	{
		for (auto plane = mCollisionHalfSpaces.cbegin(); plane != mCollisionHalfSpaces.cend(); ++plane)
		{
			if (!mCollisionData.hasMoreContacts()) return;

			CollisionDetector::SphereAndHalfSpace(*(*it), *(*plane), &mCollisionData);
			//CollisionDetector::BoxAndHalfSpace(*(*it), *(*plane), &mCollisionData);
		}

		for (auto other = mCollisionSpheres.cbegin(); other != mCollisionSpheres.cend(); ++other)
		{
			if ((*it)->body != (*other)->body)
			{
				CollisionDetector::SphereAndSphere(*(*it), *(*other), &mCollisionData);
			}
		}

		for (auto other = mCollisionBoxes.cbegin(); other != mCollisionBoxes.cend(); ++other)
		{
			CollisionDetector::BoxAndSphere(*(*other), *(*it), &mCollisionData);
		}
	}

	//std::cout << mCollisionData.contactCount << std::endl;
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

	for (auto it = mCollisionSpheres.cbegin(); it != mCollisionSpheres.cend(); ++it)
	{
		perObject.World = (*it)->body->getTransform()->GetTransform();
		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
		perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		perObject.Material.Emissive = Vector3(0.0f);
		perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.271f);
		perObject.Material.Metallic = 0.0f;
		perObject.Material.Roughness = 0.15f;
		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(mpRenderer);
	}

	for (auto it = mCollisionBoxes.cbegin(); it != mCollisionBoxes.cend(); ++it)
	{
		perObject.World = (*it)->body->getTransform()->GetTransform();
		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
		perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		perObject.Material.Emissive = Vector3(0.0f);
		//perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.271f);
		perObject.Material.SpecularColor = Vector3(0.5f);
		perObject.Material.Metallic = 0.0f;
		perObject.Material.Roughness = 0.15f;
		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer2.Render(mpRenderer);
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